/// @file binding.h
/// @brief binding.h brief
/// @details binding.h details
///
/// @author luke
/// @date 2026-07-17
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

namespace nmbs::binding
{
    typedef std::uint32_t AccessMode;
    enum AccessModeFlags : AccessMode
    {
        am_none = 0,

        am_read = 1 << 0,

        am_write = 1 << 1
    };


    typedef std::uint32_t ProfileSupport;
    enum ProfileSupportFlags : ProfileSupport
    {

        /// @brief The file does not support ADatP-4774 labels (at least in this implementation...)
        /// @since 1.0.0
        ps_none = 0,

        /// @brief The file supports the XMP binding profile.
        /// @since 1.0.0
        ps_xmp = 1 << 0,

        /// @brief The file supports the Sidecar binding profile.
        /// @since 1.0.0
        ps_sidecar = 1 << 1,

        /// @brief The file supports the XML binding profile.
        /// @since 1.0.0
        ps_xml = 1 << 2,

        /// @brief The file supports the EXT4 binding profile.
        /// @since 1.0.0
        ps_ext4 = 1 << 3

    };


    /// @brief File analysis to check the binding capabilities of the path.
    /// @details Using nmbs::binding::ProfileSupportFlags you can then use classic binary logic
    /// to see which binding profiles could potentially be supported by this file. Note that this
    /// ignores rights! For example, sidecar will be reported as supported for files in /etc/...
    /// as it is entirely possible that a readonly file can have a sidecar. Users should use the
    /// relevant "access_mode" function checks to verify if a binding profile has e.g. read or
    /// write access.
    ///
    /// This function is written for speed, but it does necessarily perform several IO operations.
    /// Care should be taken to cache the result if possible.
    /// @since 1.0.0
    /// @param path to analyse
    /// @return nmbs::binding::flags containing all information about the path parameter
    [[nodiscard]] ProfileSupport support(const std::filesystem::path& path);

}
