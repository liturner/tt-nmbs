/// @file support.tests.cpp
/// @brief support.tests.cpp brief
/// @details support.tests.cpp details
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

TEST(ProfileSupport, All)
{
    auto support_flags = nmbs::binding::support("/etc/shadow");

    ASSERT_EQ(nmbs::binding::ps_none, 0);
    ASSERT_NE(support_flags | nmbs::binding::ps_none, 0);
    ASSERT_NE(support_flags & nmbs::binding::ps_sidecar, 0);
    ASSERT_EQ(support_flags & nmbs::binding::ps_xml, 0);
    ASSERT_EQ(support_flags & nmbs::binding::ps_xmp, 0);

    support_flags = nmbs::binding::support("resources/test-no-xmp.jpg");

    ASSERT_NE(support_flags | nmbs::binding::ps_none, 0);
    ASSERT_NE(support_flags & nmbs::binding::ps_sidecar, 0);
    ASSERT_EQ(support_flags & nmbs::binding::ps_xml, 0);
    ASSERT_NE(support_flags & nmbs::binding::ps_xmp, 0);

    support_flags = nmbs::binding::support("resources/spif/NATO.spif");

    ASSERT_NE(support_flags | nmbs::binding::ps_none, 0);
    ASSERT_NE(support_flags & nmbs::binding::ps_sidecar, 0);
    ASSERT_NE(support_flags & nmbs::binding::ps_xml, 0);
    ASSERT_EQ(support_flags & nmbs::binding::ps_xmp, 0);

    support_flags = nmbs::binding::support("/usr/share/xml/schema/xml-core/catalog.xml");

    ASSERT_NE(support_flags | nmbs::binding::ps_none, 0);
    ASSERT_NE(support_flags & nmbs::binding::ps_sidecar, 0);
    ASSERT_NE(support_flags & nmbs::binding::ps_xml, 0);
    ASSERT_EQ(support_flags & nmbs::binding::ps_xmp, 0);

    support_flags = nmbs::binding::support("/root");

    ASSERT_EQ(support_flags, nmbs::binding::ps_none);

    support_flags = nmbs::binding::support("trash name");

    ASSERT_EQ(support_flags, nmbs::binding::ps_none);
}

TEST(ProfileSupport, XMP)
{
    ASSERT_FALSE(nmbs::binding::xmp::supported("/usr/share/xml/schema/xml-core/catalog.xml"));
    ASSERT_FALSE(nmbs::binding::xmp::supported("trash name"));
    ASSERT_FALSE(nmbs::binding::xmp::supported("/root"));
    ASSERT_FALSE(nmbs::binding::xmp::supported("/home"));
    ASSERT_TRUE(nmbs::binding::xmp::supported("resources/test-no-xmp.jpg"));
}

TEST(ProfileSupport, XML)
{
    ASSERT_TRUE(nmbs::binding::xml::supported("/usr/share/xml/schema/xml-core/catalog.xml"));
    ASSERT_FALSE(nmbs::binding::xml::supported("trash name"));
    ASSERT_FALSE(nmbs::binding::xml::supported("/root"));
    ASSERT_FALSE(nmbs::binding::xml::supported("/home"));
    ASSERT_FALSE(nmbs::binding::xml::supported("resources/test-no-xmp.jpg"));
}

TEST(ProfileSupport, Sidecar)
{
    ASSERT_TRUE(nmbs::binding::sidecar::supported("/usr/share/xml/schema/xml-core/catalog.xml"));
    ASSERT_FALSE(nmbs::binding::sidecar::supported("trash name"));
    ASSERT_FALSE(nmbs::binding::sidecar::supported("/root"));
    ASSERT_FALSE(nmbs::binding::sidecar::supported("/home"));
    ASSERT_TRUE(nmbs::binding::sidecar::supported("resources/test-no-xmp.jpg"));
}
