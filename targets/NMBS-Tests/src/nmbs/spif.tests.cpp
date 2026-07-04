/// @file spif.tests.cpp
/// @brief Tests for spif Security Profile support
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

const std::string dummy_spif_1{
    R"(<?xml version="1.0" encoding="UTF-8"?>
<spif:SPIF
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xmlns:spif="http://www.xmlspif.org/spif"
	schemaVersion="1.0"
	version="5">
	<spif:securityPolicyId
		name="TT"
		id="temp"/>
	<spif:securityClassifications>
		<spif:securityClassification
			name="UNMARKED"
			lacv="0"
			hierarchy="0">
		</spif:securityClassification>
        <spif:securityClassification
			name="SECRET"
			lacv="1"
			hierarchy="1">
		</spif:securityClassification>
        <spif:securityClassification
			name="SAUCY"
			lacv="3"
			hierarchy="2"
            obsolete="true"
            color="red">
		</spif:securityClassification>
	</spif:securityClassifications>
</spif:SPIF>
)"
};

const std::string error_spif_lacv_1{
    R"(<?xml version="1.0" encoding="UTF-8"?>
<spif:SPIF
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xmlns:spif="http://www.xmlspif.org/spif"
	schemaVersion="1.0"
	version="1">
	<spif:securityPolicyId
		name="TT"
		id="temp"/>
	<spif:securityClassifications>
		<spif:securityClassification
			name="UNMARKED"
			lacv="string"
			hierarchy="0">
		</spif:securityClassification>
	</spif:securityClassifications>
</spif:SPIF>
)"
};

TEST(SPIF, Deserialise)
{
    const nmbs::spif::security_policy tt_policy = nmbs::xml::deserialise_security_policy(dummy_spif_1).value();

    ASSERT_EQ(tt_policy.name, "TT");
    ASSERT_EQ(tt_policy.id, "temp");
    ASSERT_EQ(tt_policy.version, 5);
    ASSERT_EQ(tt_policy.security_classifications.size(), 3);
    ASSERT_EQ(tt_policy.security_classifications[0].name, "UNMARKED");
    ASSERT_EQ(tt_policy.security_classifications[0].lacv, 0);
    ASSERT_EQ(tt_policy.security_classifications[0].hierarchy, 0);
    ASSERT_EQ(tt_policy.security_classifications[0].colour, std::nullopt);
    ASSERT_EQ(tt_policy.security_classifications[0].obsolete, false);
    ASSERT_EQ(tt_policy.security_classifications[1].name, "SECRET");
    ASSERT_EQ(tt_policy.security_classifications[1].lacv, 1);
    ASSERT_EQ(tt_policy.security_classifications[1].hierarchy, 1);
    ASSERT_EQ(tt_policy.security_classifications[1].colour, std::nullopt);
    ASSERT_EQ(tt_policy.security_classifications[1].obsolete, false);
    ASSERT_EQ(tt_policy.security_classifications[2].name, "SAUCY");
    ASSERT_EQ(tt_policy.security_classifications[2].lacv, 3);
    ASSERT_EQ(tt_policy.security_classifications[2].hierarchy, 2);
    ASSERT_EQ(tt_policy.security_classifications[2].colour, "red");
    ASSERT_EQ(tt_policy.security_classifications[2].obsolete, true);
}

TEST(SPIF, DeserialiseLacvIntFalse)
{
    ASSERT_FALSE(nmbs::xml::deserialise_security_policy(error_spif_lacv_1).has_value());
}

TEST(SPIF, Globals)
{
    setenv(std::string(nmbs::spif::override_spif_location_env_var).c_str(), "resources/spif", 1);

    auto& policies = nmbs::spif::get_security_policies();

    ASSERT_EQ(policies.size(), 2);
}
