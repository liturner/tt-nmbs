# Getting Started

The library contains the root namespace ```nmbs```.

# Examples

```cpp
#include <nmbs/nmbs.h>
```
```cpp
std::filesystem::path file{"my-image.jpg"};

nmbs::confidentiality_label label_to_write;
label_to_write.confidentiality_information.policy_identifier{"PUBLIC"};
label_to_write.confidentiality_information.classification{"UNMARKED"};

try
{
    // The returned string contains the XML label which was written to the file.
    std::string output = nmbs::write_xmp(file, label_to_write);
    
    // Note that single files can contain multiple labels (e.g. NATO / ACME / PUBLIC)
    std::vector<nmbs::confidentiality_label> labels_read = nmbs::read_xmp(file)
}
catch (const nmbs::exception& e)
{
    // The nmbs::exception contains an nmbs::exit_code for communicating failures via API / CLI
    std::cerr << e.what() << std::endl;
    return e.code();
}
```
