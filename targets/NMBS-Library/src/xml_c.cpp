/// @file xml_c.cpp
/// @brief nmbs::xml xmllib interactions including C code calls
///
/// @author Luke Ian Turner
/// @date 2026-06-26
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

#include "nmbs_private.h"
#include "nmbs/binding.h"
#include "nmbs/constants.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlwriter.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <cstring>
#include <string>

#include "nmbs/expected.h"

namespace nmbs::xml
{
    // 2. RAII Custom Deleters
    struct XmlDocDeleter { void operator()(xmlDoc* d) const { xmlFreeDoc(d); } };
    struct XmlCharDeleter { void operator()(xmlChar* s) const { xmlFree(s); } };
    struct XmlXPathCtxDeleter { void operator()(xmlXPathContext* c) const { xmlXPathFreeContext(c); } };
    struct XmlXPathObjDeleter { void operator()(xmlXPathObject* o) const { xmlXPathFreeObject(o); } };

    [[nodiscard]] std::string encode_base64(const std::string& input)
    {
        xmlBufferPtr const xml_buffer = xmlBufferCreate();
        if (!xml_buffer)
        {
            return "";
        }

        xmlTextWriterPtr const writer = xmlNewTextWriterMemory(xml_buffer, 0);
        if (!writer) {
            xmlBufferFree(xml_buffer);
            return "";
        }

        xmlTextWriterWriteBase64(writer, input.c_str(), 0, input.length());
        xmlFreeTextWriter(writer);

        std::string result(reinterpret_cast<char*>(xml_buffer->content), xml_buffer->use);
        xmlBufferFree(xml_buffer);

        return result;
    }

    [[nodiscard]] std::string decode_base64(std::string_view input)
    {
        std::string out;
        out.reserve((input.size() / 4) * 3); // Pre-allocate exact memory needed

        // Clean, branchless-style lookup lambda
        auto get_val = [](unsigned char c) -> int {
            if (c >= 'A' && c <= 'Z') return c - 'A';
            if (c >= 'a' && c <= 'z') return c - 'a' + 26;
            if (c >= '0' && c <= '9') return c - '0' + 52;
            if (c == '+') return 62;
            if (c == '/') return 63;
            return -1; // Newlines, spaces, or invalid chars
        };

        int val = 0, valb = -8;
        for (unsigned char c : input) {
            if (c == '=') break; // Reached the padding end

            int v = get_val(c);
            if (v == -1) continue; // Skip XML line-breaks safely

            val = (val << 6) + v;
            valb += 6;

            if (valb >= 0) {
                out.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        return out;
    }

    /// Helper for relative XPath queries. In particular, this helper will return only the string value of the
    /// searched node. It is useful for cases where exactly one node, with no attributes or children are expected.
    /// @param context_node
    /// @param ctx
    /// @param xpath_expr
    /// @return
    [[nodiscard]] std::optional<std::string> get_relative_xpath_node_value(xmlNode* context_node, xmlXPathContext* ctx, const std::string& xpath_expr) {
        std::unique_ptr<xmlXPathObject, XmlXPathObjDeleter> obj(
            xmlXPathNodeEval(context_node, reinterpret_cast<const xmlChar*>(xpath_expr.c_str()), ctx)
        );

        if (obj && !xmlXPathNodeSetIsEmpty(obj->nodesetval)) {
            const xmlNode* node = obj->nodesetval->nodeTab[0];
            const std::unique_ptr<xmlChar, XmlCharDeleter> content(xmlNodeGetContent(node));
            if (content) {
                return std::string(reinterpret_cast<const char*>(content.get()));
            }
        }
        return std::nullopt;
    }


    expected<binding::binding_information> deserialise_binding_information(const std::string& xml)
    {
        binding::binding_information bdo;

        const std::unique_ptr<xmlDoc, XmlDocDeleter> xml_doc(
            xmlReadMemory(xml.c_str(), static_cast<int>(xml.length()), nullptr, nullptr, 0)
        );
        if (!xml_doc) {
            return std::unexpected(error::xml_could_not_parse());
        }

        const std::unique_ptr<xmlXPathContext, XmlXPathCtxDeleter> xpath_ctx(xmlXPathNewContext(xml_doc.get()));
        if (!xpath_ctx)
        {
            return std::unexpected(error::xml_could_not_create_xpath_context());
        }

        // Here we go to string as the string_view handles null termination differently. The tiny overhead is not a
        // concern!
        // ReSharper disable CppVariableCanBeMadeConstexpr
        const std::string s4778_prefix(nmbs::constants::s4778_prefix);
        const std::string s4778_namespace(nmbs::constants::s4778_namespace);
        const std::string s4774_prefix(nmbs::constants::s4774_prefix);
        const std::string s4774_namespace(nmbs::constants::s4774_namespace);
        // ReSharper restore CppVariableCanBeMadeConstexpr

        xmlXPathRegisterNs(xpath_ctx.get(), reinterpret_cast<const xmlChar*>(s4778_prefix.c_str()), reinterpret_cast<const xmlChar*>(s4778_namespace.c_str()));
        xmlXPathRegisterNs(xpath_ctx.get(), reinterpret_cast<const xmlChar*>(s4774_prefix.c_str()), reinterpret_cast<const xmlChar*>(s4774_namespace.c_str()));


        const std::unique_ptr<xmlXPathObject, XmlXPathObjDeleter> binding_information_element(xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>("//s4778:BindingInformation"), xpath_ctx.get()));
        if (!binding_information_element || binding_information_element->nodesetval->nodeNr == 0) {
            return std::unexpected(error::xml_could_not_parse("Could not parse XML. Could not find BindingInformation Element"));
        }

        if (binding_information_element->nodesetval->nodeNr != 1) {
            return std::unexpected(error::xml_could_not_parse("Could not parse XML. Found multiple BindingInformation Elements"));
        }

        bdo.reference.uri = get_relative_xpath_node_value(binding_information_element->nodesetval->nodeTab[0], xpath_ctx.get(), ".//s4778:DataReference/@URI").value_or("");

        const std::unique_ptr<xmlXPathObject, XmlXPathObjDeleter> confidentiality_label_collections[3]
        {
            std::unique_ptr<xmlXPathObject, XmlXPathObjDeleter>(xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>("//s4774:originatorConfidentialityLabel"), xpath_ctx.get())),
            std::unique_ptr<xmlXPathObject, XmlXPathObjDeleter>(xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>("//s4774:alternativeConfidentialityLabel"), xpath_ctx.get())),
            std::unique_ptr<xmlXPathObject, XmlXPathObjDeleter>(xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>("//s4774:metadataConfidentialityLabel"), xpath_ctx.get()))
        };

        for (const auto & confidentiality_label_collection : confidentiality_label_collections)
        {
            if (!confidentiality_label_collection || xmlXPathNodeSetIsEmpty(confidentiality_label_collection->nodesetval)) {
                continue;
            }

            xmlNodeSetPtr confidentiality_label_nodes = confidentiality_label_collection->nodesetval;

            for (int i = 0; i < confidentiality_label_nodes->nodeNr; ++i) {
                xmlNode* binding_node = confidentiality_label_nodes->nodeTab[i];
                nmbs::confidentiality_label current_label;

                const std::string current_node_name(reinterpret_cast<const char*>(binding_node->name));
                if (current_node_name == "originatorConfidentialityLabel") {
                    current_label.label_type = confidentiality_label::originator;
                } else if (current_node_name == "alternativeConfidentialityLabel") {
                    current_label.label_type = confidentiality_label::alternative;
                } else if (current_node_name == "metadataConfidentialityLabel") {
                    current_label.label_type = confidentiality_label::successor;
                } else {
                    // TODO: Possibly cerr a warning here? At this point it is wierd if there is anything other than the three cases, but I dont think its worth killing the function? Consider in depth.
                    continue;
                }

                // The './/' means "search anywhere inside the current node".
                auto node_value = get_relative_xpath_node_value(
                    binding_node, xpath_ctx.get(), ".//s4774:PolicyIdentifier"
                );
                if (!node_value)
                {
                    return std::unexpected(error::xml_could_not_parse("Could not parse ConfidentialityInformation element. No PolicyIdentifier, although it is a mandatory element."));
                }
                current_label.confidentiality_information.policy_identifier = node_value.value();


                node_value = get_relative_xpath_node_value(
                    binding_node, xpath_ctx.get(), ".//s4774:Classification"
                );
                if (!node_value)
                {
                    return std::unexpected(error::xml_could_not_parse("Could not parse ConfidentialityInformation element. No Classification, although it is a mandatory element."));
                }
                current_label.confidentiality_information.classification = node_value.value();


                node_value = get_relative_xpath_node_value(
                    binding_node, xpath_ctx.get(), ".//s4774:CreationDateTime"
                );
                if (!node_value)
                {
                    return std::unexpected(error::xml_could_not_parse("Could not parse *ConfidentialityLabel element. No CreationDateTime, although it is a mandatory element."));
                }

                std::istringstream in_stream(node_value.value());
                // TODO: This is not tollerant of +00:00! Make a more flexible date-time reading function
                const std::string in_format = "%FT%TZ";
                in_stream >> std::chrono::parse(in_format, current_label.creation_date_time);
                if (in_stream.fail()) {
                    return std::unexpected(error::xml_could_not_parse("Could not parse *ConfidentialityLabel element. CreationDateTime could not be parsed. Maybe wrong format?"));
                }

                auto originator_id_type= get_relative_xpath_node_value(binding_node, xpath_ctx.get(), ".//s4774:OriginatorID/@IDType");
                auto originator_id_value= get_relative_xpath_node_value(binding_node, xpath_ctx.get(), ".//s4774:OriginatorID");
                if (originator_id_type.has_value() || originator_id_value.has_value())
                {
                    current_label.originator_id.emplace();;
                    current_label.originator_id->id_type = originator_id_type.value_or("");
                    current_label.originator_id->value = originator_id_value.value_or("");
                }

                bdo.labels.push_back(current_label);
            }
        }

        return bdo;
    }

    [[nodiscard]] expected<spif::security_policy> deserialise_security_policy(const std::string& xml)
    {
        spif::security_policy spif;

        const std::unique_ptr<xmlDoc, XmlDocDeleter> xml_doc(
            xmlReadMemory(xml.c_str(), static_cast<int>(xml.length()), nullptr, nullptr, 0)
        );
        if (!xml_doc)
        {
            return std::unexpected(error::xml_could_not_parse());
        }

        const std::unique_ptr<xmlXPathContext, XmlXPathCtxDeleter> xpath_ctx(xmlXPathNewContext(xml_doc.get()));
        if (!xpath_ctx)
        {
            return std::unexpected(error::xml_could_not_create_xpath_context());
        }

        // Here we go to string as the string_view handles null termination differently. The tiny overhead is not a
        // concern!
        // ReSharper disable CppVariableCanBeMadeConstexpr
        const std::string spif_prefix(nmbs::constants::spif_prefix);
        const std::string spif_namespace(nmbs::constants::spif_namespace);
        // ReSharper restore CppVariableCanBeMadeConstexpr

        xmlXPathRegisterNs(xpath_ctx.get(), reinterpret_cast<const xmlChar*>(spif_prefix.c_str()),
                           reinterpret_cast<const xmlChar*>(spif_namespace.c_str()));

        const std::unique_ptr<xmlXPathObject, XmlXPathObjDeleter> binding_information_element(
            xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>("//spif:SPIF"), xpath_ctx.get()));
        if (!binding_information_element || binding_information_element->nodesetval->nodeNr == 0)
        {
            return std::unexpected(error::xml_could_not_parse("Could not parse SPIF. Could not find SPIF Element"));
        }
        if (binding_information_element->nodesetval->nodeNr != 1)
        {
            return std::unexpected(error::xml_could_not_parse("Could not parse SPIF. Found multiple SPIF Elements"));
        }

        xmlNodePtr root_element = binding_information_element->nodesetval->nodeTab[0];

        spif.name = get_relative_xpath_node_value(root_element, xpath_ctx.get(), ".//spif:securityPolicyId/@name").value();
        spif.id = get_relative_xpath_node_value(root_element, xpath_ctx.get(), ".//spif:securityPolicyId/@id").value();
        try
        {
            spif.version = std::stoi(get_relative_xpath_node_value(root_element, xpath_ctx.get(), "./@version").value());
        }
        catch (...)
        {
            return std::unexpected(error::xml_could_not_parse("Could not parse SPIF. ./@version was missing or invalid"));
        }

        auto security_classification_nodes = std::unique_ptr<xmlXPathObject, XmlXPathObjDeleter>(xmlXPathEvalExpression(
            reinterpret_cast<const xmlChar*>("//spif:securityClassification"), xpath_ctx.get()));
        if (!security_classification_nodes || xmlXPathNodeSetIsEmpty(security_classification_nodes->nodesetval))
        {
            return std::unexpected(error::xml_could_not_parse("Could not parse SPIF. Found no securityClassification Elements"));
        }

        for (int i = 0; i < security_classification_nodes->nodesetval->nodeNr; ++i)
        {
            xmlNode* security_classification_node = security_classification_nodes->nodesetval->nodeTab[i];
            spif::security_classification current_classification;

            current_classification.name = get_relative_xpath_node_value(security_classification_node, xpath_ctx.get(),"./@name").value();

            try
            {
                current_classification.hierarchy = std::stoi(get_relative_xpath_node_value(security_classification_node, xpath_ctx.get(), "./@hierarchy").value());
            }
            catch (...)
            {
                return std::unexpected(error::xml_could_not_parse("Could not parse SPIF. securityClassification/@hierarchy was missing or invalid"));
            }

            try
            {
                current_classification.lacv = std::stoi(get_relative_xpath_node_value(security_classification_node, xpath_ctx.get(), "./@lacv").value());
            }
            catch (...)
            {
                return std::unexpected(error::xml_could_not_parse("Could not parse SPIF. securityClassification/@lacv was missing or invalid"));
            }

            current_classification.colour = get_relative_xpath_node_value(security_classification_node, xpath_ctx.get(), "./@color");
            auto obsolete = get_relative_xpath_node_value(security_classification_node, xpath_ctx.get(), "./@obsolete");
            current_classification.obsolete = obsolete.has_value() &&  obsolete.value() == "true";
            spif.security_classifications.push_back(current_classification);
        }

        return spif;
    }

    [[nodiscard]] std::string serialise_binding_information(const binding::binding_information& binding_information)
    {
        std::string xml;
        xml.reserve(1024);
        xml.append(std::format(R"(<{0}:BindingInformation xmlns:{0}="{1}"><{0}:MetadataBindingContainer><{0}:MetadataBinding><{0}:Metadata>{2}</{0}:Metadata>)", constants::s4778_prefix, constants::s4778_namespace, serialise_confidentiality_labels(binding_information.labels)));
        xml.append(std::format(R"(<{0}:DataReference URI="{1}" />)", constants::s4778_prefix, binding_information.reference.uri));
        xml.append(std::format(R"(</{0}:MetadataBinding></{0}:MetadataBindingContainer></{0}:BindingInformation>)", constants::s4778_prefix));
        return xml;
    }

    [[nodiscard]] std::string serialise_confidentiality_labels(const std::vector<confidentiality_label>& confidentiality_labels)
    {
        std::string xml;
        for (const auto& confidentiality_label : confidentiality_labels)
        {
            xml.append(serialise_confidentiality_label(confidentiality_label));
        }
        return xml;
    }

    [[nodiscard]] std::string serialise_confidentiality_label(const confidentiality_label& confidentiality_label)
    {
        const std::string creation_time_string = std::format("{:%FT%TZ}", confidentiality_label.creation_date_time);

        std::string xml;
        xml.reserve(1024);
        xml.append(std::format(R"(<{0}:originatorConfidentialityLabel xmlns:{0}="{1}"><{0}:ConfidentialityInformation>)", constants::s4774_prefix, constants::s4774_namespace));
        xml.append(std::format("<{0}:PolicyIdentifier>{1}</{0}:PolicyIdentifier>", constants::s4774_prefix, confidentiality_label.confidentiality_information.policy_identifier));
        xml.append(std::format("<{0}:Classification>{1}</{0}:Classification></{0}:ConfidentialityInformation>", constants::s4774_prefix, confidentiality_label.confidentiality_information.classification));
        if (confidentiality_label.originator_id.has_value())
        {
            xml.append(std::format(R"(<{0}:OriginatorID IDType="{1}">{2}</{0}:OriginatorID>)", constants::s4774_prefix, confidentiality_label.originator_id.value().id_type, confidentiality_label.originator_id.value().value));
        }
        xml.append(std::format("<{0}:CreationDateTime>{1}</{0}:CreationDateTime>", constants::s4774_prefix, creation_time_string));
        xml.append(std::format("</{0}:originatorConfidentialityLabel>", constants::s4774_prefix));
        return xml;
    }

}
