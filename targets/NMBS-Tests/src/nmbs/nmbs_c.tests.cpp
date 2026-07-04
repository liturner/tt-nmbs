/// @file nmbs_c.tests.cpp
/// @brief Tests for the C API
///
/// @author Luke Ian Turner
/// @date 2026-07-01
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

TEST(C, NewDeleteConfidentialityLabels)
{
    auto labels = nmbs_confidentiality_labels_new();
    ASSERT_NE(labels, nullptr);
    nmbs_confidentiality_labels_delete(labels);
    ASSERT_NE(labels, nullptr);
}

TEST(C, ReadXmpLabels)
{
    auto labels = nmbs_confidentiality_labels_new();
    ASSERT_NE(labels, nullptr);
    nmbs_confidentiality_labels_read_labels(labels, "resources/test-public-unmarked.jpg");
    ASSERT_EQ(nmbs_confidentiality_labels_size(labels), 1);
    auto label = nmbs_confidentiality_labels_get(labels, 0);
    ASSERT_STREQ(nmbs_confidentiality_label_get_policy(label), "PUBLIC");
    ASSERT_STREQ(nmbs_confidentiality_label_get_classification(label), "UNMARKED");
    nmbs_confidentiality_labels_delete(labels);
    nmbs::cleanup();
}
