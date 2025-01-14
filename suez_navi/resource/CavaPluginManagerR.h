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

#include "navi/engine/Resource.h"
#include "suez_navi/resource/CavaJitWrapperR.h"
#include "suez_navi/resource/MetricsReporterR.h"
#include "suez_navi/resource/CavaConfig.h"
#include "suez/turing/expression/cava/common/CavaPluginManager.h"
#include "suez/turing/expression/function/FunctionInfo.h"

namespace suez_navi {

class FunctionInfo : public suez::turing::FunctionInfo {
public:
    using suez::turing::FunctionInfo::Jsonize;
    void Jsonize(navi::NaviConfigContext& ctx) {
        ctx.Jsonize("name", funcName, funcName);
        ctx.Jsonize("class_name", className, className);
        ctx.Jsonize("module_name", moduleName, moduleName);
        ctx.Jsonize("parameters", params, params);
    }
};

class CavaPluginManagerR : public navi::Resource
{
public:
    CavaPluginManagerR();
    ~CavaPluginManagerR();
    CavaPluginManagerR(const CavaPluginManagerR &) = delete;
    CavaPluginManagerR &operator=(const CavaPluginManagerR &) = delete;
public:
    void def(navi::ResourceDefBuilder &builder) const override;
    bool config(navi::ResourceConfigContext &ctx) override;
    navi::ErrorCode init(navi::ResourceInitContext &ctx) override;
public:
    const suez::turing::CavaPluginManagerPtr &getManager() const;
public:
    static const std::string RESOURCE_ID;
private:
    std::vector<suez::turing::FunctionInfo> _functionInfos;
    MetricsReporterR *_metricReporterR;
    CavaJitWrapperR *_cavaJitWrapperR;
    CavaConfig _cavaConfig;
    suez::turing::CavaPluginManagerPtr _manager;
};
NAVI_TYPEDEF_PTR(CavaPluginManagerR);

}
