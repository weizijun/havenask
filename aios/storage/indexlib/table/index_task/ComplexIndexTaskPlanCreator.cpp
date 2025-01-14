/*
 * Copyright 2014-present Alibaba Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "indexlib/table/index_task/ComplexIndexTaskPlanCreator.h"

#include "indexlib/config/IndexTaskConfig.h"
#include "indexlib/config/TabletOptions.h"
#include "indexlib/framework/TabletData.h"
#include "indexlib/framework/index_task/IIndexOperationCreator.h"
#include "indexlib/framework/index_task/IndexOperation.h"
#include "indexlib/framework/index_task/IndexTaskContext.h"
#include "indexlib/framework/index_task/IndexTaskHistory.h"
#include "indexlib/framework/index_task/IndexTaskPlan.h"
#include "indexlib/table/index_task/IndexTaskConstant.h"
#include "indexlib/table/index_task/SimpleIndexTaskPlanCreator.h"

namespace indexlibv2::table {
AUTIL_LOG_SETUP(indexlib.table, ComplexIndexTaskPlanCreator);

struct SortItemForCandidateTask {
    ComplexIndexTaskPlanCreator::SimpleTaskItem taskItem;
    int64_t taskPriority;
};

#define TABLET_LOG(level, format, args...)                                                                             \
    AUTIL_LOG(level, "[%s] " format, taskContext->GetTabletData()->GetTabletName().c_str(), ##args)

ComplexIndexTaskPlanCreator::ComplexIndexTaskPlanCreator() {}

ComplexIndexTaskPlanCreator::~ComplexIndexTaskPlanCreator() {}

std::pair<Status, std::unique_ptr<framework::IndexTaskPlan>>
ComplexIndexTaskPlanCreator::CreateTaskPlan(const framework::IndexTaskContext* taskContext)
{
    std::vector<std::pair<std::string, std::string>> tasks;
    RETURN2_IF_STATUS_ERROR(ScheduleSimpleTask(taskContext, &tasks), nullptr, "schedule simple task failed");
    for (auto& taskInfo : tasks) {
        auto& taskType = taskInfo.first;
        auto& taskName = taskInfo.second;
        auto iter = _simpleCreatorFuncMap.find(taskType);
        if (iter == _simpleCreatorFuncMap.end()) {
            TABLET_LOG(ERROR, "invalid task type [%s], no task plan created", taskType.c_str());
            return {Status::Unknown(), nullptr};
        }

        std::unique_ptr<SimpleIndexTaskPlanCreator> simpleCreator(iter->second(taskName));
        auto [status, taskPlan] = simpleCreator->CreateTaskPlan(taskContext);
        if (!status.IsOK()) {
            TABLET_LOG(ERROR, "task plan created failed for task [type:%s, name:%s] : %s.", taskType.c_str(),
                       taskName.c_str(), status.ToString().c_str());
            return {status, nullptr};
        }

        if (taskPlan == nullptr) {
            TABLET_LOG(INFO, "no task plan created for task [type:%s, name:%s], skip it.", taskType.c_str(),
                       taskName.c_str());
            continue;
        }

        TABLET_LOG(INFO, "task plan created: task [type:%s, name:%s].", taskType.c_str(), taskName.c_str());
        return {status, std::move(taskPlan)};
    }
    return {Status::OK(), nullptr};
}

Status ComplexIndexTaskPlanCreator::ScheduleSimpleTask(const framework::IndexTaskContext* taskContext,
                                                       std::vector<std::pair<std::string, std::string>>* tasks)
{
    assert(tasks);
    tasks->clear();
    // 0. TODO: check in full build phrase, only trigger full merge strategy

    // * inc task strategy below
    // 1. alter table task is always top priority
    auto tabletData = taskContext->GetTabletData();
    if (tabletData) {
        const auto& version = tabletData->GetOnDiskVersion();
        if (version.GetSchemaId() != version.GetReadSchemaId()) {
            TABLET_LOG(INFO, "version[%d] schema id[%u], read schema id[%u], task type is alter_table",
                       version.GetVersionId(), version.GetSchemaId(), version.GetReadSchemaId());
            tasks->push_back({/*taskType*/ ALTER_TABLE_TASK_TYPE, /*taskName*/ "alter_table"});
            return Status::OK();
        }
    }

    // 2. user designate task goes second priority
    auto designateTask = taskContext->GetDesignateTask();
    if (designateTask != std::nullopt) {
        tasks->push_back(designateTask.value());
        return Status::OK();
    }

    // 3. check index task configs & get candidate tasks with inner priority reorderd
    std::vector<SimpleTaskItem> candidateTasks;
    RETURN_IF_STATUS_ERROR(GetCandidateTasks(taskContext, &candidateTasks), "get candidate task failed");
    for (size_t i = 0; i < candidateTasks.size(); i++) {
        tasks->push_back(candidateTasks[i].identifier);
    }
    auto indexTaskConfigs = taskContext->GetTabletOptions()->GetAllIndexTaskConfigs();
    bool hasMergeTaskConfig = false;
    for (const auto& indexTaskConfig : indexTaskConfigs) {
        if (indexTaskConfig.GetTaskType() == MERGE_TASK_TYPE) {
            hasMergeTaskConfig = true;
            break;
        }
    }
    if (!hasMergeTaskConfig) {
        tasks->push_back({/*taskType*/ MERGE_TASK_TYPE, /*taskName*/ ""}); // default merge strateg
    }
    return Status::OK();
}

Status
ComplexIndexTaskPlanCreator::GetCandidateTasks(const framework::IndexTaskContext* taskContext,
                                               std::vector<ComplexIndexTaskPlanCreator::SimpleTaskItem>* taskItems)
{
    assert(taskItems);
    taskItems->clear();
    int64_t currentTimeInSec = autil::TimeUtility::currentTimeInSeconds();
    auto CalculateTaskPriority = [&taskContext, &currentTimeInSec](const std::string& taskLogType) -> int64_t {
        auto tabletData = taskContext->GetTabletData();
        if (!tabletData) {
            return 0;
        }
        const auto& history = tabletData->GetOnDiskVersion().GetIndexTaskHistory();
        const auto& logItems = history.GetTaskLogs(taskLogType);
        if (logItems.empty()) {
            return currentTimeInSec - history.GetOldestTaskLogTimestamp();
        }
        return currentTimeInSec - logItems[0]->GetTriggerTimestampInSecond();
    };

    // for inc build index tasks
    std::vector<config::IndexTaskConfig> taskConfigs;
    RETURN_IF_STATUS_ERROR(SelectTaskConfigs(taskContext, &taskConfigs), "select task configs error");
    for (auto& taskConfig : taskConfigs) {
        const std::string& taskType = taskConfig.GetTaskType();
        const std::string& taskName = taskConfig.GetTaskName();
        auto iter = _simpleCreatorFuncMap.find(taskType);
        if (iter == _simpleCreatorFuncMap.end()) {
            TABLET_LOG(ERROR, "unknown task type [%s].", taskType.c_str());
            return Status::InvalidArgs("unknown task type [%s].", taskType.c_str());
        }
        std::unique_ptr<SimpleIndexTaskPlanCreator> simpleCreator(iter->second(taskName));
        auto [status, needTrigger] = simpleCreator->NeedTriggerTask(taskConfig, taskContext);
        RETURN_IF_STATUS_ERROR(status, "NeedTriggerTask with error status [%s] for [taskType:%s, taskName:%s]",
                               status.ToString().c_str(), taskType.c_str(), taskName.c_str());
        if (!needTrigger) {
            continue;
        }

        SimpleTaskItem item;
        item.identifier = make_pair(taskType, taskName);
        std::string taskLogType = simpleCreator->ConstructLogTaskType();
        item.priority = CalculateTaskPriority(taskLogType);
        taskItems->push_back(item);
    }
    if (taskItems->size() > 1) {
        std::stable_sort(taskItems->begin(), taskItems->end(),
                         [](auto& lft, auto& rht) { return lft.priority > rht.priority; });
        TABLET_LOG(INFO, "NeedTriggerTask totally get [%lu] tasks.", taskItems->size());
        for (auto& item : *taskItems) {
            TABLET_LOG(INFO, "candidate taskType [%s] taskName [%s] priority [%ld].", item.identifier.first.c_str(),
                       item.identifier.second.c_str(), item.priority);
        }
    }

    return Status::OK();
}

Status ComplexIndexTaskPlanCreator::SelectTaskConfigs(const framework::IndexTaskContext* taskContext,
                                                      std::vector<config::IndexTaskConfig>* configs)
{
    *configs = taskContext->GetTabletOptions()->GetAllIndexTaskConfigs();
    return Status::OK();
}

} // namespace indexlibv2::table
