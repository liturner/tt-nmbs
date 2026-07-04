/// @file basic.tests.cpp
/// @brief Misc tests
///
/// @author Luke Ian Turner
/// @date 2026-06-10
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

TEST(XMP, Read)
{
    const auto labels = nmbs::read_labels("resources/test-public-unmarked.jpg").value();
    ASSERT_EQ(labels.size(), 1);
    ASSERT_EQ(labels[0].label_type, nmbs::confidentiality_label::originator);
    ASSERT_EQ(labels[0].confidentiality_information.policy_identifier, "PUBLIC");
    ASSERT_EQ(labels[0].confidentiality_information.classification, "UNMARKED");
    nmbs::cleanup();
}

TEST(Sidecar, Write)
{
    std::vector<nmbs::confidentiality_label> labels(1);
    labels[0].confidentiality_information.policy_identifier = "PUBLIC";
    labels[0].confidentiality_information.classification = "UNMARKED";
    auto response = nmbs::write_sidecar("resources/test-no-xmp.jpg", labels);
    ASSERT_NE(response, "");
}