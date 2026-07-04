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

#pragma once

#include <string>
#include <filesystem>

#include "confidentiality_label.h"
#include "expected.h"

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

    /// @brief Returns the semantic version of the library.
    /// @details the version identifier for this binary build using standard
    /// semantic‑versioning format (`MAJOR.MINOR.PATCH`), for example `1.0.0`.
    /// @return The semantic version string for the current build.
    [[nodiscard]] std::string_view version() noexcept;

    /// @brief Free up any allocated memory and state to keep tools like Valgrind clean.
    /// @details This should be safe to call in the middle of an application, it will just
    /// delete caches and unregister namespaces etc.
    void cleanup();

    /// @brief Writes ADatP‑4778 binding information using the best possible binding profile.
    /// Embedded is preferred over Sidecar, and the presence of a Sidecar is ignored if
    /// embedding is possible. Furthermore, existing data will be overridden.
    /// @param path to the image file to label.
    /// @param confidentiality_labels collection of labels to write to the file
    /// @return The labels written to the file in XML form.
    [[nodiscard]] expected<std::string> write_labels(const std::filesystem::path& path, const std::vector<confidentiality_label>& confidentiality_labels);

    /// @brief Writes raw XML packet to the file, using the best possible binding profile.
    /// Embedded is preferred over Sidecar, and the presence of a Sidecar is ignored if
    /// embedding is possible. Furthermore, existing data will be overridden. No checks
    /// for valid labels are performed.
    /// @param path to the image file to label.
    /// @param confidentiality_labels XML to write to the file
    /// @return The labels written to the file in XML form.
    [[nodiscard]] expected<std::string> write_labels_xml(const std::filesystem::path& path, const std::string& confidentiality_labels);

    /// @brief Reads the ADatP-4774 labels from the file
    /// @details and returns them in a deserialised form. The deserialization
    /// is quite tolerant, and will favour returning incomplete data, rather
    /// than erroring out. This design choice is to ensure interoperability
    /// with other less strict implementations.
    /// @param path to the file
    /// @return a collection of all labels applied to the file
    /// @see nmbs::write_labels
    [[nodiscard]] expected<std::vector<confidentiality_label>> read_labels(const std::filesystem::path& path);

    /// @brief Reads the ADatP-4774 labels from the file
    /// @details and returns them in their raw XML form
    /// @param path to the file
    /// @return the raw XML of the stored labels
    /// @see nmbs::write_labels_xml
    [[nodiscard]] expected<std::string> read_labels_xml(const std::filesystem::path& path);

}
