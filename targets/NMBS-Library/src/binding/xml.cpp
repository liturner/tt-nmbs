/// @file xml.cpp
/// @brief xml.cpp brief
/// @details xml.cpp details
///
/// @author luke
/// @date 2026-07-17
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

#include "nmbs/nmbs_private.h"

#include <fstream>


namespace nmbs::binding::xml
{

    AccessMode access_mode(const std::filesystem::path& path)
    {
        if (supported(path))
        {
            return am_read;
        }
        return am_none;
    }


    std::expected<BindingInformation, Error> read(const std::filesystem::path& path)
    {
        return read_xml(path).and_then(nmbs::serialisation::deserialise_binding_information);
    }


    Expected<std::string> read_xml(const std::filesystem::path& path)
    {
        if (auto xml_string = nmbs::serialisation::read_binding_information_xml_from_file(path);
            xml_string.has_value() &&
            xml_string.value().has_value())
        {
            return xml_string.value().value();
        }
        // TODO: Detailed error. Note that the binding functions expect the result, the deserialisation its optional
        return std::unexpected(Error::unexpected());
    }


    bool supported(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path) && std::filesystem::is_regular_file(path))
        {
            return false;
        }

        if (const auto extension = path.extension();
            extension == ".xml" ||
            extension == ".xsd" ||
            extension == ".svg" ||
            extension == ".spif" ||
            extension == ".b")
        {
            return true;
        }

        ///
        /// The following chunk checks for the xml header line in a fast manner;
        ///
        if (access_mode_filesystem(path) & am_read)
        {
            std::ifstream file(path, std::ios::binary);
            if (!file) {
                return false;
            }
            std::array<char, 8> buffer{};
            file.read(buffer.data(), buffer.size());
            if (const std::string_view header(buffer.data(), file.gcount());
                header.starts_with("<?xml") ||
                header.starts_with("\xEF\xBB\xBF<?xml")) {
                return true;
            }
        }

        return false;
    }

}
