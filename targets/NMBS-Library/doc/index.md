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
find_package(TT-NMBS REQUIRED CONFIG)

target_link_libraries(Your-Target
        PRIVATE
        TT-NMBS::NMBS-Library
)
```

Ensure that the core header is included in your code. This should be the only header you 
ever need for libnmbs (assuming your using C++ of course...).

```cpp
#include <nmbs/nmbs.h>
```

The following is a minimal example for writing and reading the XMP metadata on an image.
Note that the error handling for the library is using nmbs::exception, which contains
a human-readable message, and a machine-readable code. nmbs::confidentiality_label is
a simple struct reflecting the content of the XML specified in ADatP-4774.

```cpp
std::filesystem::path file{"my-image.jpg"};

nmbs::confidentiality_label label;
label.confidentiality_information.policy_identifier{"PUBLIC"};
label.confidentiality_information.classification{"UNMARKED"};

try
{
    // The returned string contains the XML which was written to the file.
    std::vector<nmbs::confidentiality_label> labels_to_write;
    labels_to_write.push_back(label);
    std::string output = nmbs::write_labels(file, labels_to_write);
    
    // Note that single files can contain multiple labels (e.g. NATO / ACME / PUBLIC)
    std::vector<nmbs::confidentiality_label> labels_read = nmbs::read_labels(file)
}
catch (const nmbs::exception& e)
{
    // The nmbs::exception contains an nmbs::exit_code for communicating failures via API / CLI
    std::cerr << e.what() << std::endl;
    return e.code();
}
```

If you plan to use the limited C API, look into the C header:

```cpp
#include <nmbs/nmbs_c.h>
```
