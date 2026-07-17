/// @file sidecar.cpp
/// @brief sidecar.cpp brief
/// @details sidecar.cpp details
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

namespace nmbs::binding::sidecar
{

    AccessMode access_mode(const std::filesystem::path& path)
    {
        try
        {
            if (std::filesystem::is_directory(path))
            {
                return am_none;
            }

            std::filesystem::path bdo_path{path};
            bdo_path += ".bdo";
            if (std::filesystem::exists(bdo_path) && std::filesystem::is_regular_file(bdo_path)) [[unlikely]]
            {
                return access_mode_filesystem(bdo_path);
            }

            if (const std::filesystem::path dir = path.parent_path();
                std::filesystem::is_directory(dir))
            {
                return access_mode_filesystem(dir);
            }

            return am_none;
        }
        catch (const std::exception&)
        {
            // Can happen in some cases, like trying to check a /root/... file
            return am_none;
        }
    }


    Expected<BindingInformation> read(const std::filesystem::path& path)
    {
        return read_xml(path).and_then(nmbs::serialisation::deserialise_binding_information);
    }


    Expected<std::string> read_xml(const std::filesystem::path& path)
    {
        std::filesystem::path bdo_path = path;
        bdo_path += ".bdo";
        if (!std::filesystem::exists(path) || !std::filesystem::exists(bdo_path)) [[unlikely]]
        {
            return std::unexpected(Error::file_not_found());
        }

        if (std::ifstream bdo_file(bdo_path); bdo_file.is_open())
        {
            std::stringstream buffer;
            buffer << bdo_file.rdbuf();
            return buffer.str();
        }
        return std::unexpected(Error::unexpected());
    }


    bool supported(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path) && std::filesystem::is_regular_file(path))
        {
            return false;
        }

        if (std::filesystem::is_directory(path))
        {
            return false;
        }

        if (const std::filesystem::path dir = path.parent_path();
            std::filesystem::is_directory(dir))
        {
            return true;
        }

        return false;
    }


    Expected<std::string> write(const std::filesystem::path& path, const std::vector<ConfidentialityLabel>& confidentiality_labels)
    {
        try
        {
            if (!std::filesystem::exists(path)) [[unlikely]]
            {
                return std::unexpected(Error::file_not_found());
            }

            std::filesystem::path bdo_path{path};
            bdo_path += ".bdo";
            const std::filesystem::path bdo_uri = "./" / bdo_path.filename();

            binding::BindingInformation bdo;
            bdo.reference.uri = bdo_uri.string();
            bdo.labels = confidentiality_labels;

            const std::string payload = nmbs::serialisation::serialise_binding_information(bdo);

            if (std::ofstream bdo_file(bdo_path); bdo_file.is_open()) {
                bdo_file << R"(<?xml version="1.0" encoding="UTF-8"?>)";
                bdo_file << payload;
            }

            return payload;
        }
        catch (const std::exception& e) {
            return std::unexpected(Error::unexpected("std::exception: " + std::string(e.what())));
        }
    }

}
