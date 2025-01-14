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
#ifndef __INDEXLIB_TABLE_IDENTIFIER_H
#define __INDEXLIB_TABLE_IDENTIFIER_H

#include <memory>

#include "indexlib/common_define.h"
#include "indexlib/indexlib.h"

DECLARE_REFERENCE_CLASS(config, TableIdentifierImpl);

namespace indexlib { namespace config {

class TableIdentifier
{
public:
    struct Partition {
        uint16_t from = 0;
        uint16_t to = 65535;
        Partition(uint16_t f, uint16_t t);
    };

public:
    TableIdentifier(const std::string& id, TableType type, const std::string& name,
                    TableIdentifier::Partition partition);
    ~TableIdentifier();

public:
    void SetId(const std::string& id);
    void SetTableName(const std::string& name);
    void SetTableType(TableType type);
    void SetPartition(const TableIdentifier::Partition& partition);

public:
    const std::string& GetId() const;
    const std::string& GetTableName() const;
    TableType GetTableType() const;
    const TableIdentifier::Partition& GetPartition() const;

private:
    TableIdentifierImplPtr mImpl;

private:
    IE_LOG_DECLARE();
};

DEFINE_SHARED_PTR(TableIdentifier);
}} // namespace indexlib::config

#endif //__INDEXLIB_TABLE_IDENTIFIER_H
