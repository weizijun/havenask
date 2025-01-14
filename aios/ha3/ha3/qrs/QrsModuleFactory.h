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

#include <string>

#include "build_service/plugin/ModuleFactory.h"

#include "ha3/isearch.h"
#include "autil/Log.h" // IWYU pragma: keep

namespace isearch {
namespace qrs {
class QrsProcessor;

static const std::string MODULE_FUNC_QRS = "_Qrs";
class QrsModuleFactory : public build_service::plugin::ModuleFactory
{
// public:
//     QrsModuleFactory();
//     virtual ~QrsModuleFactory();
public:
    virtual bool init(const KeyValueMap &parameters) = 0;
    virtual void destroy() = 0;

    virtual QrsProcessor* createQrsProcessor(const std::string &name) = 0;
private:
    AUTIL_LOG_DECLARE();
};

// extern "C" ModuleFactory* createFactory();
// extern "C" void destroyFactory(ModuleFactory *factory);

} // namespace qrs
} // namespace isearch

