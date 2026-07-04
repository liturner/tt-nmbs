/// @file nmbs_c.h
/// @brief C API for certain aspects of the libnmbs
/// @details These are primarily written to support in GLib like integrations, such as
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

#pragma once

/// @defgroup c_api C Language API
/// @brief The C interface to libnmbs.
///
/// This API does not "wrap" the underlying C++ API, rather it aims to provide a
/// clean C Style API with a slightly reduced scope to the full C++ API. It has
/// been written to enable the Nautilus extension in GNOME, and will be extended
/// as needed.
///
/// *Principles*
///
/// * You create it, you delete it. Use the new and delete functions provided for
/// the structs. Fill and manipulate with the support functions. Any C Strings
/// returned will be owned by the data structure, not the caller (use strdup if
/// needed etc.)
/// * Naming Scheme = [NAMESPACE]_[STRUCT]_[FUNCTION]

/// @defgroup c_bindings Bindings
/// @ingroup c_api
/// @brief Functions for working with ADatP-4778 bindings.

/// @defgroup c_confidentiality_labels Confidentiality Labels
/// @ingroup c_api
/// @brief Functions for working with ADatP-4774 labels.

/// @defgroup c_spif Security Policies
/// @ingroup c_api
/// @brief Functions for working with SPIF files and Security Policies.

#ifdef __cplusplus
# define NMBS_NOEXCEPT noexcept
#else
# define NMBS_NOEXCEPT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/// @brief Basic flags type
/// @ingroup c_bindings
/// @see nmbs_binding_flags_read_support
/// @see nmbs_binding_flags_has_labels
typedef uint32_t nmbs_binding_flags;

/// @brief Opaque Pointer to a collection of confidentiality labels.
/// @details This is one of the core data objects you should work with.
/// Note, that the struct type is not implemented! It is used only to ensure type safety.
/// @ingroup c_confidentiality_labels
/// @see nmbs_confidentiality_labels_new
/// @see nmbs_confidentiality_labels_delete
typedef struct nmbs_confidentiality_labels* nmbs_confidentiality_labels_ptr;

/// @brief Opaque Pointer to a confidentiality label.
/// @details Note, that the struct type is not implemented! It is used only to ensure type safety.
/// @ingroup c_confidentiality_labels
/// @see nmbs_confidentiality_labels_get
/// @see nmbs::confidentiality_label
typedef struct nmbs_confidentiality_label* nmbs_confidentiality_label_ptr;

/// @brief Opaque Pointer to a collection of security classifications.
/// @ingroup c_spif
/// @see nmbs_security_policy_get_security_classifications
typedef struct nmbs_security_classifications* nmbs_security_classifications_ptr;

/// @brief Opaque Pointer to a security classification.
/// @details Note, that the struct type is not implemented! It is used only to ensure type safety.
/// @ingroup c_spif
/// @see nmbs_security_classifications_get
typedef struct nmbs_security_classification* nmbs_security_classification_ptr;

/// @brief Opaque Pointer to a collection of security policies.
/// @details Note, that the struct type is not implemented! It is used only to ensure type safety.
/// @ingroup c_spif
/// @see nmbs_security_policies_new
/// @see nmbs_security_policies_delete
/// @see nmbs_security_policies_read_installed
typedef struct nmbs_security_policies* nmbs_security_policies_ptr;

/// @brief Opaque Pointer to a security policy.
/// @details Note, that the struct type is not implemented! It is used only to ensure type safety.
/// @ingroup c_spif
/// @see nmbs_security_policies_get
typedef struct nmbs_security_policy* nmbs_security_policy_ptr;

/// Provides flags indicating the binding methods supported or used on the path.
/// The method does its best to be fast, nonetheless the result should ideally
/// be cached, as it queries numerous IO APIs to ascertain what is supported.
/// @ingroup c_bindings
/// @param file to investigate
/// @return ::nmbs_binding_flags
[[nodiscard]] uint32_t nmbs_binding_flags_read_support(const char* file) NMBS_NOEXCEPT;

/// @brief
/// @ingroup c_bindings
/// @param flags
/// @return
[[nodiscard]] bool nmbs_binding_flags_has_labels(uint32_t flags) NMBS_NOEXCEPT;

/// @brief
/// @ingroup c_bindings
/// @param flags
/// @return
[[nodiscard]] bool nmbs_binding_flags_supports_labels(uint32_t flags) NMBS_NOEXCEPT;

/// @brief
/// @ingroup c_api
void nmbs_cleanup() NMBS_NOEXCEPT;

/// @brief Creates and returns an instance of nmbs_confidentiality_labels.
/// @ingroup c_confidentiality_labels
/// @return Pointer to a "new" nmbs_confidentiality_labels
/// @see nmbs_confidentiality_labels_delete
[[nodiscard]] nmbs_confidentiality_labels_ptr nmbs_confidentiality_labels_new() NMBS_NOEXCEPT;

/// @brief "delete"s an instance of nmbs_confidentiality_labels.
/// @ingroup c_confidentiality_labels
/// @see nmbs_confidentiality_labels_new
void nmbs_confidentiality_labels_delete(nmbs_confidentiality_labels_ptr labels) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_confidentiality_labels
/// @param labels
/// @return Pointer to the underlying data. Do not free or delete this!
[[nodiscard]] nmbs_confidentiality_label_ptr nmbs_confidentiality_labels_emplace_back(nmbs_confidentiality_labels_ptr labels) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_confidentiality_labels
/// @param labels
/// @param i
/// @return Pointer to the underlying data. Do not free or delete this!
[[nodiscard]] nmbs_confidentiality_label_ptr nmbs_confidentiality_labels_get(nmbs_confidentiality_labels_ptr labels, unsigned long i) NMBS_NOEXCEPT;

/// @brief Reads the nmbs_confidentiality_labels from a file, regardless of binding profile.
/// @ingroup c_confidentiality_labels
/// @param labels_out A pointer to your buffer which we will fill.
/// @param file Path to the file to read labels from.
/// @see nmbs_confidentiality_labels_new
void nmbs_confidentiality_labels_read_labels(nmbs_confidentiality_labels_ptr labels_out, const char* file) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_confidentiality_labels
/// @param labels
/// @return
[[nodiscard]] unsigned long nmbs_confidentiality_labels_size(nmbs_confidentiality_labels_ptr labels) NMBS_NOEXCEPT;

/// @brief Writes ADatP-4774 labels to a file, using the most appropriate method.
/// @details Any existing labels will be overwritten. If an "append" functionality
/// is desired, please call nmbs_read_labels first, and extend the list.
/// @ingroup c_confidentiality_labels
/// @param file to write labels to
/// @param labels to write to the file. The caller owns the memory
/// @return nmbs::exit_code
[[nodiscard]] int nmbs_confidentiality_labels_write_labels(const char* file, nmbs_confidentiality_labels_ptr labels) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_confidentiality_labels
/// @param label
/// @return Pointer to the underlying data. Do not free or delete this!
[[nodiscard]] const char* nmbs_confidentiality_label_get_classification(nmbs_confidentiality_label_ptr label) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_confidentiality_labels
/// @param label
/// @return Pointer to the underlying data. Do not free or delete this!
[[nodiscard]] const char* nmbs_confidentiality_label_get_policy(nmbs_confidentiality_label_ptr label) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_confidentiality_labels
/// @param label
/// @param string
void nmbs_confidentiality_label_set_policy(nmbs_confidentiality_label_ptr label, const char* string) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_confidentiality_labels
/// @param label
/// @param string
void nmbs_confidentiality_label_set_classification(nmbs_confidentiality_label_ptr label, const char* string) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_spif
/// @param classifications
/// @return
[[nodiscard]] unsigned long nmbs_security_classifications_size(nmbs_security_classifications_ptr classifications) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_spif
/// @param classifications
/// @param i
/// @return Pointer to the underlying data. Do not free or delete this!
[[nodiscard]] nmbs_security_classification_ptr nmbs_security_classifications_get(nmbs_security_classifications_ptr classifications, unsigned long i) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_spif
/// @param classification
/// @return Pointer to the underlying data. Do not free or delete this!
[[nodiscard]] const char* nmbs_security_classification_get_name(nmbs_security_classification_ptr classification) NMBS_NOEXCEPT;

/// @brief Creates and returns an instance of nmbs_security_policies.
/// @ingroup c_spif
/// @return Pointer to a "new" nmbs_security_policies
[[nodiscard]] nmbs_security_policies_ptr nmbs_security_policies_new() NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_spif
/// @param policies
/// @return
[[nodiscard]] unsigned long nmbs_security_policies_size(nmbs_security_policies_ptr policies) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_spif
/// @param policies
/// @param i
/// @return Pointer to the underlying data. Do not free or delete this!
[[nodiscard]] nmbs_security_policy_ptr nmbs_security_policies_get(nmbs_security_policies_ptr policies, unsigned long i) NMBS_NOEXCEPT;

/// @brief "delete"s an instance of nmbs_security_policies_ptr.
/// @ingroup c_spif
/// @param policies
void nmbs_security_policies_delete(nmbs_security_policies_ptr policies) NMBS_NOEXCEPT;

/// @brief Reads the nmbs_security_policies which are installed on the system.
/// @details The C++ API contains a few constants detailing where the lib will look
/// for installed SPIF files.
/// @ingroup c_spif
/// @see nmbs::spif::packaged_spif_location
/// @see nmbs::spif::administered_spif_location
/// @see nmbs::spif::override_spif_location_env_var
void nmbs_security_policies_read_installed(nmbs_security_policies_ptr policies_out) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_spif
/// @param policy
/// @return Pointer to the underlying data. Do not free or delete this!
[[nodiscard]] const char* nmbs_security_policy_get_name(nmbs_security_policy_ptr policy) NMBS_NOEXCEPT;

/// @brief Opaque Accessor
/// @ingroup c_spif
/// @param policy
/// @return Pointer to the underlying data. Do not free or delete this!
[[nodiscard]] nmbs_security_classifications_ptr nmbs_security_policy_get_security_classifications(nmbs_security_policy_ptr policy) NMBS_NOEXCEPT;

#ifdef __cplusplus
}
#endif
