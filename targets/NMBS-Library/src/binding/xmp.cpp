/// @file xmp.cpp
/// @brief xmp.cpp brief
/// @details xmp.cpp details
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

#include <exiv2/exiv2.hpp>

#include "nmbs/expected.h"


namespace nmbs::binding::xmp
{

    [[nodiscard]] AccessMode access_mode(const std::filesystem::path& path)
    {
        const Exiv2::ImageType type = Exiv2::ImageFactory::getType(path);
        try
        {
            const Exiv2::AccessMode mode = Exiv2::ImageFactory::checkMode(type, Exiv2::mdXmp);
            if (mode == Exiv2::AccessMode::amNone) return am_none;
            if (mode == Exiv2::AccessMode::amRead) return am_read;
            if (mode == Exiv2::AccessMode::amWrite) return am_write;
            if (mode == Exiv2::AccessMode::amReadWrite) return am_read | am_write;
            return am_none;
        }
        catch (Exiv2::Error& e)
        {
            return am_none;
        }
    }


    Expected<BindingInformation> read(const std::filesystem::path& path)
    {
        return read_xml(path).and_then(nmbs::serialisation::deserialise_binding_information);
    }


    Expected<std::string> read_xml(const std::filesystem::path& path)
    {
        try
        {
            Exiv2::XmpProperties::registerNs(std::string(s4778_xmp_namespace), std::string(s4778_xmp_prefix));
            const Exiv2::XmpKey s4778_key{std::string(s4778_xmp_prefix), std::string(s4778_xmp_key)};
            const auto image = Exiv2::ImageFactory::open(path.string());
            if (image.get() == nullptr)
            {
                return std::unexpected(Error::file_not_found());
            }
            image->readMetadata();

            // This function must assume XMP is here. It is an exception if not. The user may be trying to read
            // a file with no XMP support, so it is NOT a case for returning nullopt.
            Exiv2::XmpData& xmp_data = image->xmpData();
            if (xmp_data.empty())
            {
                return std::unexpected(Error::xmp_not_found());
            }

            // Here we have XMP, but no label. It's valid to return nullopt
            const auto xmp_iter = xmp_data.findKey(s4778_key);
            if (xmp_iter == xmp_data.end())
            {
                return std::unexpected(Error::xmp_key_not_found());
            }

            const auto xmp_value = xmp_iter->getValue();
            return xmp_value->toString();
        }
        catch (const Exiv2::Error& e) {
            return std::unexpected(Error::unexpected("Exiv2::Error: " + std::string(e.what())));
        }
    }


    [[nodiscard]] bool supported(const std::filesystem::path& path)
    {
        try
        {
            if(!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
            {
                return false;
            }
            const Exiv2::ImageType type = Exiv2::ImageFactory::getType(path);
            if (const Exiv2::AccessMode mode = Exiv2::ImageFactory::checkMode(type, Exiv2::mdXmp);
                mode == Exiv2::AccessMode::amNone)
            {
                return false;
            }
            return true;
        }
        catch (std::exception&)
        {
            return false;
        }
    }


    Expected<std::string> write(const std::filesystem::path& path, const std::vector<ConfidentialityLabel>& confidentiality_labels)
    {
        try
        {
            Exiv2::XmpProperties::registerNs(std::string(s4778_xmp_namespace), std::string(s4778_xmp_prefix));
            const Exiv2::XmpKey s4778_key{std::string(s4778_xmp_prefix), std::string(s4778_xmp_key)};
            binding::BindingInformation bdo;
            bdo.labels = confidentiality_labels;
            std::string payload = nmbs::serialisation::serialise_binding_information(bdo);
            const auto image = Exiv2::ImageFactory::open(path.string());
            if (image.get() == nullptr)
            {
                return std::unexpected(Error::file_not_found());
            }
            image->readMetadata();
            Exiv2::XmpData& xmp_data = image->xmpData();
            const Exiv2::XmpTextValue slab_value{std::string{payload}};
            xmp_data.add(s4778_key, &slab_value);
            image->writeMetadata();
            return payload;
        }
        catch (const Exiv2::Error& e) {
            return std::unexpected(Error::unexpected("Exiv2::Error: " + std::string(e.what())));
        }
    }
}


