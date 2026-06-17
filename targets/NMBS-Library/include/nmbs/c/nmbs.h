#pragma once

/// @file nmbs.h
/// @brief C wrappers for certain aspects of the libnmbs
/// These are primarily written to support in GLib like integrations, such as
/// the Nautilus extension system. It is not intended to cover all aspect of
/// the C++ API, rather it will be minimally expanded as needed. It is assumed
/// that the primary users of the libnmbs will be using C++.
///
/// @author Luke Ian Turner
/// @date 2026-06-22
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

#ifdef __cplusplus
# define NMBS_NOEXCEPT noexcept
#else
# define NMBS_NOEXCEPT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/// @brief C struct representation of nmbs::confidentiality_label
/// @see nmbs_free_confidentiality_label
typedef struct {
    const char* policy_identifier;
    const char* classification;
} nmbs_confidentiality_label;

/// @brief C struct for holding lists of nmbs::confidentiality_label
/// @see nmbs_free_confidentiality_labels
typedef struct {
    int size;
    nmbs_confidentiality_label* label;
} nmbs_confidentiality_labels;

/// @brief C Flags detailing various binding support for specific files.
/// @see nmbs::binding::flags
typedef enum nmbs_binding_flags : uint32_t {
    ///
    /// The file does not support ADatP-4774 labels (at least in this implementation...)
    nmbs_binding_none              = 0,

    ///
    /// The file supports the XMP binding profile, and is writeable
    nmbs_binding_supports_xmp      = 1 << 0,

    ///
    /// The file has an XMP binding, and its readable
    nmbs_binding_has_xmp           = 1 << 1,

    ///
    /// The file supports the Sidecar binding profile, and is writeable
    nmbs_binding_supports_sidecar  = 1 << 2,

    ///
    /// The file has a Sidecar binding, and its readable
    nmbs_binding_has_sidecar       = 1 << 3,

    ///
    /// The file supports an embedded binding, and no writeable sidecar file is present
    nmbs_binding_recommend_xmp     = 1 << 4,

    ///
    /// The file either does not support an embedded binding, or a writeable sidecar file is present
    nmbs_binding_recommend_sidecar = 1 << 5
} nmbs_binding_flags;

/// Reads ADatP-4774 labels from a file. WARNING! Allocates Memory!
/// @param file to read labels from
/// @return A "malloc"ed list of labels. The caller owns the memory.
/// @see ::nmbs_free_confidentiality_labels
[[nodiscard]] nmbs_confidentiality_labels nmbs_read_labels(const char* file) NMBS_NOEXCEPT;

/// Writes ADatP-4774 labels to a file, using the most appropriate method.
/// Any existing labels will be overwritten. If an "append" functionality
/// is desired, please call nmbs_read_labels first, and extend the list.
/// @param file to write labels to
/// @param labels to write to the file. The caller owns the memory
/// @return nmbs::exit_code
[[nodiscard]] int nmbs_write_labels(const char* file, const nmbs_confidentiality_labels* labels) NMBS_NOEXCEPT;

/// Provides flags indicating the binding methods supported or used on the path.
/// The method does its best to be fast, nonetheless the result should ideally
/// be cached, as it queries numerous IO APIs to ascertain what is supported.
/// @param file to investigate
/// @return ::nmbs_binding_flags
/// @see nmbs::binding::binding_support
[[nodiscard]] uint32_t nmbs_binding_support(const char* file) NMBS_NOEXCEPT;

/// @brief Cleans up any allocated memory.
/// Will call free on any sub structures if needed. Pointers will also be reset to nullptr
/// @param label to clear from memory
/// @see ::nmbs_free_confidentiality_labels
void nmbs_free_confidentiality_label(nmbs_confidentiality_label* label) NMBS_NOEXCEPT;

/// @brief Cleans up any allocated memory.
/// Will call free on any sub structures if needed. Pointers will also be reset to nullptr
/// @param labels to clear from memory
/// @see ::nmbs_free_confidentiality_label
void nmbs_free_confidentiality_labels(nmbs_confidentiality_labels* labels) NMBS_NOEXCEPT;

#ifdef __cplusplus
}
#endif