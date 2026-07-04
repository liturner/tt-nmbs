/// @file nmbs_private.h
/// @brief Private nmbs declarations, not intended to be exposed to the public API.
///
/// @author Luke Ian Turner
/// @date 2026-06-11
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

#pragma once

#include "nmbs/nmbs.h"
#include "nmbs/binding.h"

#include <string>

#include "nmbs/expected.h"
#include "nmbs/spif.h"

namespace nmbs
{
    [[nodiscard]] expected<std::string> write_xmp(const std::filesystem::path& path, const std::vector<confidentiality_label>& confidentiality_labels);

    [[nodiscard]] expected<std::string> write_sidecar(const std::filesystem::path& path, const std::vector<confidentiality_label>& confidentiality_labels);

    [[nodiscard]] std::vector<confidentiality_label> read_xmp(const std::filesystem::path& path);

    [[nodiscard]] expected<std::string> read_xmp_xml(const std::filesystem::path& path);

    [[nodiscard]] std::vector<confidentiality_label> read_sidecar(const std::filesystem::path& path);

    [[nodiscard]] expected<std::string> read_sidecar_xml(const std::filesystem::path& path);

    namespace xml
    {
        [[nodiscard]] expected<binding::binding_information> deserialise_binding_information(const std::string& xml);

        [[nodiscard]] expected<spif::security_policy> deserialise_security_policy(const std::string& xml);

        [[nodiscard]] std::string encode_base64(const std::string& input);

        [[nodiscard]] std::string decode_base64(std::string_view input);

        [[nodiscard]] std::string serialise_binding_information(const binding::binding_information& binding_information);

        [[nodiscard]] std::string serialise_confidentiality_labels(const std::vector<confidentiality_label>& confidentiality_labels);

        [[nodiscard]] std::string serialise_confidentiality_label(const confidentiality_label& confidentiality_label);

    }

}
