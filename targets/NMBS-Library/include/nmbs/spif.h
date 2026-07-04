/// @file nmbs.h
/// @brief The intended "include all" header file for libnmbs
/// @details This file will include all the available nmbs namespace. Use it as
/// single import to work with the nmbs namespace.
///
/// @author Luke Ian Turner
/// @date 2026-06-06
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

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace nmbs::spif
{
    ///
    /// @brief Location of the spif files sent with the package manager.
    inline const std::filesystem::path packaged_spif_location = "/usr/share/xml/nmbs/spif";

    ///
    /// @brief Location of spif files additionally read. May be administered by the sys admin.
    inline const std::filesystem::path administered_spif_location = "/etc/nmbs/spif";

    inline constexpr std::string_view override_spif_location_env_var = "NMBS_SPIF_DIR_OVERRIDE";

    struct security_classification
    {
        std::string name;

        std::optional<std::string> colour;

        int lacv;

        int hierarchy;

        bool obsolete = false;
    };

    struct security_policy
    {
        std::string name;

        std::string id;

        int version;

        std::vector<security_classification> security_classifications;
    };

    const std::vector<security_policy>& get_security_policies();
}
