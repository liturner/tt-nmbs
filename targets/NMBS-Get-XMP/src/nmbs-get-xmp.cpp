/// @file nmbs-get-xmp.cpp
/// @brief
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
#include <argparse/argparse.hpp>
#include <iostream>
#include <filesystem>

int main(const int argc, char* argv[]) {

    // Argument Parsing
    argparse::ArgumentParser program("nmbs-get-xmp", std::string(nmbs::version()));
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
            if (const auto raw_xml = nmbs::read_labels_xml(file); raw_xml.has_value())
            {
                std::cout << raw_xml.value() << std::endl;
                return nmbs::exit_code::success;
            }
            std::cerr << "no label was present on the provided file" << std::endl;
            return nmbs::exit_code::no_label_present;
        }
        if (const auto labels = nmbs::read_labels(file); labels.has_value())
        {
            for (const auto& label : labels.value())
            {
                std::cout << label.confidentiality_information.policy_identifier << " " << label.confidentiality_information.classification << std::endl;
                return nmbs::exit_code::success;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "std::exception: " << e.what() << std::endl;
        return nmbs::exit_code::unknown_error;
    }
}