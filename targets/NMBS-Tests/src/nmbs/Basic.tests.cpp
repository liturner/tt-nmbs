//
// Created by luke on 6/10/26.
//

#include <nmbs/Test.h>

TEST(XML, Namespaces) {

    /// This value may never change. You may refactor the variable name, or add another namespace in a different
    /// variable, but the namespace itself may never be modified. It is defined by the standard ADatP-4774 Ed.A V.1.
    ASSERT_EQ(nmbs::s4774namespace, "urn:nato:stanag:4774:confidentialitymetadatalabel:1:0");

}