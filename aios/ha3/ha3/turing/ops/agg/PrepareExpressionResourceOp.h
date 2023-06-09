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
#include <memory>
#include <string>

#include "autil/Log.h" // IWYU pragma: keep
#include "ha3/common/Request.h"
#include "indexlib/misc/common.h"
#include "suez/turing/common/SessionResource.h"
#include "suez/turing/expression/function/FunctionInterfaceCreator.h"
#include "suez/turing/expression/util/TableInfo.h"
#include "tensorflow/core/framework/op_kernel.h"

namespace autil {
namespace mem_pool {
class Pool;
}  // namespace mem_pool
}  // namespace autil
namespace indexlib {
namespace partition {
class PartitionReaderSnapshot;
}  // namespace partition
}  // namespace indexlib
namespace suez {
namespace turing {
class SuezCavaAllocator;
class Tracer;
}  // namespace turing
}  // namespace suez

namespace isearch {
namespace turing {
struct ExpressionResource;

typedef std::shared_ptr<ExpressionResource> ExpressionResourcePtr;

class PrepareExpressionResourceOp : public tensorflow::OpKernel
{
public:
    explicit PrepareExpressionResourceOp(tensorflow::OpKernelConstruction* ctx)
        : tensorflow::OpKernel(ctx)
    {}
public:
    void Compute(tensorflow::OpKernelContext* ctx) override;
public:
    static ExpressionResourcePtr createExpressionResource(
            const common::RequestPtr &request,
            const suez::turing::TableInfoPtr &tableInfoPtr,
            const std::string &mainTableName,
            const suez::turing::FunctionInterfaceCreatorPtr &functionInterfaceCreator,
            indexlib::partition::PartitionReaderSnapshot *partitionReaderSnapshot,
            suez::turing::Tracer *tracer,
            const suez::turing::CavaPluginManagerPtr &cavaPluginManager,
            autil::mem_pool::Pool *pool,
            suez::turing::SuezCavaAllocator *cavaAllocator);

private:
    static bool useSubDoc(const common::Request *request,
                          const suez::turing::TableInfo *tableInfo);
private:
    AUTIL_LOG_DECLARE();
};

} // namespace turing
} // namespace isearch