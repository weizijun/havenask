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

#include <map>
#include <string>

#include "autil/legacy/jsonizable.h"

namespace isearch {
namespace sql {

class SqlQueryPattern : public autil::legacy::Jsonizable {
public:
    SqlQueryPattern();
public:
    void Jsonize(autil::legacy::Jsonizable::JsonWrapper &json) override;
public:
    std::string toCompactJson() const;
public:
    size_t queryHashKey;
    std::string sqlStr;
    std::map<std::string, std::string> _kvPair;
};

} // namespace sql
} // namespace isearch