/// @file exit_code.h
/// @brief Basic enum containing a list of integer exit codes which the application may use
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

namespace nmbs
{
    /// The possible exit codes which any of the nmbs tools may produce. These may be used in library functions, and as
    /// return values for executables.
    /// @since 1.0.0
    enum ExitCode {

        ///
        /// No errors occurred. The operation completed successfully.
        success = 0,

        ///
        /// One or more of the mandatory or provided arguments was invalid.
        invalid_arguments = 1,

        ///
        /// The specified file could not be found.
        file_not_found = 10,

        ///
        /// There was no XMP packet attached to a file.
        xmp_not_found = 11,

        ///
        /// Although the file contained XMP, the desired key was not found.
        xmp_key_not_found = 12,

        ///
        /// The XML parser was unable to read the provided XML.
        xml_could_not_parse = 13,

        ///
        /// The XML parser was unable to read create an XPath context.
        xml_could_not_create_xpath_context = 14,

        ///
        /// There was no label attached to the file.
        no_label_present = 15,

        ///
        /// There was no binding profile available for the selected file.
        no_binding_support = 16,

        ///
        /// An unspecified error occurred.
        unknown_error = 99
    };
}
