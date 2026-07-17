/// @file access_mode.tests.cpp
/// @brief access_mode.tests.cpp brief
/// @details access_mode.tests.cpp details
///
/// @author Luke Ian Turner
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

#include <nmbs/test.h>

TEST(AccessMode, FileSystem)
{
    auto access_flags = nmbs::binding::access_mode_filesystem("/etc/shadow");

    ASSERT_EQ(access_flags, 0);
    ASSERT_EQ(access_flags, nmbs::binding::am_none);
    ASSERT_EQ(access_flags & nmbs::binding::am_read, 0);
    ASSERT_EQ(access_flags & nmbs::binding::am_write, 0);

    access_flags = nmbs::binding::access_mode_filesystem("/etc/passwd");

    ASSERT_NE(access_flags & nmbs::binding::am_read, 0);
    ASSERT_EQ(access_flags & nmbs::binding::am_write, 0);

    access_flags = nmbs::binding::access_mode_filesystem("resources/test-no-xmp.jpg");

    ASSERT_NE(access_flags & nmbs::binding::am_read, 0);
    ASSERT_NE(access_flags & nmbs::binding::am_write, 0);
}

TEST(AccessMode, XMP)
{
    auto access_flags = nmbs::binding::xmp::access_mode("/etc/shadow");

    ASSERT_EQ(access_flags, 0);
    ASSERT_EQ(access_flags, nmbs::binding::am_none);
    ASSERT_EQ(access_flags & nmbs::binding::am_read, 0);
    ASSERT_EQ(access_flags & nmbs::binding::am_write, 0);

    access_flags = nmbs::binding::xmp::access_mode("/etc/passwd");

    ASSERT_EQ(access_flags & nmbs::binding::am_read, 0);
    ASSERT_EQ(access_flags & nmbs::binding::am_write, 0);

    access_flags = nmbs::binding::xmp::access_mode("resources/test-no-xmp.jpg");

    ASSERT_NE(access_flags & nmbs::binding::am_read, 0);
    ASSERT_NE(access_flags & nmbs::binding::am_write, 0);
}

TEST(AccessMode, Sidecar)
{
    auto access_flags = nmbs::binding::sidecar::access_mode("/etc/shadow");

    ASSERT_NE(access_flags & nmbs::binding::am_read, 0);
    ASSERT_EQ(access_flags & nmbs::binding::am_write, 0);

    access_flags = nmbs::binding::sidecar::access_mode("/root/dummy");

    ASSERT_EQ(access_flags & nmbs::binding::am_read, 0);
    ASSERT_EQ(access_flags & nmbs::binding::am_write, 0);

    access_flags = nmbs::binding::sidecar::access_mode("resources/test-no-xmp.jpg");

    ASSERT_NE(access_flags & nmbs::binding::am_read, 0);
    ASSERT_NE(access_flags & nmbs::binding::am_write, 0);

    access_flags = nmbs::binding::sidecar::access_mode("resources/xml/test.1");

    ASSERT_NE(access_flags & nmbs::binding::am_read, 0);
    ASSERT_NE(access_flags & nmbs::binding::am_write, 0);

    access_flags = nmbs::binding::sidecar::access_mode("/home");

    ASSERT_EQ(access_flags & nmbs::binding::am_read, 0);
    ASSERT_EQ(access_flags & nmbs::binding::am_write, 0);

    access_flags = nmbs::binding::sidecar::access_mode("8ite m3");

    ASSERT_EQ(access_flags & nmbs::binding::am_read, 0);
    ASSERT_EQ(access_flags & nmbs::binding::am_write, 0);
}
