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

#include "binding.h"
#include "confidentiality_label.h"
#include "expected.h"

/// @brief Lightweight helpers for generating NATO confidentiality metadata.
/// @details This namespace contains a compact set of utilities implementing the essential parts of the ADatP‑4774, ADatP‑4778,
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
    /// @details Using standard semantic‑versioning format (`MAJOR.MINOR.PATCH`), for example `1.0.0`. This value is
    /// embedded during the build, and will match the CMake version of the source code. This function will return a
    /// complete version, and never any suffix like "~beta". This is important to consider if you are working with beta
    /// and pre-release builds, as during beta phases any new API changes will be unstable and may change without the
    /// version changing.
    /// @return The semantic version string for the current build.
    /// @since 1.0.0
    [[nodiscard]] std::string_view version() noexcept;

    /// @brief Free up any allocated memory and state to keep tools like Valgrind clean.
    /// @details This should be safe to call in the middle of an application, it will just delete caches and unregister
    /// namespaces etc. This may lead to increased function times in subsequent calls.
    /// @since 1.0.0
    void cleanup();

    /// @brief Writes ADatP‑4778 binding information using the best possible binding profile.
    /// @details Embedded is preferred over Sidecar, and the presence of a Sidecar is ignored if embedding is possible.
    /// Furthermore, existing data will be overridden. You can attempt to force a specific profile by passing the flag
    /// in to the binding_support parameter. Check the return parameter for errors to see if it succeeded.
    /// @param path to the image file to label.
    /// @param confidentiality_labels collection of labels to write to the file.
    /// @param binding_support flags if already known. The presence of this parameter will save CPU cycles in
    /// determining the available binding method for the file. Use manually, or via nmbs::binding::support.
    /// @return The labels written to the file in XML form. If the nmbs::Expected does not have a value, then there was an error.
    /// @since 1.0.0
    [[nodiscard]] Expected<std::string> write_labels(
        const std::filesystem::path& path,
        const std::vector<ConfidentialityLabel>& confidentiality_labels,
        std::optional<binding::ProfileSupport> binding_support = std::nullopt);

    /// @brief Reads the ADatP-4774 labels from the file
    /// @details and returns them in a deserialised form. The deserialization is quite tolerant, and will favour
    /// returning incomplete data, rather than erroring out. This design choice is to ensure interoperability with other
    /// less strict implementations.
    /// @param path to the file
    /// @param binding_support flags if already known. The presence of this parameter will save CPU cycles in
    /// determining the available binding method for the file. Use manually, or via nmbs::binding::support.
    /// @return a collection of all labels applied to the file
    /// @see nmbs::write_labels
    /// @since 1.0.0
    [[nodiscard]] Expected<std::vector<ConfidentialityLabel>> read_labels(
        const std::filesystem::path& path,
        std::optional<binding::ProfileSupport> binding_support = std::nullopt);

    /// @brief Reads the full ADatP-4778 binding from the file.
    /// @details Returns the full XML packet in its raw XML form.
    /// @param path to the file
    /// @param binding_support flags if already known. The presence of this parameter will save CPU cycles in
    /// determining the available binding method for the file. Use manually, or via nmbs::binding::support.
    /// @return the raw XML of the stored binding.
    /// @since 1.0.0
    [[nodiscard]] Expected<std::string> read_binding_xml(
        const std::filesystem::path& path,
        std::optional<binding::ProfileSupport> binding_support = std::nullopt);

}
