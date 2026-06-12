#pragma once

/// @file exception.h
/// @brief the base exception class for nmbs
/// Every exception we throw inherits from this class.
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

#include "exit_code.h"
#include <exception>
#include <string>
#include <utility>


namespace nmbs
{
    ///
    /// @brief generic base exception for the nmbs namespace
    class exception : public std::exception
    {
    public:
        /// generic base exception for the nmbs namespace
        /// @param code the nmbs::exit_code related to this error
        /// @param message a message which may be presented to the end user
        /// @see nmbs::exit_code
        exception(const nmbs::exit_code code, std::string message)
            : code_(code), message_(std::move(message))
        {
        }

        ///
        /// virtual destructor ensuring proper cleanup for derived classes
        ~exception() noexcept override = default;

        ///
        /// overriding standard what() to return our custom string message
        [[nodiscard]] const char* what() const noexcept override
        {
            return message_.c_str();
        }

        ///
        /// getter for the custom integer error code
        [[nodiscard]] nmbs::exit_code code() const noexcept
        {
            return code_;
        }

    private:
        nmbs::exit_code code_;
        std::string message_;
    };

}
