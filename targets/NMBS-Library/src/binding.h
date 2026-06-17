/// @file binding.h
/// @brief ADatP-4778 oriented namespace, focusing on binding the ADatP-4774 labels to files.
///
/// @author Luke Ian Turner
/// @date 2026-06-24
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

#include <filesystem>

/// @brief ADatP-4778 oriented namespace, focusing on binding the ADatP-4774 labels to files.
///
namespace nmbs::binding
{
    ///
    /// @brief Flags detailing various binding support for specific files.
    enum class flags : std::uint32_t
    {
        ///
        /// The file does not support ADatP-4774 labels (at least in this implementation...)
        none = 0,

        ///
        /// The file supports the XMP binding profile, and is writeable
        supports_xmp = 1 << 0,

        ///
        /// The file has an XMP binding, and its readable
        has_xmp = 1 << 1,

        ///
        /// The file supports the Sidecar binding profile, and is writeable
        supports_sidecar = 1 << 2,

        ///
        /// The file has a Sidecar binding, and its readable
        has_sidecar = 1 << 3,

        ///
        /// The file supports an embedded binding, and no writeable sidecar file is present
        recommend_xmp = 1 << 4,

        ///
        /// The file either does not support an embedded binding, or a writeable sidecar file is present
        recommend_sidecar = 1 << 5
    };

    constexpr flags operator|(flags a, flags b) noexcept
    {
        using T = std::underlying_type_t<flags>;
        return static_cast<flags>(static_cast<T>(a) | static_cast<T>(b));
    }

    constexpr flags operator&(flags a, flags b) noexcept
    {
        using T = std::underlying_type_t<flags>;
        return static_cast<flags>(static_cast<T>(a) & static_cast<T>(b));
    }

    constexpr flags& operator|=(flags& a, const flags b) noexcept
    {
        return a = a | b;
    }

    constexpr flags operator~(flags a) noexcept
    {
        using T = std::underlying_type_t<flags>;
        return static_cast<flags>(~static_cast<T>(a));
    }

    constexpr flags& operator&=(flags& a, flags b) noexcept
    {
        return a = a & b;
    }

    /// @brief Unary Predicate to check if the XMP binding profile is supported.
    /// @param flags provided from a call to nmbs::binding::support
    /// @return if the nmbs::binding::flags::supports_xmp flag is in the flags parameter
    [[nodiscard]] constexpr bool supports_xmp(const flags flags) noexcept
    {
        return (flags & flags::supports_xmp) != flags::none;
    }

    /// @brief Unary Predicate to check if the XMP binding is in use.
    /// @param flags provided from a call to nmbs::binding::support
    /// @return if the nmbs::binding::flags::has_xmp flag is in the flags parameter
    [[nodiscard]] constexpr bool has_xmp(const flags flags) noexcept
    {
        return (flags & flags::has_xmp) != flags::none;
    }

    /// @brief Unary Predicate to check if the Sidecar binding profile is supported.
    /// @param flags provided from a call to nmbs::binding::support
    /// @return if the nmbs::binding::flags::supports_sidecar flag is in the flags parameter
    [[nodiscard]] constexpr bool supports_sidecar(const flags flags) noexcept
    {
        return (flags & flags::supports_sidecar) != flags::none;
    }

    /// @brief Unary Predicate to check if the Sidecar binding is in use.
    /// @param flags provided from a call to nmbs::binding::support
    /// @return if the nmbs::binding::flags::has_sidecar flag is in the flags parameter
    [[nodiscard]] constexpr bool has_sidecar(const flags flags) noexcept
    {
        return (flags & flags::has_sidecar) != flags::none;
    }

    [[nodiscard]] constexpr bool supports_labels(const flags binding_flags) noexcept
    {
        return supports_xmp(binding_flags) || supports_sidecar(binding_flags);
    }

    [[nodiscard]] constexpr bool recommend_xmp(const flags flags) noexcept
    {
        return (flags & flags::recommend_xmp) != flags::none;
    }

    [[nodiscard]] constexpr bool recommend_sidecar(const flags flags) noexcept
    {
        return (flags & flags::recommend_sidecar) != flags::none;
    }

    /// @brief Full file analysis to check the binding capabilities of the path.
    /// This function is written for speed, but it does necessarily perform several IO operations.
    /// Care should be taken to cache the result if possible.
    /// @param path to analyse
    /// @return nmbs::binding::flags containing all information about the path parameter
    [[nodiscard]] flags support(const std::filesystem::path& path);
}
