//
// Created by luke on 6/7/26.
//
#include <nmbs/nmbs.h>
#include <iostream>

int main(int argc, char* argv[]) {

    if (argc != 1)
    {
        std::cerr << "This program requires two arguments, some other number was supplied. Please submit a Policy Identifier and a Classification." << std::endl;
        return 1;
    }

    const std::string tag = nmbs::nmbs_confidentiality_label("TT", "Dummy");
    const std::string canonicalizedTag = nmbs::canonicalize(tag);
    const std::string rdpTag = nmbs::rdp(canonicalizedTag);
    nmbs::write_xmp("test.jpg", rdpTag);

    return 0;
}