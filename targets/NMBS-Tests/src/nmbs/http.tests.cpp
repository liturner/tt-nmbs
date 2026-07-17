/// @file http.tests.cpp
/// @brief Tests for the REST Binding Profile of ADatP-4778.2
///
/// @author Luke Ian Turner
/// @date 2026-06-26
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

TEST(HTTP, DataReferenceContentType)
{
    NMBS_REQUIREMENT_STANDARD_4778_2_A1
    NMBS_REQUIREMENT_TEXT(
        The DataReference xmime:contentType attribute MUST be present with a value of message/http.)

    ASSERT_EQ(nmbs::binding::http::create_data_reference().content_type, "message/http");
}

TEST(HTTP, DataReferenceURI)
{
    NMBS_REQUIREMENT_STANDARD_4778_2_A1
    NMBS_REQUIREMENT_TEXT(
        The BDO is a detached BDO that MUST contain at least one MetadataBinding that
        contains a null DataReference URI attribute value...)

    ASSERT_EQ(nmbs::binding::http::create_data_reference().uri, "");
}

TEST(HTTP, HeaderKey)
{
    NMBS_REQUIREMENT_STANDARD_4778_2_A1
    NMBS_REQUIREMENT_TEXT(
        In the cases where there is a requirement for BDOs to be located in the HTTP
        protocol layer it is RECOMMENDED to use the Binding-Data header field)

    ASSERT_EQ(nmbs::binding::http::header_key, "Binding-Data");
}

TEST(HTTP, BindingDataContent)
{
    NMBS_REQUIREMENT_STANDARD_4778_2_A1
    NMBS_REQUIREMENT_TEXT()

    std::vector<nmbs::ConfidentialityLabel> labels(1);
    labels[0].confidentiality_information.policy_identifier = "PUBLIC";
    labels[0].confidentiality_information.classification = "UNMARKED";
    const std::string binding_data = nmbs::binding::http::serialise_header(labels);

    ASSERT_EQ(binding_data.find("urn:nato:stanag:4778:bindinginformation"), 14);
    ASSERT_EQ(binding_data.find("binding-data-object"), 58);

    // This is a hint that the base64 encoding didn't work.
    ASSERT_GT(binding_data.length(), 100);
}

TEST(HTTP, SerialiseDeserialise)
{
    NMBS_REQUIREMENT_STANDARD_4778_2_A1
    NMBS_REQUIREMENT_TEXT()

    std::vector<nmbs::ConfidentialityLabel> labels(1);
    labels[0].confidentiality_information.policy_identifier = "PUBLIC";
    labels[0].confidentiality_information.classification = "UNMARKED";
    const std::string binding_data = nmbs::binding::http::serialise_header(labels);
    const auto deserialised_labels = nmbs::binding::http::deserialise_header(binding_data);

    ASSERT_EQ(deserialised_labels.value().labels.size(), labels.size());
    ASSERT_EQ(deserialised_labels.value().labels[0], labels[0]);
}
