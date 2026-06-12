//
// Created by luke on 6/7/26.
//
#include <nmbs/nmbs.h>
#include <argparse/argparse.hpp>
#include <iostream>
#include <filesystem>

int main(const int argc, char* argv[]) {

    // Argument Parsing
    argparse::ArgumentParser program("nmbs-get-xmp", nmbs::version());
    program.add_description("gets the classification tag on XMP compatible file types following the NATO ADatP-4774, ADatP-4778 and ADatP-5636 standards. This is a minimal tagging tool, and is only intended to aid in simple tagging. Further refinement, such as the addition of exemptions or markings is outside the scope of this tool. The use case of this tool is rather to lower the cost of tagging uninteresting files in complex systems (e.g. this tool may be used by software to tag software icons as UNCLASSIFIED, rather than paying thousands for full tagging tools...)");
    program.add_argument("file").help("the file to retrieve the classification labels from");
    program.add_argument("-r", "--raw").help("print the raw XML label. This will be output as a single line and is intended to be ingested by other applications").flag();

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return nmbs::exit_code::invalid_arguments;
    }

    // Argument Validation
    const std::filesystem::path file = program.get<std::string>("file");
    if (!(std::filesystem::exists(file) && std::filesystem::is_regular_file(file))) {
        std::cerr << "file not found" << std::endl;
        return nmbs::exit_code::file_not_found;
    }

    // Program Logic
    try
    {
        if (program["--raw"] == true) {
            if (const auto raw_xml = nmbs::read_xmp_xml(file); raw_xml.has_value())
            {
                std::cout << raw_xml.value() << std::endl;
                return nmbs::exit_code::success;
            }
            std::cerr << "no label was present on the provided file" << std::endl;
            return nmbs::exit_code::no_label_present;
        } else
        {
            for (const auto confidentiality_labels = nmbs::read_xmp(file); const auto& label : confidentiality_labels)
            {
                std::cout << label.confidentiality_information.policy_identifier << " " << label.confidentiality_information.classification << std::endl;
                return nmbs::exit_code::success;
            }
        }

    }
    catch (const nmbs::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return e.code();
    }
    catch (const std::exception& e)
    {
        std::cerr << "std::exception: " << e.what() << std::endl;
        return nmbs::exit_code::unknown_error;
    }
}