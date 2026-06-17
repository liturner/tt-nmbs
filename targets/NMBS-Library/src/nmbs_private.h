/// @file nmbs_private.h
/// @brief Private nmbs declarations, not intended to be exposed to the public API.
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

#include "nmbs/nmbs.h"
#include "binding.h"
#include <string>

namespace nmbs
{
    /// @brief Writes ADatP‑4778 binding information into an image's XMP metadata.
    /// @brief Embeds the provided payload into the XMP metadata of the specified image
    /// using the standardized ADatP‑4778 XMP property key. This overload of the
    /// function is a convenience overload. In a future version a more complete
    /// function will be provided taking in a vector of labels to write.
    /// @param path     Path to the image file to modify.
    /// @param confidentiality_label  A standards‑compliant ADatP‑4778 BindingInformation element.
    /// @return The label written to the file.
    /// @throws nmbs::file_not_found_exception failed to locate the provided path
    std::string write_xmp(const std::filesystem::path& path, const confidentiality_label& confidentiality_label);

    /// @brief Reads the ADatP-4774 labels from the XMP of a file
    /// @details and returns them in a deserialised form. The deserialization
    /// is quite tolerant, and will favour returning incomplete data, rather
    /// than erroring out. This design choice is to ensure interoperability
    /// with other less strict implementations.
    /// @param path to the file
    /// @return a collection of all labels applied to the file
    /// @throws nmbs::exceptions::file_not_found_exception failed to locate the provided path
    /// @throws nmbs::exception for all unexpected errors
    /// @see nmbs::write_xmp
    /// @see nmbs::read_xmp_xml
    [[nodiscard]] std::vector<confidentiality_label> read_xmp(const std::filesystem::path& path);

    /// @brief Reads the ADatP-4774 labels from the XMP of a file
    /// @details and returns them in their raw XML form
    /// @param path to the file
    /// @return the raw XML of the stored labels
    /// @throws nmbs::exceptions::file_not_found_exception failed to locate the provided path
    /// @throws nmbs::exception for all unexpected errors
    /// @see nmbs::write_xmp
    /// @see nmbs::read_xmp
    [[nodiscard]] std::optional<std::string> read_xmp_xml(const std::filesystem::path& path);

    /// @brief Generates a basic ADatP‑4774 confidentiality label.
    ///
    /// Constructs a minimal confidentiality label using the provided policy
    /// identifier and classification value, producing a standards‑compliant
    /// ADatP‑4774 label suitable for embedding in metadata structures or
    /// BindingInformation elements.
    ///
    /// @param confidentiality_label  The label to serialise.
    /// @return An XML string representing a basic ADatP‑4774 confidentiality label.
    [[nodiscard]] std::string serialise_confidentiality_label(const confidentiality_label& confidentiality_label);

    /// @brief Wraps a confidentiality label in an ADatP‑4778 BindingInformation element.
    ///
    /// Generates a BindingInformation element containing the provided
    /// confidentiality label. This function assumes that the resulting element will
    /// be embedded within a larger data structure; therefore, no XML declaration is
    /// included, and the `DataReference` field is set to an empty string as
    /// permitted by the ADatP‑4778 schema.
    ///
    /// @param confidentiality_label The XML confidentiality label to embed within
    ///        the BindingInformation element.
    /// @return An XML string whose root element is a standards‑compliant
    ///         BindingInformation element.
    [[nodiscard]] std::string binding_information(std::string_view confidentiality_label);

    namespace xml
    {
        [[nodiscard]] std::vector<confidentiality_label> from_xml(const std::string& xml);
    }

}
