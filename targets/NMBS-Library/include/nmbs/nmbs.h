#pragma once

/// @file nmbs.h
/// @brief The intended "include all" header file for libnmbs
/// @details This file will include all the available nmbs namespace. Use it as
/// single import to work with the nmbs namespace.
///
/// @author Luke Ian Turner
/// @date 2026-06-06
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

#include "confidentiality_label.h"
#include "exceptions.h"
#include <string>
#include <filesystem>

/// Lightweight helpers for generating NATO confidentiality metadata.
///
/// This namespace contains a compact set of utilities implementing the essential parts of the ADatP‑4774, ADatP‑4778,
/// and STANAG 5636 specifications. The functions focus on producing valid confidentiality labels and binding
/// information for files that are known to be *not* highly sensitive, enabling consistent tagging without the overhead
/// of a full‑scale DLP or metadata processing framework.
///
/// The goal of this library is to provide a minimal, dependency‑free toolkit that makes it easy to attach correct,
/// standards‑compliant metadata to low‑sensitivity artifacts—an important step in reducing the scope of data requiring
/// strict handling in cybersecurity workflows.
namespace nmbs
{
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
    inline constexpr std::string_view s4778_key = "bindingInformation";

    /// @brief Returns the semantic version of the library.
    ///
    /// Provides the version identifier for this binary build using standard
    /// semantic‑versioning format (`MAJOR.MINOR.PATCH`), for example `1.0.0`.
    ///
    /// @return The semantic version string for the current build.
    [[nodiscard]] std::string version();

    /// @brief Writes ADatP‑4778 binding information into an image's XMP metadata.
    ///
    /// Embeds the provided payload into the XMP metadata of the specified image
    /// using the standardized ADatP‑4778 XMP property key. The payload must be a
    /// valid `BindingInformation` element as produced by
    /// `nmbs::binding_information()`. Existing XMP metadata in the file is preserved
    /// unless overwritten by this operation.
    ///
    /// @param path     Path to the image file to modify.
    /// @param confidentiality_label  A standards‑compliant ADatP‑4778 BindingInformation element.
    /// @return The label written to the file.
    /// @throws nmbs::file_not_found_exception
    std::string write_xmp(const std::filesystem::path& path, const confidentiality_label& confidentiality_label);

    [[nodiscard]] std::vector<confidentiality_label> read_xmp(const std::filesystem::path& path);

    [[nodiscard]] std::optional<std::string> read_xmp_xml(const std::filesystem::path& path);

}
