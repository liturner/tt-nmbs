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
#include <ranges>
#include <iostream>

namespace nmbs
{
    std::string version()
    {
        return project_version;
    }

    std::string serialise_confidentiality_label(const confidentiality_label& confidentiality_label)
    {
        const std::string creation_time_string = std::format("{:%FT%TZ}", confidentiality_label.creation_date_time);

        // Leaving this just flexible enough to make it easy to add loops for more complex labels later.
        std::string xml;
        xml.reserve(1024);
        xml.append(std::format(R"(<{0}:originatorConfidentialityLabel xmlns:{0}="{1}"><{0}:ConfidentialityInformation>)", constants::s4774_prefix, constants::s4774_namespace));
        xml.append(std::format("<{0}:PolicyIdentifier>{1}</{0}:PolicyIdentifier>", constants::s4774_prefix, confidentiality_label.confidentiality_information.policy_identifier));
        xml.append(std::format("<{0}:Classification>{1}</{0}:Classification></{0}:ConfidentialityInformation>", constants::s4774_prefix, confidentiality_label.confidentiality_information.classification));
        if (confidentiality_label.originator_id.has_value())
        {
            xml.append(std::format(R"(<{0}:OriginatorID IDType="{1}">{2}</{0}:OriginatorID>)", constants::s4774_prefix, confidentiality_label.originator_id.value().id_type_string(), confidentiality_label.originator_id.value().value));
        }
        xml.append(std::format("<{0}:CreationDateTime>{1}</{0}:CreationDateTime>", constants::s4774_prefix, creation_time_string));
        xml.append(std::format("</{0}:originatorConfidentialityLabel>", constants::s4774_prefix));


        return xml;
    }

    std::string binding_information(const std::string_view confidentiality_label)
    {
        return std::format(R"(<{0}:BindingInformation xmlns:{0}="{1}"><{0}:MetadataBindingContainer><{0}:MetadataBinding><{0}:Metadata>{2}</{0}:Metadata><{0}:DataReference URI="" /></{0}:MetadataBinding></{0}:MetadataBindingContainer></{0}:BindingInformation>)", constants::s4778_prefix, constants::s4778_namespace, confidentiality_label);
    }

    std::string write_labels(const std::filesystem::path& path, const std::vector<confidentiality_label>& confidentiality_labels)
    {
        const binding::flags binding_support = binding::support(path);
        if (!binding::supports_labels(binding_support))
        {
            // TODO: Throw here with valid exit code
            return "";
        }

        if (binding::supports_xmp(binding_support))
        {
            // TODO: Make the XMP function support multiple labels
            return write_xmp(path, confidentiality_labels[0]);
        }

        // TODO: Throw no supported label type

        return "";
    }

    std::string write_labels_xml(const std::filesystem::path& path, const std::string& confidentiality_labels)
    {
        return "";
    }

    [[nodiscard]] std::vector<confidentiality_label> read_labels(const std::filesystem::path& path)
    {
        std::vector<confidentiality_label> return_labels;
        const binding::flags binding_support = binding::support(path);
        if (!binding::supports_labels(binding_support))
        {
            return return_labels;
        }

        if (binding::has_xmp(binding_support))
        {
            const auto xmp_labels = read_xmp(path);
            return_labels.insert(return_labels.end(), xmp_labels.begin(), xmp_labels.end());
        }

        return return_labels;
    }

    [[nodiscard]] std::optional<std::string> read_labels_xml(const std::filesystem::path& path)
    {
        const binding::flags binding_support = binding::support(path);
        if (!binding::supports_labels(binding_support))
        {
            return std::nullopt;;
        }

        if (binding::has_xmp(binding_support))
        {
            return read_xmp_xml(path);
        }

        return std::nullopt;
    }



    std::string write_xmp(const std::filesystem::path& path, const confidentiality_label& confidentiality_label)
    {
        try
        {
            std::string payload = binding_information(serialise_confidentiality_label(
                confidentiality_label));
            const auto image = Exiv2::ImageFactory::open(path.string());
            if (image.get() == nullptr)
            {
                throw exceptions::file_not_found_exception();
            }
            image->readMetadata();
            Exiv2::XmpData& xmp_data = image->xmpData();
            Exiv2::XmpProperties::registerNs(std::string(constants::s4778_xmp_namespace), std::string(constants::s4778_xmp_prefix));
            const Exiv2::XmpKey slab_key{std::string(constants::s4778_xmp_prefix), std::string(constants::s4778_key)};
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
            Exiv2::XmpProperties::registerNs(std::string(constants::s4778_xmp_namespace), std::string(constants::s4778_xmp_prefix));
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
            const Exiv2::XmpKey slab_key{std::string(constants::s4778_xmp_prefix), std::string(constants::s4778_key)};
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

    namespace binding
    {
        [[nodiscard]] flags support(const std::filesystem::path& path)
        {
            // Keep early out logic in mind. This function must be as fast as possible, and IO can
            // be a problem!

            if (!std::filesystem::exists(path)) [[unlikely]]
            {
                return flags::none;
            }

            if (!std::filesystem::is_regular_file(path))
            {
                return flags::none;
            }

            flags result_flags = flags::none;

            const std::filesystem::path dir = path.parent_path();
            std::filesystem::perms dir_perms = std::filesystem::status(dir).permissions();
            if ((dir_perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none || (dir_perms & std::filesystem::perms::group_write) != std::filesystem::perms::none)
            {
                // This may be removed later if there exists a file with read-only!
                result_flags |= flags::supports_sidecar;
            }

            // Note that the dir may be read-only, but contain a writeable .bdo!
            std::filesystem::path bdo_path{path};
            bdo_path += ".bdo";
            if (std::filesystem::exists(bdo_path) && std::filesystem::is_regular_file(bdo_path)) [[unlikely]]
            {
                result_flags |= flags::has_sidecar;

                const std::filesystem::perms bdo_perms = std::filesystem::status(bdo_path).permissions();
                if ((bdo_perms & std::filesystem::perms::owner_write) == std::filesystem::perms::none && (bdo_perms & std::filesystem::perms::group_write) == std::filesystem::perms::none)
                {
                    result_flags |= flags::supports_sidecar;
                }
                else [[unlikely]]
                {
                    // Rare case. A read only sidecar exists.
                    result_flags &= ~flags::supports_sidecar;
                }
            }

            const Exiv2::ImageType type = Exiv2::ImageFactory::getType(path);
            try
            {
                const Exiv2::AccessMode mode = Exiv2::ImageFactory::checkMode(type, Exiv2::mdXmp);
                if (mode == Exiv2::AccessMode::amWrite)
                {
                    result_flags |= flags::supports_xmp;
                }
                else if (mode == Exiv2::AccessMode::amReadWrite || mode == Exiv2::AccessMode::amRead)
                {
                    result_flags |= flags::supports_xmp;
                    // Here it gets expensive. We now need to check the presence of the key
                    auto image = Exiv2::ImageFactory::open(path);
                    if (image) {
                        image->readMetadata();
                        Exiv2::XmpData& xmp_data = image->xmpData();
                        if (!xmp_data.empty()) {
                            const Exiv2::XmpKey slab_key{std::string(constants::s4778_xmp_prefix), std::string(constants::s4778_key)};
                            const auto xmp_iter = xmp_data.findKey(slab_key);
                            if (xmp_iter != xmp_data.end())
                            {
                                result_flags |= flags::has_xmp;
                            }
                        }
                    }
                }
            }
            catch (...)
            {
                // This is fine, Exiv just uses exceptions for certain logic flows
                result_flags &= ~flags::supports_xmp;
                result_flags &= ~flags::has_xmp;
            }

            return result_flags;
        }
    }

}
