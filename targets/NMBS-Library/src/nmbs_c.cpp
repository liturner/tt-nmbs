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

#include <iostream>
#include <sstream>
#include <chrono>
#include <string>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "nmbs/confidentiality_label.h"
#include "nmbs/nmbs.h"

namespace nmbs::xml
{
    // 2. RAII Custom Deleters
    struct XmlDocDeleter { void operator()(xmlDoc* d) const { xmlFreeDoc(d); } };
    struct XmlCharDeleter { void operator()(xmlChar* s) const { xmlFree(s); } };
    struct XmlXPathCtxDeleter { void operator()(xmlXPathContext* c) const { xmlXPathFreeContext(c); } };
    struct XmlXPathObjDeleter { void operator()(xmlXPathObject* o) const { xmlXPathFreeObject(o); } };

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


    std::string to_xml(const nmbs::confidentiality_label& label)
    {
        return "";
    }

    std::vector<confidentiality_label> from_xml(const std::string& xml)
    {
        std::vector<confidentiality_label> results;

        const std::unique_ptr<xmlDoc, XmlDocDeleter> xml_doc(
            xmlReadMemory(xml.c_str(), static_cast<int>(xml.length()), nullptr, nullptr, 0)
        );
        if (!xml_doc) {
            throw exceptions::xml_could_not_parse_exception();
        }

        const std::unique_ptr<xmlXPathContext, XmlXPathCtxDeleter> xpath_ctx(xmlXPathNewContext(xml_doc.get()));
        if (!xpath_ctx)
        {
            throw exceptions::xml_could_not_create_xpath_context_exception();
        }

        // Here we go to string as the string_view handles null termination differently. The tiny overhead is not a
        // concern!
        // ReSharper disable CppVariableCanBeMadeConstexpr
        const std::string s4778_prefix(nmbs::s4778_prefix);
        const std::string s4778_namespace(nmbs::s4778_namespace);
        const std::string s4774_prefix(nmbs::s4774_prefix);
        const std::string s4774_namespace(nmbs::s4774_namespace);
        // ReSharper restore CppVariableCanBeMadeConstexpr

        xmlXPathRegisterNs(xpath_ctx.get(), reinterpret_cast<const xmlChar*>(s4778_prefix.c_str()), reinterpret_cast<const xmlChar*>(s4778_namespace.c_str()));
        xmlXPathRegisterNs(xpath_ctx.get(), reinterpret_cast<const xmlChar*>(s4774_prefix.c_str()), reinterpret_cast<const xmlChar*>(s4774_namespace.c_str()));

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
                    current_label.label_type = confidentiality_label::metadata;
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
                    throw exceptions::xml_could_not_parse_exception("Could not parse ConfidentialityInformation element. No PolicyIdentifier, although it is a mandatory element.");
                }
                current_label.confidentiality_information.policy_identifier = node_value.value();


                node_value = get_relative_xpath_node_value(
                    binding_node, xpath_ctx.get(), ".//s4774:Classification"
                );
                if (!node_value)
                {
                    throw exceptions::xml_could_not_parse_exception("Could not parse ConfidentialityInformation element. No Classification, although it is a mandatory element.");
                }
                current_label.confidentiality_information.classification = node_value.value();


                node_value = get_relative_xpath_node_value(
                    binding_node, xpath_ctx.get(), ".//s4774:CreationDateTime"
                );
                if (!node_value)
                {
                    throw exceptions::xml_could_not_parse_exception("Could not parse *ConfidentialityLabel element. No CreationDateTime, although it is a mandatory element.");
                }

                std::istringstream in_stream{node_value.value()};
                const std::string in_format{"%FT%T%Ez"};
                in_stream >> std::chrono::parse(in_format, current_label.creation_date_time);

                //current_label.data_ref_uri = get_relative_xpath(
                //  binding_node, xpath_ctx.get(), ".//s4778:DataReference/@URI"
                //);

                results.push_back(current_label);
            }

            return results;
        }
        return results;
    }

}
