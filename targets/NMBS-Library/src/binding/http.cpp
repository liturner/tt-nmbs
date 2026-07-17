/// @file http.cpp
/// @brief http.cpp brief
/// @details http.cpp details
///
/// @author luke
/// @date 2026-07-18
/// @copyright Copyright (c) 2026 Luke Ian Turner
/// @copyright
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// @copyright
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// @copyright
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

#include "nmbs/nmbs_private.h"

namespace nmbs::binding::http
{
    std::string serialise_header(const std::vector<ConfidentialityLabel>& confidentiality_labels)
    {
        BindingInformation bdo;
        bdo.labels = confidentiality_labels;
        const std::string binding_xml = nmbs::serialisation::serialise_binding_information(bdo);
        const std::string base64_xml = nmbs::serialisation::encode_base64(binding_xml);
        return std::format(
            R"(binding-type="urn:nato:stanag:4778:bindinginformation:1:0 binding-data-object="{}")",
            base64_xml);
    }

    Expected<BindingInformation> deserialise_header(std::string_view binding_data)
    {
        if (binding_data.empty())
        {
            // TODO: Descriptive error type
            return std::unexpected(Error::unexpected());
        }

        constexpr std::string_view key = R"(binding-data-object=")";
        size_t start_pos = binding_data.find(key);
        if (start_pos == std::string_view::npos) {
            // TODO: Descriptive error type
            return std::unexpected(Error::unexpected());
        }

        start_pos += key.length();
        const size_t end_pos = binding_data.find('"', start_pos);
        if (end_pos == std::string_view::npos) {
            // TODO: Descriptive error type
            return std::unexpected(Error::unexpected());
        }

        const std::string_view base64_xml = binding_data.substr(start_pos, end_pos - start_pos);
        const std::string xml = nmbs::serialisation::decode_base64(base64_xml);

        return nmbs::serialisation::deserialise_binding_information(xml);
    }
}
