#pragma once

/// @file exceptions.h
/// @brief nmbs specific exceptions
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

#include "exception.h"
#include <string>
#include <utility>

///
/// nmbs::exception specialisations.
namespace nmbs::exceptions
{
    ///
    /// @brief "File not found" exception including sensible defaults for the message.
    class file_not_found_exception : public exception
    {
    public:
        /// Constructor
        /// @param message message
        explicit file_not_found_exception(std::string message = "Requested resource could not be found")
            : exception(exit_code::file_not_found, std::move(message))
        {
        }
    };

    ///
    /// "Xmp not found" exception including sensible defaults for the message.
    class xmp_not_found_exception : public exception
    {
    public:
        explicit xmp_not_found_exception(
            std::string message = "Requested resource did not contain any XMP data (at all)")
            : exception(exit_code::xmp_not_found, std::move(message))
        {
        }
    };

    ///
    /// "Xmp not found" exception including sensible defaults for the message.
    class xmp_key_not_found_exception : public exception
    {
    public:
        explicit xmp_key_not_found_exception(
            std::string key,
            std::string message = "Requested resource did not contain the desired XMP key ")
            : exception(exit_code::xmp_key_not_found, std::move(message) + std::move(key))
        {
        }
    };

    ///
    /// XML Parser could not read an XML in.
    class xml_could_not_parse_exception : public exception
    {
    public:
        explicit xml_could_not_parse_exception(
            std::string message = "Requested XML could not be read")
            : exception(exit_code::xml_could_not_parse, std::move(message))
        {
        }
    };

    ///
    /// XML Parser could not read an XML in.
    class xml_could_not_create_xpath_context_exception : public exception
    {
    public:
        explicit xml_could_not_create_xpath_context_exception(
            std::string message = "Requested XML could not be read")
            : exception(exit_code::xml_could_not_create_xpath_context, std::move(message))
        {
        }
    };
}
