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
#include "nmbs/binding.h"
#include "nmbs/constants.h"
#include "nmbs/exceptions.h"
#include "nmbs/version.h"
#include "nmbs_private.h"

#include <exiv2/exiv2.hpp>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>

// Anonymous namespace guarantees internal linkage.
// This symbol will not leak out of this specific .cpp file or the .so.
namespace
{
    std::vector<nmbs::spif::security_policy> security_policies;

    bool security_policies_initialised = false;

    void initialise_security_policies() noexcept
    {
        if (security_policies_initialised) [[likely]]
        {
            return;
        }

        try
        {
            char* spif_override_cstr = std::getenv(std::string(nmbs::spif::override_spif_location_env_var).c_str());
            if (spif_override_cstr != nullptr)
            {
                const std::filesystem::path spif_override_dir = std::string(spif_override_cstr);

                if (std::filesystem::exists(spif_override_dir) && std::filesystem::is_directory(spif_override_dir))
                {
                    for (const auto& entry : std::filesystem::directory_iterator(spif_override_dir))
                    {
                        if (entry.is_regular_file())
                        {
                            std::ifstream file(entry.path());
                            if (file)
                            {
                                std::stringstream buffer;
                                buffer << file.rdbuf();
                                security_policies.emplace_back(nmbs::xml::deserialise_security_policy(buffer.str()));
                            }
                        }
                    }
                }

                security_policies_initialised = true;
                return;
            }

            security_policies.clear();
            if (std::filesystem::exists(nmbs::spif::packaged_spif_location) && std::filesystem::is_directory(
                nmbs::spif::packaged_spif_location))
            {
                for (const auto& entry : std::filesystem::directory_iterator(nmbs::spif::packaged_spif_location))
                {
                    if (entry.is_regular_file())
                    {
                        std::ifstream file(entry.path());
                        if (file)
                        {
                            std::stringstream buffer;
                            buffer << file.rdbuf();
                            security_policies.emplace_back(nmbs::xml::deserialise_security_policy(buffer.str()));
                        }
                    }
                }
            }
            if (std::filesystem::exists(nmbs::spif::administered_spif_location) && std::filesystem::is_directory(
                nmbs::spif::administered_spif_location))
            {
                for (const auto& entry : std::filesystem::directory_iterator(nmbs::spif::administered_spif_location))
                {
                    if (entry.is_regular_file())
                    {
                        std::ifstream file(entry.path());
                        if (file)
                        {
                            std::stringstream buffer;
                            buffer << file.rdbuf();
                            security_policies.emplace_back(nmbs::xml::deserialise_security_policy(buffer.str()));
                        }
                    }
                }
            }

            // Filter duplicate policies out, preferring newer versions.
            std::ranges::sort(security_policies,
                              [](const nmbs::spif::security_policy& a, const nmbs::spif::security_policy& b) {
                                  if (a.name != b.name) {
                                      return a.name < b.name; // Group by name
                                  }
                                  return a.version > b.version; // Highest version comes first
                              });
            auto last = std::ranges::unique(security_policies,
                                            [](const nmbs::spif::security_policy& a, const nmbs::spif::security_policy& b) {
                                                return a.name == b.name;
                                            }).begin();
            security_policies.erase(last, security_policies.end());
        }
        catch (...)
        {
            std::cerr << "libnmbs: Exception while reading SPIF files." << std::endl;
        }

        security_policies_initialised = true;
    }
}

namespace nmbs
{
    std::string version()
    {
        return project_version;
    }

    void cleanup_state()
    {
        Exiv2::XmpProperties::unregisterNs();
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
            return write_xmp(path, confidentiality_labels);
        }

        if (binding::supports_sidecar(binding_support))
        {
            return write_sidecar(path, confidentiality_labels);
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
            const auto labels = read_xmp(path);
            return_labels.insert(return_labels.end(), labels.begin(), labels.end());
        }

        if (binding::has_sidecar(binding_support))
        {
            const auto labels = read_sidecar(path);
            return_labels.insert(return_labels.end(), labels.begin(), labels.end());
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



    std::string write_xmp(const std::filesystem::path& path, const std::vector<confidentiality_label>& confidentiality_labels)
    {
        try
        {
            binding::binding_information bdo;
            bdo.labels = confidentiality_labels;
            std::string payload = xml::serialise_binding_information(bdo);
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

    std::string write_sidecar(const std::filesystem::path& path, const std::vector<confidentiality_label>& confidentiality_labels)
    {
        try
        {
            if (!std::filesystem::exists(path)) [[unlikely]]
            {
                throw exceptions::file_not_found_exception();
            }

            std::filesystem::path bdo_path{path};
            bdo_path += ".bdo";
            const std::filesystem::path bdo_uri = "./" / bdo_path.filename();

            binding::binding_information bdo;
            bdo.reference.uri = bdo_uri.string();
            bdo.labels = confidentiality_labels;

            const std::string payload = xml::serialise_binding_information(bdo);

            if (std::ofstream bdo_file(bdo_path); bdo_file.is_open()) {
                bdo_file << R"(<?xml version="1.0" encoding="UTF-8"?>)";
                bdo_file << payload;
            }

            return payload;
        }
        catch (const std::exception& e) {
            throw nmbs::exception(exit_code::unknown_error, "std::exception: " + std::string(e.what()));
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

    std::vector<confidentiality_label> read_xmp(const std::filesystem::path& path)
    {
        if (const auto label_xml{read_xmp_xml(path)}; label_xml.has_value())
        {
            return xml::deserialise_binding_information(label_xml.value()).labels;
        }
        return {};
    }

    [[nodiscard]] std::optional<std::string> read_sidecar_xml(const std::filesystem::path& path)
    {
        try
        {
            std::filesystem::path bdo_path = path;
            bdo_path += ".bdo";
            if (!std::filesystem::exists(path) || !std::filesystem::exists(bdo_path)) [[unlikely]]
            {
                throw exceptions::file_not_found_exception();
            }

            if (std::ifstream bdo_file(bdo_path); bdo_file.is_open())
            {
                std::stringstream buffer;
                buffer << bdo_file.rdbuf();
                return buffer.str();
            }
            return std::nullopt;
        }
        catch (const Exiv2::Error& e)
        {
            throw nmbs::exception(exit_code::unknown_error, "Exiv2::Error: " + std::string(e.what()));
        }
    }

    [[nodiscard]] std::vector<confidentiality_label> read_sidecar(const std::filesystem::path& path)
    {
        if (const auto label_xml{read_sidecar_xml(path)}; label_xml.has_value())
        {
            return xml::deserialise_binding_information(label_xml.value()).labels;
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
                if ((bdo_perms & std::filesystem::perms::owner_write) == std::filesystem::perms::none && (bdo_perms & std::filesystem::perms::group_write) == std::filesystem::perms::none) [[unlikely]]
                {
                    // Rare case. A read only sidecar exists.
                    result_flags &= ~flags::supports_sidecar;
                }
                else
                {
                    result_flags |= flags::supports_sidecar;
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

        namespace http
        {
            [[nodiscard]] std::string serialise_labels(const std::vector<confidentiality_label>& confidentiality_labels)
            {
                binding_information bdo;
                bdo.labels = confidentiality_labels;
                const std::string binding_xml = xml::serialise_binding_information(bdo);
                const std::string base64_xml = xml::encode_base64(binding_xml);
                return std::format(
                    R"(binding-type="urn:nato:stanag:4778:bindinginformation:1:0 binding-data-object="{}")",
                    base64_xml);
            }

            [[nodiscard]] std::vector<confidentiality_label> deserialise_labels(std::string_view binding_data)
            {
                std::vector<confidentiality_label> labels;
                if (binding_data.empty())
                {
                    return labels;
                }

                // 1. Define what we are looking for
                constexpr std::string_view key = R"(binding-data-object=")";

                // 2. Find the start of the key
                size_t start_pos = binding_data.find(key);
                if (start_pos == std::string_view::npos) {
                    return labels; // Key not found!
                }

                // 3. Move the start pointer to the actual Base64 payload
                start_pos += key.length();

                // 4. Find the closing quote
                size_t end_pos = binding_data.find('"', start_pos);
                if (end_pos == std::string_view::npos) {
                    return labels; // Malformed string, missing closing quote!
                }

                std::string_view base64_xml = binding_data.substr(start_pos, end_pos - start_pos);
                std::string xml = xml::decode_base64(base64_xml);
                return xml::deserialise_binding_information(xml).labels;
            }

        }

    }

    namespace spif
    {

        const std::vector<security_policy>& get_security_policies()
        {
            ::initialise_security_policies();

            return ::security_policies;
        }

    }

}
