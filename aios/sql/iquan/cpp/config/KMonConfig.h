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

#include "autil/legacy/jsonizable.h"
#include "iquan/common/Common.h"

namespace iquan {

class KMonConfig : public autil::legacy::Jsonizable {
public:
    KMonConfig() : serviceName(""), tenantName("default"), flumeAddress("127.0.0.1:4141"), autoRecycle(true) {}

    void Jsonize(autil::legacy::Jsonizable::JsonWrapper &json) override {
        json.Jsonize("service_name", serviceName, serviceName);
        json.Jsonize("tenant_name", tenantName, tenantName);
        json.Jsonize("flume_address", flumeAddress, flumeAddress);
        json.Jsonize("auto_recycle", autoRecycle, autoRecycle);
        json.Jsonize("global_tags", globalTags, globalTags);
    }

    bool isValid() const {
        if (serviceName.empty() || tenantName.empty() || flumeAddress.empty()) {
            return false;
        }
        return true;
    }

public:
    std::string serviceName;
    std::string tenantName;
    std::string flumeAddress;
    bool autoRecycle;
    std::string globalTags;
};

} // namespace iquan
