#pragma once

/// @file confidentiality_label.h
/// @brief Object representation of a Confidentiality Label as per ADatP-4774
///
/// @author Luke Ian Turner
/// @date 2026-06-11
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

#include <chrono>
#include <string>

#include "exception.h"

namespace nmbs
{
    /// @brief Struct representation of the "Confidentiality Label"
    /// @details More information can be found on this in section 2.4 of ADatP-4774.1 Ed. A V. 1. The
    /// standard specifies three types of label containing identical data. This struct
    /// represents all three originatorConfidentialityLabel, alternativeConfidentialityLabel and
    /// successorConfidentialityLabel. See nmbs::confidentiality_label::confidentiality_label_type
    /// and the nmbs::confidentiality_label::label_type field.
    ///
    /// Noteworthy is that the label couples "confidentiality information" with a timestamp and
    /// additional metadata such as the user and review dates.
    ///
    /// @see nmbs::confidentiality_information_type
    struct confidentiality_label
    {
        ///
        /// @brief identifier for the specialisation of the label.
        /// @details There is no <s4774::ConfidentialityLabel> in the standard, but all specialisations
        /// share the exact same values. The specialisation is used in validation steps with logic for
        /// deciding which classification applies.
        enum confidentiality_label_type
        {
            ///
            /// @brief <s4774::originatorConfidentialityLabel>
            originator,

            ///
            /// @brief <s4774::alternativeConfidentialityLabel>
            alternative,

            ///
            /// @brief <s4774::successorConfidentialityLabel>
            successor
        };

        /// @brief A core element of the nmbs::confidentiality_label containing the classification (e.g. PUBLIC UNMARKED)
        /// @details This struct represents <s4774::ConfidentialityInformation>.
        struct confidentiality_information_type
        {
            ///
            /// @brief <s4774::PolicyIdentifier>
            std::string policy_identifier;

            ///
            /// @brief <s4774::Classification>
            std::string classification;

            ///
            /// @brief <s4774::PrivacyMark>
            std::optional<std::string> privacy_mark;

            ///
            /// @brief <s4774::Category>
            // TODO: List of other structs. category;
        };

        struct originator_id
        {
            enum originator_id_type
            {
                /// @brief An Internet electronic mail address.
                /// @details john.doe@ncia.nato.int
                rfc822_name,

                /// @brief An Internet domain name.
                /// @details ncia.nato.int
                dns_name,

                /// @brief A distinguished name encoded as a string
                /// @details cn=John Doe, ou=NCIA, o=NATO
                directory_name,

                /// @brief A Uniform Resource Identifier (URI) for the World Wide Web.
                /// @details http://www.ncia.nato.int/
                uniform_resource_identifier,

                /// @brief An Internet Protocol address.
                /// @details 192.168.0.1
                ip_address,

                /// @brief An O/R address encoded as a string.
                /// @details /cn=John Doe
                /// /OU=NCIA
                /// /O=NATO
                /// /PRMD=NMS
                /// /C=OO/
                x400_address,

                /// @brief An XMPP address.
                /// @details doe@ncia.nato.int/mobile
                jid,

                /// @brief An Internet-style username format defined by Microsoft.
                /// @details john.doe@nr.ncia.nato.int
                user_principal_name
            };

            originator_id_type id_type;

            std::string value;

            [[nodiscard]] std::string id_type_string() const
            {
                switch (this->id_type)
                {
                case rfc822_name:
                    return "rfc822Name";
                case dns_name:
                    return "dNSName";
                case directory_name:
                    return "directoryName";
                case uniform_resource_identifier:
                    return "uniformResourceIdentifier";
                case ip_address:
                    return "iPAddress";
                case x400_address:
                    return "x400Address";
                case jid:
                    return "jID";
                case user_principal_name:
                    return "userPrincipalName";
                default:
                    throw nmbs::exception(nmbs::exit_code::unknown_error, "Invalid state. An unknown originator_id_type was provided. This should not be po");
                }
            }
        };

        /// Overloaded constructor taking the label type as a parameter.
        /// The label type can be changed later.
        /// @param label_type the type of label this instance will represent
        explicit confidentiality_label(const confidentiality_label_type label_type = originator) :
            label_type(label_type),
            creation_date_time(std::chrono::floor<std::chrono::seconds>(std::chrono::utc_clock::now())) {}

        confidentiality_label(const confidentiality_label&) = default;
        confidentiality_label& operator=(const confidentiality_label& rhs) = default;

        ///
        /// @brief Field to save having to implement the type inheritance here
        confidentiality_label_type label_type;

        ///
        /// @brief <s4774::ConfidentialityInformation>
        confidentiality_information_type confidentiality_information;

        ///
        /// @brief <s4774:OriginatorID>
        std::optional<originator_id> originator_id;

        ///
        /// @brief <s4774::CreationDateTime>
        std::chrono::time_point<std::chrono::utc_clock, std::chrono::seconds> creation_date_time;
    };

}
