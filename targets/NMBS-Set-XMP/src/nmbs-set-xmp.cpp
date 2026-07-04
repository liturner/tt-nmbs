/// @file nmbs-set-xmp.cpp
/// @brief Basic example of using the libnmbs API
///
/// @author Luke Ian Turner
/// @date 2026-06-07
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

#include <nmbs/nmbs.h>
#include <nmbs/constants.h>
#include <argparse/argparse.hpp>
#include <iostream>
#include <filesystem>

int main(const int argc, char* argv[]) {

    // Argument Parsing
    argparse::ArgumentParser program("nmbs-xmp", std::string(nmbs::version()));
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
        std::vector<nmbs::confidentiality_label> labels;
        labels.push_back(label);
        auto output = nmbs::write_labels(file,  labels);
        if (output.has_value())
        {
            if (program["--verbose"] == true) {
                std::cout << "  Key: Xmp." << nmbs::constants::s4778_xmp_prefix << "." << nmbs::constants::s4778_key << std::endl
                << "Value: " << output.value() << std::endl;
            }
            return nmbs::exit_code::success;
        }
        std::cerr << output.error().message() << std::endl;
        return output.error().code();

    }
    catch (const std::exception& e)
    {
        std::cerr << "std::exception: " << e.what() << std::endl;
        return nmbs::exit_code::unknown_error;
    }

    return nmbs::exit_code::success;
}