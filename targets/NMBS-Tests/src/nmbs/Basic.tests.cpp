//
// Created by luke on 6/10/26.
//

#include <nmbs/Test.h>

TEST(XML, Namespaces) {

    /// This value may never change. You may refactor the variable name, or add another namespace in a different
    /// variable, but the namespace itself may never be modified. It is defined by the standard ADatP-4774 Ed.A V.1.
    ASSERT_EQ(nmbs::constants::s4774_namespace, "urn:nato:stanag:4774:confidentialitymetadatalabel:1:0");

}

TEST(XMP, Read)
{
    const auto labels = nmbs::read_labels("targets/NMBS-Tests/resources/test-has-label.jpg");
    ASSERT_EQ(labels.size(), 1);
    ASSERT_EQ(labels[0].label_type, nmbs::confidentiality_label::originator);
    ASSERT_EQ(labels[0].confidentiality_information.policy_identifier, "PUBLIC");
    ASSERT_EQ(labels[0].confidentiality_information.classification, "UNMARKED");
}