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

#include "nmbs/nmbs_private.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <cstring>
#include <string>

#include "nmbs/expected.h"

namespace
{
    constexpr xmlChar target_namespace_4778[] = "urn:nato:stanag:4778:bindinginformation:1:0";
    constexpr xmlChar target_namespace_4774[] = "urn:nato:stanag:4774:confidentialitymetadatalabel:1:0";

    // RAII Custom Deleters
    struct XmlDocDeleter { void operator()(xmlDoc* const d) const { xmlFreeDoc(d); } };
    struct XmlCharDeleter { void operator()(xmlChar* const s) const { xmlFree(s); } };
    struct XmlXPathCtxDeleter { void operator()(xmlXPathContext* const c) const { xmlXPathFreeContext(c); } };
    struct XmlXPathObjDeleter { void operator()(xmlXPathObject* const o) const { xmlXPathFreeObject(o); } };
    struct XmlTextReaderDeleter { void operator()(xmlTextReader* const o) const { xmlFreeTextReader(o); } };

    /// Helper for relative XPath queries. In particular, this helper will return only the string value of the
    /// searched node. It is useful for cases where exactly one node, with no attributes or children are expected.
    /// @param context_node
    /// @param ctx
    /// @param xpath_expr
    /// @return
    [[nodiscard]] std::optional<std::string> get_relative_xpath_node_value(xmlNode* context_node, xmlXPathContext* ctx, const std::string& xpath_expr) {
        const std::unique_ptr<xmlXPathObject, XmlXPathObjDeleter> obj(
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

    [[nodiscard]] bool is_a_node_named(const xmlNode* node, const xmlChar* name)
    {
        return node->type == XML_ELEMENT_NODE &&
            xmlStrEqual(target_namespace_4778, node->ns->href) &&
            xmlStrEqual(name, node->name);
    }

    [[nodiscard]] bool is_a_4774_node_named(const xmlNode* node, const xmlChar* name)
    {
        return node->type == XML_ELEMENT_NODE &&
            xmlStrEqual(target_namespace_4774, node->ns->href) &&
            xmlStrEqual(name, node->name);
    }

    [[nodiscard]] std::string get_attribute_value(const xmlAttr* attribute) {
        if (!attribute || !attribute->children)
        {
            return "";
        }
        const std::unique_ptr<xmlChar, XmlCharDeleter> value(xmlNodeGetContent(attribute->children));
        return value ? reinterpret_cast<const char*>(value.get()) : "";
    }

    [[nodiscard]] std::optional<std::string> get_node_text(const xmlNode* node) {
        if (!node || !node->children)
        {
            return std::nullopt;
        }
        const std::unique_ptr<xmlChar, XmlCharDeleter> value(xmlNodeGetContent(node));
        if (value) [[likely]]
        {
            return std::string(reinterpret_cast<const char*>(value.get()));
        }
        return std::nullopt;
    }

    [[nodiscard]] nmbs::Expected<nmbs::binding::BindingInformation> deserialise_binding_information_xml_doc(const std::unique_ptr<xmlDoc, XmlDocDeleter>& xml_doc)
    {
        if (!xml_doc)
        {
            return std::unexpected(nmbs::Error::xml_could_not_parse("Found an s4778:BindingInformation, but could not parse it."));
        }

        nmbs::binding::BindingInformation return_binding_information;
        xmlNodePtr root = xmlDocGetRootElement(xml_doc.get());
        if (!xmlStrEqual(root->name, reinterpret_cast<const xmlChar*>("BindingInformation")) ||
            !xmlStrEqual(root->ns->href, target_namespace_4778))
        {
            return std::unexpected(nmbs::Error::xml_could_not_parse("deserialise_binding_information_xml_doc passed an unknown xml element"));
        }

        for (xmlNodePtr binding_information_child = root->children; binding_information_child; binding_information_child = binding_information_child->next)
        {
            if (is_a_node_named(binding_information_child, reinterpret_cast<const xmlChar*>("MetadataBindingContainer")))
            {
                for (xmlNodePtr binding_container = binding_information_child->children; binding_container; binding_container = binding_container->next)
                {
                    if (is_a_node_named(binding_container, reinterpret_cast<const xmlChar*>("MetadataBinding")))
                    {
                        for (xmlNodePtr metadata_binding_child = binding_container->children; metadata_binding_child; metadata_binding_child = metadata_binding_child->next)
                        {
                            if (is_a_node_named(metadata_binding_child, reinterpret_cast<const xmlChar*>("Metadata")))
                            {
                                for (xmlNodePtr metadata_child = metadata_binding_child->children; metadata_child; metadata_child = metadata_child->next)
                                {
                                    if (is_a_4774_node_named(metadata_child, reinterpret_cast<const xmlChar*>("originatorConfidentialityLabel")) ||
                                        is_a_4774_node_named(metadata_child, reinterpret_cast<const xmlChar*>("alternativeConfidentialityLabel")) ||
                                        is_a_4774_node_named(metadata_child, reinterpret_cast<const xmlChar*>("metadataConfidentialityLabel")))
                                    {
                                        nmbs::ConfidentialityLabel confidentiality_label;

                                        if (xmlStrEqual(metadata_child->name, reinterpret_cast<const xmlChar*>("originatorConfidentialityLabel")))
                                        {
                                            confidentiality_label.label_type = nmbs::ConfidentialityLabel::originator;
                                        }
                                        else if (xmlStrEqual(metadata_child->name, reinterpret_cast<const xmlChar*>("alternativeConfidentialityLabel")))
                                        {
                                            confidentiality_label.label_type = nmbs::ConfidentialityLabel::alternative;
                                        }
                                        else
                                        {
                                            confidentiality_label.label_type = nmbs::ConfidentialityLabel::successor;
                                        }

                                        for (xmlNodePtr label_child = metadata_child->children; label_child; label_child = label_child->next)
                                        {
                                            if (is_a_4774_node_named(label_child, reinterpret_cast<const xmlChar*>("CreationDateTime")))
                                            {
                                                const auto creation_time_string = get_node_text(label_child);
                                                if (creation_time_string->empty())
                                                {
                                                    return std::unexpected(nmbs::Error::xml_could_not_parse("Could not parse *ConfidentialityLabel element. No CreationDateTime, although it is a mandatory element."));
                                                }
                                                std::istringstream in_stream(creation_time_string.value());
                                                // TODO: This is not tolerant of +00:00! Make a more flexible date-time reading function
                                                const std::string in_format = "%FT%TZ";
                                                in_stream >> std::chrono::parse(in_format, confidentiality_label.creation_date_time);
                                                if (in_stream.fail()) {
                                                    return std::unexpected(nmbs::Error::xml_could_not_parse("Could not parse *ConfidentialityLabel element. CreationDateTime could not be parsed. Maybe wrong format?"));
                                                }
                                            }
                                            else if (is_a_4774_node_named(label_child, reinterpret_cast<const xmlChar*>("ConfidentialityInformation")))
                                            {
                                                for (xmlNodePtr information_child = label_child->children; information_child; information_child = information_child->next)
                                                {
                                                    if (is_a_4774_node_named(information_child, reinterpret_cast<const xmlChar*>("PolicyIdentifier")))
                                                    {
                                                        confidentiality_label.confidentiality_information.policy_identifier = get_node_text(information_child).value_or("ERROR!");
                                                    }
                                                    else if (is_a_4774_node_named(information_child, reinterpret_cast<const xmlChar*>("Classification")))
                                                    {
                                                        confidentiality_label.confidentiality_information.classification = get_node_text(information_child).value_or("ERROR!");
                                                    }
                                                }
                                            }
                                            else if (is_a_4774_node_named(label_child, reinterpret_cast<const xmlChar*>("OriginatorID")))
                                            {
                                                nmbs::ConfidentialityLabel::OriginatorId originator_id;
                                                auto originator_id_name = get_node_text(label_child);
                                                if (originator_id_name->empty())
                                                {
                                                    return std::unexpected(nmbs::Error::xml_could_not_parse("Could not parse OriginatorID element. No Text content, although it is a mandatory element."));
                                                }
                                                originator_id.value = originator_id_name.value();
                                                for (xmlAttrPtr originator_id_property = label_child->properties; originator_id_property; originator_id_property = originator_id_property->next)
                                                {
                                                    if (xmlStrEqual(reinterpret_cast<const xmlChar*>("IDType"), originator_id_property->name))
                                                    {
                                                        originator_id.id_type = get_attribute_value(originator_id_property);
                                                    }
                                                }
                                                confidentiality_label.originator_id = originator_id;
                                            }
                                        }
                                        return_binding_information.labels.push_back(confidentiality_label);
                                    }
                                }
                            }
                            else if (is_a_node_named(metadata_binding_child, reinterpret_cast<const xmlChar*>("DataReference")))
                            {
                                for (xmlAttrPtr data_reference_property = metadata_binding_child->properties; data_reference_property; data_reference_property = data_reference_property->next)
                                {
                                    if (xmlStrEqual(reinterpret_cast<const xmlChar*>("URI"), data_reference_property->name))
                                    {
                                        return_binding_information.reference.uri = get_attribute_value(data_reference_property);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        return return_binding_information;
    }

}

namespace nmbs::serialisation
{
    void cleanup()
    {
        xmlCleanupParser();
    }

    [[nodiscard]] std::string encode_base64(const std::string& input)
    {
        xmlBuffer* const xml_buffer = xmlBufferCreate();
        if (!xml_buffer)
        {
            return "";
        }

        xmlTextWriter* const writer = xmlNewTextWriterMemory(xml_buffer, 0);
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

    [[nodiscard]] std::string decode_base64(const std::string_view input)
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

    Expected<std::optional<std::string>> read_binding_information_xml_from_file(const std::filesystem::path& file)
    {
        if (!std::filesystem::is_regular_file(file))
        {
            return std::unexpected(Error::file_not_found());
        }
        LIBXML_TEST_VERSION

        const std::unique_ptr<xmlTextReader, XmlTextReaderDeleter> reader(xmlReaderForFile(file.c_str(), nullptr, 0));
        while (xmlTextReaderRead(reader.get()))
        {
            // Check only name for now. Its very unlikely to hit, so we should optimise as much as possible till
            // here
            if (xmlTextReaderNodeType(reader.get()) == XML_READER_TYPE_ELEMENT &&
                xmlStrEqual(reinterpret_cast<const xmlChar*>("BindingInformation"), xmlTextReaderConstLocalName(reader.get()))) [[unlikely]]
            {
                if (xmlStrEqual(target_namespace_4778, xmlTextReaderConstNamespaceUri(reader.get()))) [[likely]]
                {
                    const std::unique_ptr<xmlChar, XmlCharDeleter> outer_xml(xmlTextReaderReadOuterXml(reader.get()));
                    if (!outer_xml)
                    {
                        return std::unexpected(Error::xml_could_not_parse("Found an s4778:BindingInformation, but could not read it."));
                    }
                    return std::string(reinterpret_cast<char*>(outer_xml.get()));
                }
                std::cerr << "Warning: During xml Parsing an element BindingInformation was found, but was not using the namespace urn:nato:stanag:4778:bindinginformation:1:0. This element will be ignored by libnmbs as it may belong to another application." << std::endl;
            }
        }
        return std::nullopt;
    }


    // Will just try to get the binding information from an XML file. Should be irrelevant if a bdo or in some embedded file.
    Expected<std::optional<binding::BindingInformation>> deserialise_binding_information_from_file(const std::filesystem::path& file)
    {
        if (!std::filesystem::is_regular_file(file))
        {
            return std::unexpected(Error::file_not_found());
        }
        LIBXML_TEST_VERSION

        const std::unique_ptr<xmlTextReader, XmlTextReaderDeleter> reader(xmlReaderForFile(file.c_str(), nullptr, 0));
        while (xmlTextReaderRead(reader.get()))
        {
            // Check only name for now. Its very unlikely to hit, so we should optimise as much as possible till
            // here
            if (xmlTextReaderNodeType(reader.get()) == XML_READER_TYPE_ELEMENT &&
                xmlStrEqual(reinterpret_cast<const xmlChar*>("BindingInformation"), xmlTextReaderConstLocalName(reader.get()))) [[unlikely]]
            {
                if (xmlStrEqual(target_namespace_4778, xmlTextReaderConstNamespaceUri(reader.get()))) [[likely]]
                {
                    const std::unique_ptr<xmlChar, XmlCharDeleter> outer_xml(xmlTextReaderReadOuterXml(reader.get()));
                    const std::unique_ptr<xmlDoc, XmlDocDeleter> binding_xml_doc(xmlReadMemory(
                        reinterpret_cast<const char*>(outer_xml.get()),
                        xmlStrlen(outer_xml.get()),
                        nullptr,
                        nullptr,
                        0));

                    if (!binding_xml_doc)
                    {
                        return std::unexpected(Error::xml_could_not_parse("Found an s4778:BindingInformation, but could not parse it."));
                    }

                    // NOTE: We only support one s4778:BindingInformation per file!
                    return deserialise_binding_information_xml_doc(binding_xml_doc);
                }
                std::cerr << "Warning: During xml Parsing an element BindingInformation was found, but was not using the namespace urn:nato:stanag:4778:bindinginformation:1:0. This element will be ignored by libnmbs as it may belong to another application." << std::endl;
            }
        }
        return std::nullopt;
    }

    Expected<binding::BindingInformation> deserialise_binding_information(const std::string& xml)
    {
        const std::unique_ptr<xmlDoc, XmlDocDeleter> xml_doc(
            xmlReadMemory(xml.c_str(), static_cast<int>(xml.length()), nullptr, nullptr, 0)
        );
        if (!xml_doc)
        {
            return std::unexpected(Error::xml_could_not_parse());
        }
        return deserialise_binding_information_xml_doc(xml_doc);
    }

    [[nodiscard]] Expected<spif::SecurityPolicy> deserialise_security_policy(const std::string& xml)
    {
        spif::SecurityPolicy spif;

        const std::unique_ptr<xmlDoc, XmlDocDeleter> xml_doc(
            xmlReadMemory(xml.c_str(), static_cast<int>(xml.length()), nullptr, nullptr, 0)
        );
        if (!xml_doc)
        {
            return std::unexpected(Error::xml_could_not_parse());
        }

        const std::unique_ptr<xmlXPathContext, XmlXPathCtxDeleter> xpath_ctx(xmlXPathNewContext(xml_doc.get()));
        if (!xpath_ctx)
        {
            return std::unexpected(Error::xml_could_not_create_xpath_context());
        }

        // Here we go to string as the string_view handles null termination differently. The tiny overhead is not a
        // concern!
        // ReSharper disable CppVariableCanBeMadeConstexpr
        const std::string spif_prefix(spif::spif_prefix);
        const std::string spif_namespace(spif::spif_namespace);
        // ReSharper restore CppVariableCanBeMadeConstexpr

        xmlXPathRegisterNs(xpath_ctx.get(), reinterpret_cast<const xmlChar*>(spif_prefix.c_str()),
                           reinterpret_cast<const xmlChar*>(spif_namespace.c_str()));

        const std::unique_ptr<xmlXPathObject, XmlXPathObjDeleter> binding_information_element(
            xmlXPathEvalExpression(reinterpret_cast<const xmlChar*>("//spif:SPIF"), xpath_ctx.get()));
        if (!binding_information_element || binding_information_element->nodesetval->nodeNr == 0)
        {
            return std::unexpected(Error::xml_could_not_parse("Could not parse SPIF. Could not find SPIF Element"));
        }
        if (binding_information_element->nodesetval->nodeNr != 1)
        {
            return std::unexpected(Error::xml_could_not_parse("Could not parse SPIF. Found multiple SPIF Elements"));
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
            return std::unexpected(Error::xml_could_not_parse("Could not parse SPIF. ./@version was missing or invalid"));
        }

        auto security_classification_nodes = std::unique_ptr<xmlXPathObject, XmlXPathObjDeleter>(xmlXPathEvalExpression(
            reinterpret_cast<const xmlChar*>("//spif:securityClassification"), xpath_ctx.get()));
        if (!security_classification_nodes || xmlXPathNodeSetIsEmpty(security_classification_nodes->nodesetval))
        {
            return std::unexpected(Error::xml_could_not_parse("Could not parse SPIF. Found no securityClassification Elements"));
        }

        for (int i = 0; i < security_classification_nodes->nodesetval->nodeNr; ++i)
        {
            xmlNode* security_classification_node = security_classification_nodes->nodesetval->nodeTab[i];
            spif::SecurityClassification current_classification;

            current_classification.name = get_relative_xpath_node_value(security_classification_node, xpath_ctx.get(),"./@name").value();

            try
            {
                current_classification.hierarchy = std::stoi(get_relative_xpath_node_value(security_classification_node, xpath_ctx.get(), "./@hierarchy").value());
            }
            catch (...)
            {
                return std::unexpected(Error::xml_could_not_parse("Could not parse SPIF. securityClassification/@hierarchy was missing or invalid"));
            }

            try
            {
                current_classification.lacv = std::stoi(get_relative_xpath_node_value(security_classification_node, xpath_ctx.get(), "./@lacv").value());
            }
            catch (...)
            {
                return std::unexpected(Error::xml_could_not_parse("Could not parse SPIF. securityClassification/@lacv was missing or invalid"));
            }

            current_classification.colour = get_relative_xpath_node_value(security_classification_node, xpath_ctx.get(), "./@color");
            auto obsolete = get_relative_xpath_node_value(security_classification_node, xpath_ctx.get(), "./@obsolete");
            current_classification.obsolete = obsolete.has_value() &&  obsolete.value() == "true";
            spif.security_classifications.push_back(current_classification);
        }

        return spif;
    }

    [[nodiscard]] std::string serialise_binding_information(const binding::BindingInformation& binding_information)
    {
        std::string xml;
        xml.reserve(1024);
        xml.append(std::format(R"(<{0}:BindingInformation xmlns:{0}="{1}"><{0}:MetadataBindingContainer><{0}:MetadataBinding><{0}:Metadata>{2}</{0}:Metadata>)", binding::s4778_prefix, binding::s4778_namespace, serialise_confidentiality_labels(binding_information.labels)));
        xml.append(std::format(R"(<{0}:DataReference URI="{1}" />)", binding::s4778_prefix, binding_information.reference.uri));
        xml.append(std::format(R"(</{0}:MetadataBinding></{0}:MetadataBindingContainer></{0}:BindingInformation>)", binding::s4778_prefix));
        return xml;
    }

    [[nodiscard]] std::string serialise_confidentiality_labels(const std::vector<ConfidentialityLabel>& confidentiality_labels)
    {
        std::string xml;
        for (const auto& confidentiality_label : confidentiality_labels)
        {
            xml.append(serialise_confidentiality_label(confidentiality_label));
        }
        return xml;
    }

    [[nodiscard]] std::string serialise_confidentiality_label(const ConfidentialityLabel& confidentiality_label)
    {
        const std::string creation_time_string = std::format("{:%FT%TZ}", confidentiality_label.creation_date_time);

        std::string xml;
        xml.reserve(1024);
        xml.append(std::format(R"(<{0}:originatorConfidentialityLabel xmlns:{0}="{1}"><{0}:ConfidentialityInformation>)", binding::s4774_prefix, binding::s4774_namespace));
        xml.append(std::format("<{0}:PolicyIdentifier>{1}</{0}:PolicyIdentifier>", binding::s4774_prefix, confidentiality_label.confidentiality_information.policy_identifier));
        xml.append(std::format("<{0}:Classification>{1}</{0}:Classification></{0}:ConfidentialityInformation>", binding::s4774_prefix, confidentiality_label.confidentiality_information.classification));
        if (confidentiality_label.originator_id.has_value())
        {
            xml.append(std::format(R"(<{0}:OriginatorID IDType="{1}">{2}</{0}:OriginatorID>)", binding::s4774_prefix, confidentiality_label.originator_id.value().id_type, confidentiality_label.originator_id.value().value));
        }
        xml.append(std::format("<{0}:CreationDateTime>{1}</{0}:CreationDateTime>", binding::s4774_prefix, creation_time_string));
        xml.append(std::format("</{0}:originatorConfidentialityLabel>", binding::s4774_prefix));
        return xml;
    }

}
