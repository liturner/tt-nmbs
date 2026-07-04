/// @file nmbs_c.cpp
/// @brief Compilation unit to include anything C (currently libxml) related.
/// @details The intention of this file is to place in one location all the
/// C code (which requires particular care) into one location.
///
/// @author Luke Ian Turner
/// @date 2026-06-13
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

#include "nmbs/nmbs_c.h"

#include <iostream>
#include <sstream>
#include <string>

#include "nmbs/exit_code.h"
#include "nmbs_private.h"
#include "nmbs/binding.h"
#include "nmbs/spif.h"

namespace
{

    [[nodiscard]] std::vector<nmbs::spif::security_classification>* to_cpp_classifications(nmbs_security_classifications_ptr in) noexcept
    {
        return reinterpret_cast<std::vector<nmbs::spif::security_classification>*>(in);
    }

    [[nodiscard]] nmbs::spif::security_classification* to_cpp_classification(nmbs_security_classification_ptr in) noexcept
    {
        return reinterpret_cast<nmbs::spif::security_classification*>(in);
    }


    [[nodiscard]] std::vector<nmbs::confidentiality_label>* to_cpp_labels(nmbs_confidentiality_labels_ptr in) noexcept
    {
        return reinterpret_cast<std::vector<nmbs::confidentiality_label>*>(in);
    }

    [[nodiscard]] nmbs::confidentiality_label* to_cpp_label(nmbs_confidentiality_label_ptr in) noexcept
    {
        return reinterpret_cast<nmbs::confidentiality_label*>(in);
    }

    [[nodiscard]] std::vector<nmbs::spif::security_policy>* to_cpp_policies(nmbs_security_policies_ptr in) noexcept
    {
        return reinterpret_cast<std::vector<nmbs::spif::security_policy>*>(in);
    }

    [[nodiscard]] nmbs::spif::security_policy* to_cpp_policy(nmbs_security_policy_ptr in) noexcept
    {
        return reinterpret_cast<nmbs::spif::security_policy*>(in);
    }

    [[nodiscard]] nmbs::binding::flags to_cpp_binding_flags(uint32_t in) noexcept
    {
        return static_cast<nmbs::binding::flags>(in);
    }

    [[nodiscard]] nmbs_security_classifications_ptr to_c_classifications(std::vector<nmbs::spif::security_classification>* in) noexcept
    {
        return reinterpret_cast<nmbs_security_classifications_ptr>(in);
    }

    [[nodiscard]] nmbs_security_classification_ptr to_c_classification(nmbs::spif::security_classification* in) noexcept
    {
        return reinterpret_cast<nmbs_security_classification_ptr>(in);
    }

    [[nodiscard]] nmbs_confidentiality_labels_ptr to_c_labels(std::vector<nmbs::confidentiality_label>* in) noexcept
    {
        return reinterpret_cast<nmbs_confidentiality_labels_ptr>(in);
    }

    [[nodiscard]] nmbs_confidentiality_label_ptr to_c_label(nmbs::confidentiality_label* in) noexcept
    {
        return reinterpret_cast<nmbs_confidentiality_label_ptr>(in);
    }

    [[nodiscard]] nmbs_security_policies_ptr to_c_policies(std::vector<nmbs::spif::security_policy>* in) noexcept
    {
        return reinterpret_cast<nmbs_security_policies_ptr>(in);
    }

    [[nodiscard]] nmbs_security_policy_ptr to_c_policy(nmbs::spif::security_policy* in) noexcept
    {
        return reinterpret_cast<nmbs_security_policy_ptr>(in);
    }

}

void nmbs_confidentiality_labels_read_labels(nmbs_confidentiality_labels_ptr labels_out, const char* file) noexcept
{
    try
    {
        auto const cpp_labels = to_cpp_labels(labels_out);
        cpp_labels->clear();
        if (auto const labels = nmbs::read_labels(std::filesystem::path(std::string(file))); labels.has_value())
        {
            cpp_labels->insert(cpp_labels->begin(), labels.value().begin(), labels.value().end());
        }
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in nmbs_confidentiality_labels_read_labels" << std::endl;
    }
}

unsigned long nmbs_confidentiality_labels_size(nmbs_confidentiality_labels_ptr labels) noexcept
{
    const auto cpp_labels = to_cpp_labels(labels);
    if (!cpp_labels)
    {
        return 0;
    }
    return cpp_labels->size();
}

nmbs_confidentiality_label_ptr nmbs_confidentiality_labels_emplace_back(nmbs_confidentiality_labels_ptr labels) noexcept
{
    try
    {
        auto labels_vector = to_cpp_labels(labels);
        if (!labels_vector)
        {
            return nullptr;
        }
        return to_c_label(&labels_vector->emplace_back());
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in nmbs_confidentiality_labels_emplace_back" << std::endl;
        return nullptr;
    }
}

nmbs_confidentiality_label_ptr nmbs_confidentiality_labels_get(nmbs_confidentiality_labels_ptr labels, const unsigned long i) noexcept
{
    try
    {
        auto labels_vector = to_cpp_labels(labels);
        if (!labels_vector || labels_vector->size() < i + 1)
        {
            return nullptr;
        }
        return to_c_label(&labels_vector->at(i));
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in nmbs_confidentiality_labels_get" << std::endl;
        return nullptr;
    }
}

const char* nmbs_confidentiality_label_get_policy(nmbs_confidentiality_label_ptr label) noexcept
{
    try
    {
        auto cpp_label = to_cpp_label(label);
        if (cpp_label == nullptr)
        {
            return nullptr;
        }
        return cpp_label->confidentiality_information.policy_identifier.c_str();
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in nmbs_confidentiality_label_get_policy" << std::endl;
        return nullptr;
    }
}

const char* nmbs_confidentiality_label_get_classification(nmbs_confidentiality_label_ptr label) noexcept
{
    try
    {
        auto cpp_label = to_cpp_label(label);
        if (cpp_label == nullptr)
        {
            return nullptr;
        }
        return cpp_label->confidentiality_information.classification.c_str();
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in nmbs_confidentiality_label_get_classification" << std::endl;
        return nullptr;
    }
}


void nmbs_confidentiality_label_set_policy(nmbs_confidentiality_label_ptr label, const char* string) noexcept
{
    try
    {
        auto cpp_label = to_cpp_label(label);
        if (!cpp_label) return;
        cpp_label->confidentiality_information.policy_identifier = string;
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in nmbs_confidentiality_label_set_policy" << std::endl;
    }
}

void nmbs_confidentiality_label_set_classification(nmbs_confidentiality_label_ptr label, const char* string) noexcept
{
    try
    {
        auto cpp_label = to_cpp_label(label);
        if (!cpp_label) return;
        cpp_label->confidentiality_information.classification = string;
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in nmbs_confidentiality_label_set_classification" << std::endl;
    }
}

int nmbs_confidentiality_labels_write_labels(const char* file, const nmbs_confidentiality_labels_ptr labels) noexcept
{
    try
    {
        const auto cpp_labels = to_cpp_labels(labels);
        if (!cpp_labels)
        {
            return nmbs::unknown_error;
        }
        auto const result = nmbs::write_labels(std::filesystem::path(std::string(file)), *cpp_labels);
        if (result.has_value())
        {
            return nmbs::success;
        }
        return result.error().code();
    }
    catch (const std::exception& e) {
        std::cerr << "C++ Exception caught in nmbs_write_labels: " << e.what() << std::endl;
        return nmbs::unknown_error;
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in nmbs_write_labels" << std::endl;
        return nmbs::unknown_error;
    }
}

[[nodiscard]] uint32_t nmbs_binding_flags_read_support(const char* file) noexcept
{
    if (!file) {
        return 0;
    }
    try
    {
        const std::filesystem::path target_path(file);
        nmbs::binding::flags cpp_flags = nmbs::binding::support(target_path);
        return static_cast<uint32_t>(cpp_flags);
    }
    catch (const std::exception& e) {
        std::cerr << "C++ Exception caught in nmbs_binding_support: " << e.what() << std::endl;
        return 0;
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in nmbs_binding_support" << std::endl;
        return 0;
    }
}

[[nodiscard]] nmbs_confidentiality_labels_ptr nmbs_confidentiality_labels_new() noexcept
{

    return to_c_labels(new std::vector<nmbs::confidentiality_label>());
}

void nmbs_confidentiality_labels_delete(nmbs_confidentiality_labels_ptr labels) noexcept
{
    try
    {
        delete to_cpp_labels(labels);
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in during nmbs_confidentiality_labels_delete. High chance of memory leaks!" << std::endl;
    }
}

[[nodiscard]] unsigned long nmbs_security_classifications_size(nmbs_security_classifications_ptr classifications) noexcept
{
    try
    {
        if (!classifications) return 0;
        const auto cpp_classifications = to_cpp_classifications(classifications);
        return cpp_classifications->size();
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in during nmbs_security_classifications_size." << std::endl;
        return 0;
    }
}

[[nodiscard]] nmbs_security_classification_ptr nmbs_security_classifications_get(nmbs_security_classifications_ptr classifications, unsigned long i) noexcept
{
    try
    {
        if (!classifications) return nullptr;
        const auto cpp_classifications = to_cpp_classifications(classifications);
        return to_c_classification(&cpp_classifications->at(i));
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in during nmbs_security_classifications_get." << std::endl;
        return nullptr;
    }
}

[[nodiscard]] const char* nmbs_security_classification_get_name(nmbs_security_classification_ptr classification) noexcept
{
    try
    {
        if (!classification) return nullptr;
        const auto cpp_classification = to_cpp_classification(classification);
        return cpp_classification->name.c_str();
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in during nmbs_security_classification_get_name." << std::endl;
        return nullptr;
    }
}

[[nodiscard]] nmbs_security_policies_ptr nmbs_security_policies_new() noexcept
{
    return to_c_policies(new std::vector<nmbs::spif::security_policy>);
}

void nmbs_security_policies_read_installed(nmbs_security_policies_ptr policies_out) noexcept
{
    try
    {
        const auto& policies = nmbs::spif::get_security_policies();
        const auto policies_cpp = to_cpp_policies(policies_out);
        policies_cpp->clear();
        policies_cpp->insert(policies_cpp->begin(), policies.begin(), policies.end());
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in nmbs_security_policies_read_installed" << std::endl;
    }
}

void nmbs_security_policies_delete(nmbs_security_policies_ptr policies) noexcept
{
    try
    {
        delete to_cpp_policies(policies);
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in during nmbs_security_policies_delete. High chance of memory leaks!" << std::endl;
    }
}

[[nodiscard]] nmbs_security_policy_ptr nmbs_security_policies_get(nmbs_security_policies_ptr policies, const unsigned long i) noexcept
{
    try
    {
        if (const auto cpp_policies = to_cpp_policies(policies); cpp_policies && cpp_policies->size() > i)
        {
            return to_c_policy(&cpp_policies->at(i));
        }
        return nullptr;
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in during nmbs_security_policies_get." << std::endl;
        return nullptr;
    }
}

[[nodiscard]] unsigned long nmbs_security_policies_size(nmbs_security_policies_ptr policies) noexcept
{
    try
    {
        if (!policies) return 0;
        const auto cpp_policies = to_cpp_policies(policies);
        return cpp_policies->size();
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in during nmbs_security_policies_size." << std::endl;
        return 0;
    }
}

[[nodiscard]] const char* nmbs_security_policy_get_name(nmbs_security_policy_ptr policy) noexcept
{
    try
    {
        if (!policy) return nullptr;
        const auto cpp_policy = to_cpp_policy(policy);
        return cpp_policy->name.c_str();
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in during nmbs_security_policy_name." << std::endl;
        return nullptr;
    }
}

[[nodiscard]] nmbs_security_classifications_ptr nmbs_security_policy_get_security_classifications(nmbs_security_policy_ptr policy) noexcept
{
    try
    {
        if (!policy) return nullptr;
        const auto cpp_policy = to_cpp_policy(policy);
        return to_c_classifications(&cpp_policy->security_classifications);
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in during nmbs_security_policy_get_security_classifications." << std::endl;
        return nullptr;
    }
}


bool nmbs_binding_flags_has_labels(const uint32_t flags) noexcept
{
    return nmbs::binding::has_labels(to_cpp_binding_flags(flags));
}

bool nmbs_binding_flags_supports_labels(const uint32_t flags) noexcept
{
    return nmbs::binding::supports_labels(to_cpp_binding_flags(flags));
}

void nmbs_cleanup() noexcept
{
    try
    {
        nmbs::cleanup();
    }
    catch (...)
    {
        std::cerr << "C++ Exception caught in during nmbs_cleanup." << std::endl;
    }
}


