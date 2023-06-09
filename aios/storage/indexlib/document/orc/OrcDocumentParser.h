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

#include "indexlib/document/normal/NormalDocumentParser.h"

namespace indexlib::document {

class OrcDocumentParser : public indexlibv2::document::NormalDocumentParser
{
public:
    OrcDocumentParser(const std::shared_ptr<indexlibv2::analyzer::IAnalyzerFactory>& analyzerFactory,
                      bool needParseRawDoc);
    ~OrcDocumentParser();

private:
    std::unique_ptr<indexlibv2::document::SingleDocumentParser> CreateSingleDocumentParser() const override;

private:
    AUTIL_LOG_DECLARE();
};

} // namespace indexlib::document