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

#include <stddef.h>
#include <stdint.h>
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "autil/Log.h"
#include "ha3/sql/ops/join/JoinBase.h"
#include "ha3/sql/proto/SqlSearchInfo.pb.h"
#include "navi/common.h"
#include "navi/engine/Kernel.h"
#include "navi/engine/KernelConfigContext.h"
#include "table/Table.h"

namespace kmonitor {
class MetricsReporter;
} // namespace kmonitor
namespace navi {
class GraphMemoryPoolResource;
class GigQuerySessionR;
class KernelComputeContext;
class KernelDefBuilder;
class KernelInitContext;
} // namespace navi

namespace isearch {
namespace sql {
class ObjectPoolResource;
class SqlBizResource;
class SqlQueryResource;
class MetaInfoResource;
class SqlConfigResource;
class SqlSearchInfoCollector;
class TabletManagerR;
} // namespace sql
} // namespace isearch

namespace isearch {
namespace sql {

class JoinKernelBase : public navi::Kernel {
public:
    static const int DEFAULT_BATCH_SIZE;

public:
    typedef std::vector<std::pair<size_t, size_t>> HashValues; // row : hash value

public:
    JoinKernelBase();
    virtual ~JoinKernelBase();

public:
    void def(navi::KernelDefBuilder &builder) const override;
    bool config(navi::KernelConfigContext &ctx) override;
    navi::ErrorCode init(navi::KernelInitContext &initContext) override;
    navi::ErrorCode compute(navi::KernelComputeContext &runContext) override;

protected:
    inline void joinRow(size_t leftIndex, size_t rightIndex);
    void reserveJoinRow(size_t rowCount);
    bool createHashMap(const table::TablePtr &table,
                       size_t offset, size_t count, bool hashLeftTable);
    bool getHashValues(const table::TablePtr &table,
                       size_t offset,
                       size_t count,
                       const std::vector<std::string> &columnName,
                       HashValues &values);
    bool getColumnHashValues(const table::TablePtr &table,
                             size_t offset,
                             size_t count,
                             const std::string &columnName,
                             HashValues &values);
    void combineHashValues(const HashValues &valuesA, HashValues &valuesB);

    // lookup join or left multi join
    bool getLookupInput(navi::KernelComputeContext &runContext,
                        const navi::PortIndex &portIndex,
                        bool leftTable,
                        table::TablePtr &inputTable,
                        bool &eof);
    // hash join or nested loop join
    bool getInput(navi::KernelComputeContext &runContext,
                  const navi::PortIndex &portIndex,
                  bool leftTable,
                  size_t bufferLimitSize,
                  table::TablePtr &inputTable,
                  bool &eof);

    static bool canTruncate(size_t joinedCount, size_t truncateThreshold);
protected:
    virtual void reportMetrics();
    void incTotalLeftInputTable(size_t count);
    void incTotalRightInputTable(size_t count);

private:
    bool convertFields();
    navi::ErrorCode initCalcTableAndJoinColumns();
    void patchHintInfo(const std::map<std::string, std::string> &hintsMap);
protected:
    std::string _joinType;
    std::string _semiJoinType;
    std::string _conditionJson;
    std::string _equiConditionJson;
    std::string _remainConditionJson;
    std::vector<int32_t> _reuseInputs;

    // join parameters
    JoinBaseParam _joinBaseParam;
    JoinInfo _joinInfo;
    std::vector<std::string> _outputFields;

    std::vector<size_t> _tableAIndexes;
    std::vector<size_t> _tableBIndexes;
    std::vector<std::string> _leftJoinColumns;
    std::vector<std::string> _rightJoinColumns;
    std::map<std::string, std::string> _hashHints;
    std::map<std::string, std::pair<std::string, bool>> _output2InputMap;
    std::unordered_map<size_t, std::vector<size_t>> _hashJoinMap;

    JoinBasePtr _joinPtr;
    size_t _joinIndex;
    size_t _systemFieldNum;
    size_t _batchSize;
    size_t _truncateThreshold;
    int32_t _opId;
    bool _isEquiJoin;
    bool _shouldClearTable;

    kmonitor::MetricsReporter *_queryMetricsReporter = nullptr;
    SqlSearchInfoCollector *_sqlSearchInfoCollector = nullptr;
    SqlBizResource *_bizResource = nullptr;
    SqlConfigResource *_sqlConfigResource = nullptr;
    SqlQueryResource *_queryResource = nullptr;
    ObjectPoolResource *_objectPoolResource = nullptr;
    navi::GraphMemoryPoolResource *_memoryPoolResource = nullptr;
    navi::GigQuerySessionR *_naviQuerySessionR = nullptr;
    TabletManagerR *_tabletManagerR = nullptr;
    MetaInfoResource *_metaInfoResource = nullptr;

protected:
    AUTIL_LOG_DECLARE();
};

typedef std::shared_ptr<JoinKernelBase> JoinKernelBasePtr;
void JoinKernelBase::joinRow(size_t leftIndex, size_t rightIndex) {
    _tableAIndexes.emplace_back(leftIndex);
    _tableBIndexes.emplace_back(rightIndex);
}

} // namespace sql
} // namespace isearch
