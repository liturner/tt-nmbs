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

#include <exiv2/exiv2.hpp>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "nmbs/version.h"
#include "nmbs/nmbs_private.h"

// Anonymous namespace guarantees internal linkage.
// This symbol will not leak out of this specific .cpp file or the .so.
namespace
{
    std::vector<nmbs::spif::SecurityPolicy> security_policies;

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
                                if (auto policy = nmbs::serialisation::deserialise_security_policy(buffer.str()); policy.has_value())
                                {
                                    security_policies.emplace_back(policy.value());
                                }
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
                            if (auto policy = nmbs::serialisation::deserialise_security_policy(buffer.str()); policy.has_value())
                            {
                                security_policies.emplace_back(policy.value());
                            }
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
                            if (auto policy = nmbs::serialisation::deserialise_security_policy(buffer.str()); policy.has_value())
                            {
                                security_policies.emplace_back(policy.value());
                            }                        }
                    }
                }
            }

            // Filter duplicate policies out, preferring newer versions.
            std::ranges::sort(security_policies,
                              [](const nmbs::spif::SecurityPolicy& a, const nmbs::spif::SecurityPolicy& b) {
                                  if (a.name != b.name) {
                                      return a.name < b.name; // Group by name
                                  }
                                  return a.version > b.version; // Highest version comes first
                              });
            auto last = std::ranges::unique(security_policies,
                                            [](const nmbs::spif::SecurityPolicy& a, const nmbs::spif::SecurityPolicy& b) {
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
    [[nodiscard]] std::string_view version() noexcept
    {
        return project_version;
    }

    void cleanup()
    {
        Exiv2::XmpProperties::unregisterNs();
        serialisation::cleanup();
    }

    Expected<std::string> write_labels(
        const std::filesystem::path& path,
        const std::vector<ConfidentialityLabel>& confidentiality_labels,
        std::optional<binding::ProfileSupport> binding_support)
    {
        if (!binding_support.has_value())
        {
            binding_support = binding::support(path);
        }
        if (!binding::supports_labels(binding_support.value()))
        {
            return std::unexpected(Error::no_binding_support());
        }

        if (binding::supports_xmp(binding_support.value()))
        {
            return binding::xmp::write(path, confidentiality_labels);
        }

        if (binding::supports_sidecar(binding_support.value()))
        {
            return binding::sidecar::write(path, confidentiality_labels);
        }
        return std::unexpected(Error::unexpected());
    }

    [[nodiscard]] Expected<std::vector<ConfidentialityLabel>> read_labels(
        const std::filesystem::path& path,
        std::optional<binding::ProfileSupport> binding_support)
    {
        std::vector<ConfidentialityLabel> return_labels;

        if (!binding_support.has_value())
        {
            binding_support = binding::support(path);
        }
        if (!binding::supports_labels(binding_support.value()))
        {
            return return_labels;
        }

        if (binding::supports_xmp(binding_support.value()))
        {
            if (const auto binding = binding::xmp::read(path); binding.has_value())
            {
                return_labels.insert(return_labels.end(), binding->labels.begin(), binding->labels.end());
            }
        }

        if (binding::supports_sidecar(binding_support.value()))
        {
            if (const auto binding = binding::sidecar::read(path); binding.has_value())
            {
                return_labels.insert(return_labels.end(), binding->labels.begin(), binding->labels.end());
            }
        }

        if (binding::supports_xml(binding_support.value()))
        {
            if (const auto binding = serialisation::deserialise_binding_information_from_file(path); binding.has_value() && binding.value().has_value())
            {
                return_labels.insert(return_labels.end(), binding.value().value().labels.begin(), binding.value().value().labels.end());
            }
        }

        return return_labels;
    }

    [[nodiscard]] Expected<std::string> read_binding_xml(
        const std::filesystem::path& path,
        std::optional<binding::ProfileSupport> binding_support)
    {
        if (!binding_support.has_value())
        {
            binding_support = binding::support(path);
        }
        if (!binding::supports_labels(binding_support.value()))
        {
            return std::unexpected(Error::unexpected());;
        }

        if (binding::supports_xmp(binding_support.value()))
        {
            return binding::xmp::read_xml(path);
        }

        return std::unexpected(Error::unexpected());;
    }


    namespace spif
    {

        const std::vector<SecurityPolicy>& get_security_policies()
        {
            ::initialise_security_policies();

            return ::security_policies;
        }

    }

}
