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
#include "indexlib/config/document_deduper_helper.h"

#include "indexlib/config/primary_key_index_config.h"

using namespace std;

namespace indexlib { namespace config {

DocumentDeduperHelper::DocumentDeduperHelper() {}

DocumentDeduperHelper::~DocumentDeduperHelper() {}

bool DocumentDeduperHelper::DelayDedupDocument(const IndexPartitionOptions& options, const IndexConfigPtr& pkConfig)
{
    if (!pkConfig || pkConfig->GetInvertedIndexType() == it_trie || pkConfig->GetInvertedIndexType() == it_kv ||
        pkConfig->GetInvertedIndexType() == it_kkv) {
        return false;
    }
    PrimaryKeyIndexConfigPtr pkIndexConfig = DYNAMIC_POINTER_CAST(PrimaryKeyIndexConfig, pkConfig);
    assert(pkIndexConfig);
    if (pkIndexConfig->GetPrimaryKeyIndexType() == pk_hash_table) {
        return false;
    }
    if (!options.IsOffline()) {
        return false;
    }
    return !options.GetBuildConfig().speedUpPrimaryKeyReader;
}
}} // namespace indexlib::config
