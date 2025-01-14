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
#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "autil/Log.h"
#include "navi/common.h"
#include "navi/proto/GraphDef.pb.h"
#include "navi/engine/Kernel.h"
#include "navi/engine/KernelConfigContext.h"
#include "navi/resource/GraphMemoryPoolResource.h"
#include "navi/resource/GigClientResource.h"
#include "table/Table.h"
#include "ha3/sql/resource/HashFunctionCacheR.h"
#include "ha3/sql/resource/MetaInfoResource.h"
#include "ha3/sql/ops/tableSplit/TableSplit.h"

namespace isearch {
namespace sql {

class DelayDpKernel : public navi::Kernel {
public:
    struct InputInfo {
        table::TablePtr table;
        std::vector<std::vector<table::Row>> partRows;
        TableSplit splitUtil;
    };
public:
    DelayDpKernel();
    ~DelayDpKernel();

public:
    void def(navi::KernelDefBuilder &builder) const override;
    bool config(navi::KernelConfigContext &ctx) override;
    navi::ErrorCode init(navi::KernelInitContext &initContext) override;
    navi::ErrorCode compute(navi::KernelComputeContext &runContext) override;

private:
    bool initStringAttr(
        navi::KernelConfigContext &ctx,
        const std::string &key,
        std::string &value);
    bool initVectorAttr(
        navi::KernelConfigContext &ctx,
        const std::string &key,
        std::vector<std::string> &values);
    bool initGraphDef(navi::KernelConfigContext &ctx);
    bool mergeInputData(navi::GroupDatas &datas);
    bool splitInputData();
    bool redirectGraphInput(std::vector<navi::OverrideData> &overrideDatas);
    bool partHasData(int partId);
    void addOverrideDatas(
        int partId,
        std::vector<navi::OverrideData> &overrideDatas);
    navi::ErrorCode forkGraph(navi::KernelComputeContext &runContext);

private:
    std::vector<std::string> _inputNames;
    std::vector<TableDistribution> _inputDists;
    std::vector<std::string> _outputNames;
    std::unique_ptr<navi::GraphDef> _graphDef;
    bool _debug{false};
    int _partCnt{1};

    std::vector<InputInfo> _inputInfos;
    bool _broadcast{true};
    std::vector<int> _validPartIds;

    navi::GraphMemoryPoolResource *_memoryPoolResource{nullptr};
    navi::GigClientResource *_gigClientResource = nullptr;
    HashFunctionCacheR *_hashFunctionCacheR = nullptr;
    MetaInfoResource *_metaInfoResource{nullptr};
private:
    AUTIL_LOG_DECLARE();
};

} // namespace sql
} // namespace isearch