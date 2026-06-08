//
// Created by luke on 6/6/26.
//

#ifndef TT_NMBS_NMBS_H
#define TT_NMBS_NMBS_H

#include <string>
#include <filesystem>

namespace nmbs
{
    std::string nmbs_confidentiality_label(const std::string& policyIdentifier, const std::string& classification);

    std::string canonicalize(const std::string& text);

    /// Generates the payload for an XMP tag.
    /// @param payload the canonicalized form of the confidentiality label.
    /// @return the string to be inserted into the XMP key-value pair.
    std::string rdp(const std::string& payload);

    int write_xmp(const std::filesystem::path& path, const std::string& payload);
}

#endif //TT_NMBS_NMBS_H
