//
// Created by luke on 6/7/26.
//
#include <nmbs/nmbs.h>
#include <argparse/argparse.hpp>
#include <iostream>
#include <filesystem>

int main(const int argc, char* argv[]) {

    // Argument Parsing
    argparse::ArgumentParser program("nmbs-xmp", nmbs::version());
    program.add_description("sets the classification tag on XMP compatible file types following the NATO ADatP-4774, ADatP-4778 and ADatP-5636 standards. This is a minimal tagging tool, and is only intended to aid in simple tagging. Further refinement, such as the addition of exemptions or markings is outside the scope of this tool. The use case of this tool is rather to lower the cost of tagging uninteresting files in complex systems (e.g. this tool may be used by software to tag software icons as UNCLASSIFIED, rather than paying thousands for full tagging tools...)");
    program.add_argument("file").help("the file to apply the classification label to");
    program.add_argument("policy").help("the policy identifier (e.g. NATO, ITAR)");
    program.add_argument("classification").help(R"(the policy classification (e.g. UNCLASSIFIED, RESTRICTED, CONFIDENTIAL, SECRET, "TOP SECRET" (not the need for parenthesis with spaces!))");
    program.add_argument("-v", "--verbose").help("print the label written to the file on stdout").flag();

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return nmbs::exit_code::invalid_arguments;
    }

    // Argument Validation
    nmbs::confidentiality_label label;
    const std::filesystem::path file = program.get<std::string>("file");
    label.confidentiality_information.policy_identifier = program.get<std::string>("policy");
    label.confidentiality_information.classification = program.get<std::string>("classification");

    if (!(std::filesystem::exists(file) && std::filesystem::is_regular_file(file))) {
        std::cerr << "file not found" << std::endl;
        return nmbs::exit_code::file_not_found;
    }

    // Program Logic
    try
    {
        std::vector<nmbs::confidentiality_label> xmp(1);
        xmp.push_back(label);
        const std::string output = nmbs::write_labels(file,  xmp);
        if (program["--verbose"] == true) {
            std::cout << "  Key: Xmp." << nmbs::constants::s4778_xmp_prefix << "." << nmbs::constants::s4778_key << std::endl
            << "Value: " << output << std::endl;
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

    return nmbs::exit_code::success;
}