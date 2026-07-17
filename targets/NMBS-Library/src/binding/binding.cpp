/// @file binding.cpp
/// @brief binding.cpp brief
/// @details binding.cpp details
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

#include <unistd.h>

namespace nmbs::binding
{
    [[nodiscard]] AccessMode access_mode_filesystem(const std::filesystem::path& path)
    {
        AccessMode result_flags = am_none;
        if (::access(path.c_str(), R_OK) == 0) result_flags |= am_read;
        if (::access(path.c_str(), W_OK) == 0) result_flags |= am_write;
        return result_flags;
    }

    ProfileSupport support(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path)) [[unlikely]]
        {
            return ps_none;
        }

        if (!std::filesystem::is_regular_file(path))
        {
            return ps_none;
        }

        ProfileSupport result_flags = ps_none;

        if (sidecar::supported(path))
        {
            result_flags |= ps_sidecar;
        }
        if (xmp::supported(path))
        {
            result_flags |= ps_xmp;
        }
        if (xml::supported(path))
        {
            result_flags |= ps_xml;
        }

        return result_flags;
    }

}