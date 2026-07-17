/// @file nmbs_private.h
/// @brief Private nmbs declarations, not intended to be exposed to the public API,
/// but possibly will be in the future. Can be seen as a maturation space.
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

#include <string>

#include "nmbs/nmbs.h"
#include "nmbs/expected.h"
#include "nmbs/spif.h"

namespace nmbs
{
    namespace binding
    {
        struct DataReference
        {
            std::string uri;
            std::optional<std::string> content_type;
        };

        struct BindingInformation
        {
            std::vector<ConfidentialityLabel> labels;
            DataReference reference;
        };

        /// @brief XML namespace defined by ADatP‑4774 Ed. A, Ver. 1.
        ///
        /// This constant identifies the official URN for the Confidentiality Metadata
        /// Label schema as specified in ADatP‑4774. It is used when generating
        /// standards‑compliant confidentiality labels and related metadata structures.
        /// @see nmbs::s4774_prefix
        inline constexpr std::string_view s4774_namespace = "urn:nato:stanag:4774:confidentialitymetadatalabel:1:0";

        /// @brief Preferred XML prefix for the ADatP‑4774 namespace.
        ///
        /// This prefix is the normative identifier defined in ADatP‑5636 Ed. A, Ver. 1
        /// for the ADatP‑4774 Confidentiality Metadata Label schema. Earlier materials,
        /// including examples and XSD fragments in ADatP‑4774 Ed. A, Ver. 1, used the
        /// alias "slab"; however, this is not the authoritative definition. Future
        /// revisions of ADatP‑4774 are expected to align with the ADatP‑5636 prefix.
        ///
        /// Although XML processors should rely on namespace URIs rather than prefixes
        /// for identification, many software systems still treat prefixes as
        /// significant. Using the standardized prefix improves interoperability with
        /// tools that do not fully respect namespace semantics.
        /// @see nmbs::s4774_namespace
        inline constexpr std::string_view s4774_prefix = "s4774";

        /// @brief XML namespace defined by ADatP‑4778 Ed. A, Ver. 1.
        ///
        /// This constant identifies the official URN for the Binding Information schema
        /// specified in ADatP‑4778. It is used when generating standards‑compliant
        /// binding information elements that accompany confidentiality labels and other
        /// metadata structures defined across the ADatP‑4774/4778/5636 family.
        /// @see nmbs::s4778_prefix
        inline constexpr std::string_view s4778_namespace = "urn:nato:stanag:4778:bindinginformation:1:0";

        /// @brief Preferred XML prefix for the ADatP‑4778 namespace.
        ///
        /// This prefix is the normative identifier associated with the Binding
        /// Information schema defined in ADatP‑4778 Ed. A, Ver. 1. While XML processors
        /// should rely on namespace URIs rather than prefixes for schema identification,
        /// many software systems still treat prefixes as significant. Using the
        /// standardized prefix improves interoperability with tools that do not fully
        /// respect namespace semantics.
        /// @see nmbs::s4778_namespace
        inline constexpr std::string_view s4778_prefix = "s4778";

        [[nodiscard]] constexpr bool supports_xmp(const ProfileSupport flags) noexcept
        {
            return flags & ps_xmp;
        }

        [[nodiscard]] constexpr bool supports_sidecar(const ProfileSupport flags) noexcept
        {
            return flags & ps_sidecar;
        }

        [[nodiscard]] constexpr bool supports_xml(const ProfileSupport flags) noexcept
        {
            return flags & ps_xml;
        }

        [[nodiscard]] constexpr bool supports_labels(const ProfileSupport binding_flags) noexcept
        {
            constexpr auto check = ps_xmp | ps_sidecar | ps_xml | ps_ext4;
            return binding_flags & check;
        }

        [[nodiscard]] AccessMode access_mode_filesystem(const std::filesystem::path& path);

        namespace xmp
        {
            inline constexpr std::string_view profile_canonical_identifier = "urn:nato:stanag:4778:profile:xmp";

            inline constexpr std::string_view profile_version_identifier = "urn:nato:stanag:4778:profile:xmp:1:1";

            /// @brief XMP namespace defined for ADatP‑4778 binding information.
            ///
            /// This constant identifies the URN used by the XMP representation of the
            /// ADatP‑4778 Binding Information schema, as specified in ADatP‑4778.2 Ed. A,
            /// Ver. 1. Although this namespace does not appear in any XML generated by this
            /// library, it is required when interacting with XMP metadata—particularly when
            /// locating or interpreting existing binding‑information tags embedded in image
            /// files or other media.
            ///
            /// @see nmbs::s4778_xmp_prefix
            inline constexpr std::string_view s4778_xmp_namespace = "urn:nato:stanag:4778:bindinginformation:1:0:xmp#";

            /// @brief Preferred XML prefix for the ADatP‑4778 XMP namespace.
            ///
            /// This prefix appears in example material associated with ADatP‑4778.2 Ed. A,
            /// Ver. 1, but it is not normatively defined in either ADatP‑4778 or
            /// ADatP‑5636. It is provided here solely for human readability and for
            /// constructing conventional XMP property names of the form:
            ///
            ///     Xmp.[nmbs::s4778_xmp_prefix].[nmbs::s4778_key]
            ///
            /// Software must not rely on this prefix for identification. When reading XMP
            /// metadata, programs must instead use the namespace URI
            /// (`nmbs::s4778_xmp_namespace`) to determine which prefix or prefixes are in
            /// use, as multiple aliases may legitimately appear in existing files.
            /// @see nmbs::s4778_xmp_namespace
            inline constexpr std::string_view s4778_xmp_prefix = "mbxmp";

            /// @brief Local name used for the ADatP‑4778 XMP property.
            ///
            /// This constant defines the key component of the XMP property associated with
            /// ADatP‑4778 binding information. The resulting XMP property name follows the
            /// conventional structure:
            ///
            ///     Xmp.[nmbs::s4778_xmp_prefix].[nmbs::s4778_key]
            ///
            /// The value of `s4778_xmp_prefix` is provided only for human readability and
            /// for constructing familiar XMP property strings. Software must not rely on
            /// this prefix for identification. When reading XMP metadata, programs must use
            /// the namespace URI (`nmbs::s4778_xmp_namespace`) to determine which prefix or
            /// prefixes are actually in use, as multiple aliases may legitimately appear in
            /// existing files.
            inline constexpr std::string_view s4778_xmp_key = "bindingInformation";

            [[nodiscard]] constexpr DataReference create_data_reference()
            {
                DataReference reference;
                reference.uri = "";
                return reference;
            }

            [[nodiscard]] AccessMode access_mode(const std::filesystem::path& path);

            [[nodiscard]] Expected<BindingInformation> read(const std::filesystem::path& path);

            [[nodiscard]] Expected<std::string> read_xml(const std::filesystem::path& path);

            [[nodiscard]] bool supported(const std::filesystem::path& path);

            [[nodiscard]] Expected<std::string> write(const std::filesystem::path& path, const std::vector<ConfidentialityLabel>& confidentiality_labels);

        }

        namespace sidecar
        {
            inline constexpr std::string_view profile_canonical_identifier = "urn:nato:stanag:4778:profile:sidecar";

            inline constexpr std::string_view profile_version_identifier = "urn:nato:stanag:4778:profile:sidecar:1:2";

            [[nodiscard]] AccessMode access_mode(const std::filesystem::path& path);

            [[nodiscard]] std::expected<BindingInformation, Error> read(const std::filesystem::path& path);

            [[nodiscard]] Expected<std::string> read_xml(const std::filesystem::path& path);

            [[nodiscard]] bool supported(const std::filesystem::path& path);

            [[nodiscard]] Expected<std::string> write(const std::filesystem::path& path, const std::vector<ConfidentialityLabel>& confidentiality_labels);

        }

        namespace http
        {

            inline constexpr std::string_view profile_canonical_identifier = "urn:nato:stanag:4778:profile:rest";

            inline constexpr std::string_view profile_version_identifier = "urn:nato:stanag:4778:profile:rest:1:2";

            inline constexpr std::string_view header_key = "Binding-Data";

            /// @brief Creates a data_reference with the standard content type for the
            /// REST binding profile.
            /// This factory function handles the creation and initial configuration of a
            /// data_reference structure, ensuring the default content type is properly assigned.
            /// @return A fully initialized binding::data_reference object.
            [[nodiscard]] constexpr DataReference create_data_reference()
            {
                DataReference reference;
                reference.content_type = "message/http";
                return reference;
            }

            [[nodiscard]] std::string serialise_header(const std::vector<ConfidentialityLabel>& confidentiality_labels);

            // Expects full header string!
            [[nodiscard]] Expected<BindingInformation> deserialise_header(std::string_view binding_data);

        }

        namespace ext4
        {

        }

        namespace xml
        {
            [[nodiscard]] AccessMode access_mode(const std::filesystem::path& path);

            [[nodiscard]] std::expected<BindingInformation, Error> read(const std::filesystem::path& path);

            [[nodiscard]] Expected<std::string> read_xml(const std::filesystem::path& path);

            [[nodiscard]] bool supported(const std::filesystem::path& path);

        }

    }

    namespace serialisation
    {
        [[nodiscard]] Expected<binding::BindingInformation> deserialise_binding_information(const std::string& xml);

        [[nodiscard]] Expected<std::optional<std::string>> read_binding_information_xml_from_file(const std::filesystem::path& file);

        [[nodiscard]] Expected<std::optional<binding::BindingInformation>> deserialise_binding_information_from_file(const std::filesystem::path& file);

        [[nodiscard]] Expected<spif::SecurityPolicy> deserialise_security_policy(const std::string& xml);

        [[nodiscard]] std::string encode_base64(const std::string& input);

        [[nodiscard]] std::string decode_base64(std::string_view input);

        [[nodiscard]] std::string serialise_binding_information(const binding::BindingInformation& binding_information);

        [[nodiscard]] std::string serialise_confidentiality_labels(const std::vector<ConfidentialityLabel>& confidentiality_labels);

        [[nodiscard]] std::string serialise_confidentiality_label(const ConfidentialityLabel& confidentiality_label);

        void cleanup();

    }

}
