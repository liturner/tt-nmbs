//
// Created by luke on 6/6/26.
//
#include <nmbs/nmbs.h>

#include <exiv2/exiv2.hpp>
#include <chrono>
#include <iostream>

namespace nmbs
{
    std::string nmbs_confidentiality_label(const std::string& policyIdentifier, const std::string& classification)
    {
        const auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::utc_clock::now());
        const std::string nowString = std::format("{:%FT%TZ}", now);

        std::string xml;
        xml.reserve(700);
        xml.append("<slab:originatorConfidentialityLabel xmlns:slab=\"urn:nato:stanag:4774:confidentialitymetadatalabel:1:0\">");
        xml.append("<slab:ConfidentialityInformation>");
        xml.append("<slab:PolicyIdentifier>");
        xml.append(policyIdentifier);
        xml.append("</slab:PolicyIdentifier>");
        xml.append("<slab:Classification>");
        xml.append(classification);
        xml.append("</slab:Classification>");
        xml.append("</slab:ConfidentialityInformation>");
        xml.append("<slab:CreationDateTime>");
        xml.append(nowString);
        xml.append("</slab:CreationDateTime>");
        xml.append("</slab:originatorConfidentialityLabel>");
        return xml;
    }

    std::string canonicalize(const std::string& text)
    {
        std::string out;
        out.reserve(text.size() * 2); // avoids repeated reallocations

        for (char c : text) {
            switch (c) {
            case '<':  out += "&lt;";  break;
            case '>':  out += "&gt;";  break;
            case '&':  out += "&amp;"; break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&apos;"; break;
            default:   out += c; break;
            }
        }

        return out;
    }

    std::string rdp(const std::string& payload)
    {
        std::string xml;
        xml.reserve(payload.size() + 300);
        xml.append("<rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" xmlns:mbxmp=\"urn:nato:stanag:4778:bindinginformation:1:0:xmp#\">");
        xml.append("<rdf:Description rdf:about=\"\">");
        xml.append("<mbxmp:bindingInformation>");
        xml.append(payload);
        xml.append("</mbxmp:bindingInformation>");
        xml.append("</rdf:Description>");
        xml.append("</rdf:RDF>");
        return xml;
    }

    int write_xmp(const std::filesystem::path& path, const std::string& payload)
    {
        try
        {
            auto image = Exiv2::ImageFactory::open(path.string());
            if (image.get() == nullptr) return 1;

            image->readMetadata();
            Exiv2::XmpData& xmpData = image->xmpData();
            xmpData["bindingInformation"] = payload;
            image->writeMetadata();
        }
        catch (const Exiv2::Error& e) {
            std::cerr << "Exiv2 Exception: " << e.what() << "\n";
            return 1;
        }
        catch (const std::exception& e) {
            std::cerr << "Standard Exception: " << e.what() << "\n";
            return 1;
        }

        return 0;
    }
}
