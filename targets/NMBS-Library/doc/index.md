# Getting Started

This documentation is installed locally as part of the _libnmbs-dev_ package 
under /usr/share/doc/libnmbs-dev/html/index.html. You can open this in a web browser
directly, or utilise a package such as _dochelp_ to browse for it.

The library contains the root namespace ```nmbs``` in C++, and uses the prefix ```nmbs_``` in the C API. 
The API is primarily based around [functions](namespacemembers_func.html), minimising the use of data structures to simple structs 
for holding data. The structs themselves have very little, if any, functionality.

# Examples

Once you have installed _libnmbs-dev_, you will be able to integrate libnmbs1 into your
project as follows:

```cmake
find_package(NMBS REQUIRED CONFIG)

target_link_libraries(Your-Target
        PRIVATE
        NMBS::NMBS-Library
)
```

Ensure that the core header is included in your code. This should be the only header you 
ever need for libnmbs (assuming your using C++ of course...).

```cpp
#include <nmbs/nmbs.h>
```

The following is a minimal example for writing and reading the XMP metadata on an image.
Note that the error handling for the library is using nmbs::Expected, which is simply a
std::Expected<T, nmbs::Error> typedef. The majority of errors will be reported in this
manner. The nmbs::Error contains a human-readable message, and a machine-readable code. 

```cpp
std::filesystem::path file{"my-image.jpg"};

nmbs::confidentiality_label label;
label.confidentiality_information.policy_identifier{"PUBLIC"};
label.confidentiality_information.classification{"UNMARKED"};

std::vector<nmbs::ConfidentialityLabel> labels;
labels.push_back(label);

// Write labels to a file, check for errors in the return value.
auto output = nmbs::write_labels(file,  labels);
if (!output.has_value())
{
    std::cerr << output.error().message() << std::endl;
    return output.error().code();
}

// Read labels from a file. Check has_value() before iterating.
if (const auto labels = nmbs::read_labels(file); labels.has_value())
{
    for (const auto& label : labels.value())
    {
        std::cout << label.confidentiality_information.policy_identifier << " " << label.confidentiality_information.classification << std::endl;
        return nmbs::ExitCode::success;
    }
}
```

If you plan to use the limited C API, look into the C header.:

```cpp
#include <nmbs/nmbs_c.h>
```
