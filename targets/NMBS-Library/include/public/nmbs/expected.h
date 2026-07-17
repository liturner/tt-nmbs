/// @file expected.h
/// @brief expected.h brief
/// @details expected.h details
///
/// @author luke
/// @date 2026-07-04
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

#include <expected>
#include <string>
#include <utility>

#include "exit_code.h"

namespace nmbs
{

    /// @brief A simple error container to use with nmbs::expected for communicating domain issues to function callers.
    /// @details Intended to be small and lightweight. A nmbs::exit_code is provided to better enable logic to be built
    /// on top of specific errors. Static factory functions are provided to support in generating errors. e.g.
    ///
    /// - auto failure_error = nmbs::Error::file_not_found();
    ///
    /// The constructors are private, and the produced instances are immutable with only const accessors being provided.
    /// Copy constructors, including move support are provided.
    /// @since 1.0.0
    /// @see nmbs::error::message
    /// @see nmbs::error::code
    class Error
    {
        ExitCode code_;
        std::string message_;

    private:
        Error(const ExitCode code, std::string message) : code_(code), message_(std::move(message))
        {
        }

    public:
        Error() = delete;
        Error(Error&&) noexcept = default;
        Error& operator=(Error&&) noexcept = default;
        Error(const Error&) = default;
        Error& operator=(const Error&) = default;

        [[nodiscard]] ExitCode code() const { return code_; }
        [[nodiscard]] const std::string& message() const { return message_; }

        static Error unexpected(
            std::string message = "An unexpected error occurred")
        {
            return {ExitCode::unknown_error, std::move(message)};
        }

        static Error file_not_found(
            std::string message = "Requested resource could not be found")
        {
            return {ExitCode::file_not_found, std::move(message)};
        }

        static Error xmp_not_found(
            std::string message = "Requested resource did not contain any XMP data (at all)")
        {
            return {ExitCode::xmp_not_found, std::move(message)};
        }

        static Error xmp_key_not_found(
            std::string message = "Requested resource did not contain the desired XMP key")
        {
            return {ExitCode::xmp_key_not_found, std::move(message)};
        }

        static Error xml_could_not_parse(
            std::string message = "Requested XML could not be read")
        {
            return {ExitCode::xml_could_not_parse, std::move(message)};
        }

        static Error xml_could_not_create_xpath_context(
            std::string message = "Requested XML could not be read")
        {
            return {ExitCode::xml_could_not_create_xpath_context, std::move(message)};
        }

        static Error no_binding_support(
            std::string message = "Requested file does not support any supported binding mechanism")
        {
            return {ExitCode::no_binding_support, std::move(message)};
        }

    };

    /// @brief Convenience typedef to standardise usage of nmbs::error in all std::expected usages.
    /// @since 1.0.0
    template <typename T>
    using Expected = std::expected<T, Error>;
}
