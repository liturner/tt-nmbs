/// @file nmbs.cpp
/// @brief Primary compilation unit for the libnmbs
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

#include "nmbs/nmbs.h"
#include "nmbs/version.h"
#include "nmbs_private.h"

#include <exiv2/exiv2.hpp>
#include <chrono>
#include <iostream>

namespace nmbs
{
    std::string version()
    {
        return project_version;
    }

    std::string serialise_confidentiality_label(const std::string_view policy_identifier, const std::string_view classification)
    {
        const auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::utc_clock::now());
        const std::string now_string = std::format("{:%FT%TZ}", now);

        // Leaving this just flexible enough to make it easy to add loops for more complex labels later.
        std::string xml;
        xml.reserve(700);
        xml.append(std::format(R"(<{0}:originatorConfidentialityLabel xmlns:{0}="{1}"><{0}:ConfidentialityInformation><{0}:PolicyIdentifier>{2}</{0}:PolicyIdentifier><{0}:Classification>{3}</{0}:Classification></{0}:ConfidentialityInformation><{0}:CreationDateTime>{4}</{0}:CreationDateTime></{0}:originatorConfidentialityLabel>)", s4774_prefix, s4774_namespace, policy_identifier, classification, now_string));

        return xml;
    }

    std::string binding_information(const std::string_view confidentiality_label)
    {
        return std::format(R"(<{0}:BindingInformation xmlns:{0}="{1}"><{0}:MetadataBindingContainer><{0}:MetadataBinding><{0}:Metadata>{2}</{0}:Metadata><{0}:DataReference URI="" /></{0}:MetadataBinding></{0}:MetadataBindingContainer></{0}:BindingInformation>)", s4778_prefix, s4778_namespace, confidentiality_label);
    }

    std::string write_xmp(const std::filesystem::path& path, const confidentiality_label& confidentiality_label)
    {
        try
        {
            std::string payload = binding_information(serialise_confidentiality_label(
                confidentiality_label.confidentiality_information.policy_identifier,
                confidentiality_label.confidentiality_information.classification));
            const auto image = Exiv2::ImageFactory::open(path.string());
            if (image.get() == nullptr)
            {
                throw exceptions::file_not_found_exception();
            }
            image->readMetadata();
            Exiv2::XmpData& xmp_data = image->xmpData();
            Exiv2::XmpProperties::registerNs(std::string(s4778_xmp_namespace), std::string(s4778_xmp_prefix));
            const Exiv2::XmpKey slab_key{std::string(s4778_xmp_prefix), std::string(s4778_key)};
            const Exiv2::XmpTextValue slab_value{std::string{payload}};
            xmp_data.add(slab_key, &slab_value);
            image->writeMetadata();
            return payload;
        }
        catch (const Exiv2::Error& e) {
            throw nmbs::exception(exit_code::unknown_error, "Exiv2::Error: " + std::string(e.what()));
        }
    }

    std::optional<std::string> read_xmp_xml(const std::filesystem::path& path)
    {
        try
        {
            Exiv2::XmpProperties::registerNs(std::string(s4778_xmp_namespace), std::string(s4778_xmp_prefix));
            const auto image = Exiv2::ImageFactory::open(path.string());
            if (image.get() == nullptr)
            {
                throw exceptions::file_not_found_exception();
            }
            image->readMetadata();

            // This function must assume XMP is here. It is an exception if not. The user may be trying to read
            // a file with no XMP support, so it is NOT a case for returning nullopt.
            Exiv2::XmpData& xmp_data = image->xmpData();
            if (xmp_data.empty())
            {
                throw exceptions::xmp_not_found_exception();
            }

            // Here we have XMP, but no label. Its valid to return nullopt
            const Exiv2::XmpKey slab_key{std::string(s4778_xmp_prefix), std::string(s4778_key)};
            const auto xmp_iter = xmp_data.findKey(slab_key);
            if (xmp_iter == xmp_data.end())
            {
                return std::nullopt;
            }

            // TODO: Improve here. Probably validate that its sane XML or at least not empty?
            const auto xmp_value = xmp_iter->getValue();
            return xmp_value->toString();

        }
        catch (const Exiv2::Error& e) {
            throw nmbs::exception(exit_code::unknown_error, "Exiv2::Error: " + std::string(e.what()));
        }
    }

    std::vector<nmbs::confidentiality_label> read_xmp(const std::filesystem::path& path)
    {
        if (const auto label_xml{read_xmp_xml(path)}; label_xml.has_value())
        {
            return xml::from_xml(label_xml.value());
        }
        return {};
    }
}
