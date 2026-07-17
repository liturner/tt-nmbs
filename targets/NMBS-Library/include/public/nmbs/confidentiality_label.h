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

#pragma once

#include <chrono>
#include <string>

namespace nmbs
{
    /// @brief Struct representation of the ADatP-4774 "Confidentiality Label"
    /// @details More information can be found on this in section 2.4 of ADatP-4774.1 Ed. A V. 1. The
    /// standard specifies three types of label containing identical data. This struct
    /// represents all three originatorConfidentialityLabel, alternativeConfidentialityLabel and
    /// successorConfidentialityLabel. See nmbs::confidentiality_label::confidentiality_label_type
    /// and the nmbs::confidentiality_label::label_type field.
    ///
    /// Noteworthy is that the label couples "confidentiality information" with a timestamp and
    /// additional metadata such as the user and review dates.
    /// @since 1.0.0
    /// @see nmbs::confidentiality_information_type
    struct ConfidentialityLabel
    {
        /// @brief identifier for the specialisation of the label.
        /// @details There is no <s4774::ConfidentialityLabel> in the standard, but all specialisations
        /// share the exact same values. The specialisation is used in validation steps with logic for
        /// deciding which classification applies.
        /// @since 1.0.0
        enum ConfidentialityLabelType
        {
            /// @brief <s4774::originatorConfidentialityLabel>
            /// @since 1.0.0
            originator,

            /// @brief <s4774::alternativeConfidentialityLabel>
            /// @since 1.0.0
            alternative,

            /// @brief <s4774::successorConfidentialityLabel>
            /// @since 1.0.0
            successor
        };

        /// @brief A core element of the nmbs::confidentiality_label containing the classification (e.g. PUBLIC UNMARKED)
        /// @details This struct represents <s4774::ConfidentialityInformation>.
        /// @since 1.0.0
        struct ConfidentialityInformation
        {
            /// @brief <s4774::PolicyIdentifier>
            /// @since 1.0.0
            std::string policy_identifier;

            /// @brief <s4774::Classification>
            /// @since 1.0.0
            std::string classification;

            /// @brief <s4774::PrivacyMark>
            /// @since 1.0.0
            std::optional<std::string> privacy_mark;

            bool operator==(const ConfidentialityInformation&) const = default;
        };

        struct OriginatorId
        {
            static constexpr std::string_view rfc822_name = "rfc822Name";
            static constexpr std::string_view dns_name = "dNSName";
            static constexpr std::string_view directory_name = "directoryName";
            static constexpr std::string_view uniform_resource_identifier = "uniformResourceIdentifier";
            static constexpr std::string_view ip_address = "iPAddress";
            static constexpr std::string_view x400_address = "x400Address";
            static constexpr std::string_view jid = "jID";
            static constexpr std::string_view user_principal_name = "userPrincipalName";

            std::string id_type;

            std::string value;

            bool operator==(const OriginatorId&) const = default;
        };

        /// Overloaded constructor taking the label type as a parameter.
        /// The label type can be changed later.
        /// @param label_type the type of label this instance will represent
        explicit ConfidentialityLabel(const ConfidentialityLabelType label_type = originator) :
            label_type(label_type),
            creation_date_time(std::chrono::floor<std::chrono::seconds>(std::chrono::utc_clock::now())) {}

        ConfidentialityLabel(const ConfidentialityLabel&) = default;
        ConfidentialityLabel& operator=(const ConfidentialityLabel& rhs) = default;
        bool operator==(const ConfidentialityLabel&) const = default;

        ///
        /// @brief Field to save having to implement the type inheritance here
        ConfidentialityLabelType label_type;

        ///
        /// @brief <s4774::ConfidentialityInformation>
        ConfidentialityInformation confidentiality_information;

        ///
        /// @brief <s4774:OriginatorID>
        std::optional<OriginatorId> originator_id;

        ///
        /// @brief <s4774::CreationDateTime>
        std::chrono::time_point<std::chrono::utc_clock, std::chrono::seconds> creation_date_time;
    };

}
