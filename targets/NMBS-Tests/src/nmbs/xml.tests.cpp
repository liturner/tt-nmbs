/// @file xml.tests.cpp
/// @brief Tests for the nmbs::xml namespace
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

const std::string binding_information_1{R"(<?xml version="1.0" encoding="UTF-8"?>
<s4778:BindingInformation xmlns:s4778="urn:nato:stanag:4778:bindinginformation:1:0">
    <s4778:MetadataBindingContainer>
        <s4778:MetadataBinding>
            <s4778:Metadata>
                <s4774:originatorConfidentialityLabel
                        xmlns:s4774="urn:nato:stanag:4774:confidentialitymetadatalabel:1:0">
                    <s4774:ConfidentialityInformation>
                        <s4774:PolicyIdentifier>PUBLIC</s4774:PolicyIdentifier>
                        <s4774:Classification>UNMARKED</s4774:Classification>
                    </s4774:ConfidentialityInformation>
                    <s4774:CreationDateTime>2026-06-26T12:47:51Z</s4774:CreationDateTime>
                </s4774:originatorConfidentialityLabel>
            </s4778:Metadata>
            <s4778:DataReference URI="./test-no-xmp.jpg.bdo"/>
        </s4778:MetadataBinding>
    </s4778:MetadataBindingContainer>
</s4778:BindingInformation>
)"};

const std::string binding_information_2{R"(<?xml version="1.0" encoding="UTF-8"?>
<s4778:BindingInformation xmlns:s4778="urn:nato:stanag:4778:bindinginformation:1:0">
    <s4778:MetadataBindingContainer>
        <s4778:MetadataBinding>
            <s4778:Metadata>
                <s4774:originatorConfidentialityLabel
                        xmlns:s4774="urn:nato:stanag:4774:confidentialitymetadatalabel:1:0">
                    <s4774:ConfidentialityInformation>
                        <s4774:PolicyIdentifier>PUBLIC</s4774:PolicyIdentifier>
                        <s4774:Classification>UNMARKED</s4774:Classification>
                    </s4774:ConfidentialityInformation>
                    <s4774:CreationDateTime>2026-06-26T12:47:51Z</s4774:CreationDateTime>
                </s4774:originatorConfidentialityLabel>
                <s4774:alternativeConfidentialityLabel
                        xmlns:s4774="urn:nato:stanag:4774:confidentialitymetadatalabel:1:0">
                    <s4774:ConfidentialityInformation>
                        <s4774:PolicyIdentifier>ESOTERIC</s4774:PolicyIdentifier>
                        <s4774:Classification>UNINTELLIGIBLE</s4774:Classification>
                    </s4774:ConfidentialityInformation>
                    <s4774:OriginatorID IDType="rfc822Name">adam.weishaupt@bienenorden.de</s4774:OriginatorID>
                    <s4774:CreationDateTime>2026-06-26T12:47:51Z</s4774:CreationDateTime>
                </s4774:alternativeConfidentialityLabel>
            </s4778:Metadata>
            <s4778:DataReference URI="./test-no-xmp.jpg.bdo"/>
        </s4778:MetadataBinding>
    </s4778:MetadataBindingContainer>
</s4778:BindingInformation>
)"};

TEST(XML, Namespaces)
{
    /// This value may never change. You may refactor the variable name, or add another namespace in a different
    /// variable, but the namespace itself may never be modified. It is defined by the standard ADatP-4774 Ed.A V.1.
    ASSERT_EQ(nmbs::binding::s4774_namespace, "urn:nato:stanag:4774:confidentialitymetadatalabel:1:0");
}

TEST(XML, DeserialiseSingleLabel)
{
    NMBS_REQUIREMENT_STANDARD_4778_2_A1

    const nmbs::binding::BindingInformation bdo = nmbs::serialisation::deserialise_binding_information(binding_information_1).value();

    // 2026-06-26T12:47:51Z
    constexpr std::chrono::utc_seconds expected_utc_time{std::chrono::seconds{1782478098}};
    ASSERT_EQ(bdo.labels.size(), 1);
    ASSERT_EQ(bdo.labels[0].label_type, nmbs::ConfidentialityLabel::originator);
    ASSERT_EQ(bdo.labels[0].confidentiality_information.policy_identifier, "PUBLIC");
    ASSERT_EQ(bdo.labels[0].confidentiality_information.classification, "UNMARKED");
    ASSERT_EQ(bdo.labels[0].creation_date_time, expected_utc_time);
    ASSERT_EQ(bdo.labels[0].originator_id, std::nullopt);
    ASSERT_EQ(bdo.reference.uri, "./test-no-xmp.jpg.bdo");
}

TEST(XML, DeserialiseSingleLabelFile)
{
    NMBS_REQUIREMENT_STANDARD_4778_2_A1

    const nmbs::binding::BindingInformation bdo = nmbs::serialisation::deserialise_binding_information_from_file(std::filesystem::path("resources/xml/test.1.bdo")).value().value();

    // 2026-06-26T12:47:51Z
    constexpr std::chrono::utc_seconds expected_utc_time{std::chrono::seconds{1782478098}};
    ASSERT_EQ(bdo.labels.size(), 1);
    ASSERT_EQ(bdo.labels[0].label_type, nmbs::ConfidentialityLabel::originator);
    ASSERT_EQ(bdo.labels[0].confidentiality_information.policy_identifier, "PUBLIC");
    ASSERT_EQ(bdo.labels[0].confidentiality_information.classification, "UNMARKED");
    ASSERT_EQ(bdo.labels[0].creation_date_time, expected_utc_time);
    ASSERT_EQ(bdo.labels[0].originator_id, std::nullopt);
    ASSERT_EQ(bdo.reference.uri, "./a-missing.file.bdo");
}

TEST(XML, DeserialiseDualLabel)
{
    NMBS_REQUIREMENT_STANDARD_4778_2_A1

    const nmbs::binding::BindingInformation bdo = nmbs::serialisation::deserialise_binding_information(binding_information_2).value();

    // 2026-06-26T12:47:51Z
    constexpr std::chrono::utc_seconds expected_utc_time{std::chrono::seconds{1782478098}};
    ASSERT_EQ(bdo.labels.size(), 2);
    ASSERT_EQ(bdo.labels[0].label_type, nmbs::ConfidentialityLabel::originator);
    ASSERT_EQ(bdo.labels[0].confidentiality_information.policy_identifier, "PUBLIC");
    ASSERT_EQ(bdo.labels[0].confidentiality_information.classification, "UNMARKED");
    ASSERT_EQ(bdo.labels[0].creation_date_time, expected_utc_time);
    ASSERT_EQ(bdo.labels[0].originator_id, std::nullopt);
    ASSERT_EQ(bdo.labels[1].label_type, nmbs::ConfidentialityLabel::alternative);
    ASSERT_EQ(bdo.labels[1].confidentiality_information.policy_identifier, "ESOTERIC");
    ASSERT_EQ(bdo.labels[1].confidentiality_information.classification, "UNINTELLIGIBLE");
    ASSERT_EQ(bdo.labels[1].creation_date_time, expected_utc_time);
    ASSERT_TRUE(bdo.labels[1].originator_id.has_value());
    ASSERT_EQ(bdo.labels[1].originator_id->id_type, nmbs::ConfidentialityLabel::OriginatorId::rfc822_name);
    ASSERT_EQ(bdo.labels[1].originator_id->value, "adam.weishaupt@bienenorden.de");
    ASSERT_EQ(bdo.reference.uri, "./test-no-xmp.jpg.bdo");
}
