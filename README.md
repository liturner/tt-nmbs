# Normalised Metadata Binding Service (tt-nmbs)

This project provides a set of tools for use with the NATO Confidentiality Metadata Labels as specified in ADatP-4774 
and ADatP-4778.

The project is aimed as a Debian Linux project only. The intention is to produce a set of commands which can be run 
directly on a system. A Library containing the core functionality will also be produced, enabling other tools to 
integrate much of the core functionality with ease.

# Usage

Install the package to your system using apt.

```shell
# For users
apt install nmbs

# For developers
apt install libnmbs-dev
```

The documentation for the CLI is packaged with the tool itself. Doxygen is otherwise available for the code in the 
libnmbs-dev package.

```shell
# For the CLI, use the supplied man page, or the --help argument
man nmbs-get
man nmbs-set
man nmbs-verify

# For development documentation, the doxygen can be accessed using a doc-base tool or by e.g.
xdg-open /usr/share/doc/libnmbs-dev/html/index.html
```

# Building

There are a few methods for building this project. The primary focus is ensuring that the debian package is clean and
stable. This is done as follows (assuming current dir is the project root).

```shell
dpkg-buildpackage
lintian ../tt-nmbs*.changes

# To update the changelog use dch. In particular, the commands --append, --increment, --edit, --release and --newversion
# Note that debhelper decides e.g. if to sign the build based on the status in the changelog. Its an important file!!!
dch -m

# Finalise the current version in the log
dch -mr

# Verify using lintian. mentors will use pedantic settings!
lintian --info --pedantic --display-info nmbs_*_amd64.changes

# After finalisation, increment the build version automatically and start the next section
dch -mi

# To tidy up after, use
dh clean
```

Finally, this is a CLion project. The project settings include a "dpkg" target, and just using the GUI will work for
the CMake Targets.

# Standards

- ADatP-4774 (Ed.A V.1 12.2017) CONFIDENTIALITY METADATA LABEL SYNTAX
- ADatP-4778 (Ed.A V.1 10.2018) METADATA BINDING MECHANISM
- ADatP-5636 (Ed.A V.1 11.2022) NATO CORE METADATA SPECIFICATION(NCMS)

# Dependencies

Dependencies are kept to a minimum. Particular focus is paid to ensuring Licenses are all very flexible, APT presence of
packages, and trustworthiness. Ideally, this project will be developed on a Debian distro, with simple calls to install
the libs via APT on a Developer Machine, and APT Dependencies in release.

| Name     | Type        | APT                       | Description                                                            |
|----------|-------------|---------------------------|------------------------------------------------------------------------|
| Exiv2    | Product     | libexiv2-dev              | C++ Lib for writing metadata to numerous formats, in particular images |
| argparse | Product     | libargparse-dev           | C++17 Headers for parsing CLI options                                  |
|          | Product     | libxml2                   | C XML library. Most "native" and maintained lib possible in Debian     |
|          | Build       | libxml2-dev               |                                                                        |
|          | Build       | build-essential           |                                                                        |
|          | Build       | cmake                     |                                                                        |
|          | Build       | libgtest-dev              |                                                                        |
|          | Build       | doxygen                   |                                                                        |
|          | Build       | ninja-build               |                                                                        |
|          | Build       | help2man                  |                                                                        |
|          | Build       | debhelper                 | Toolset for building .deb files                                        |
|          | Build       | pkg-config                | NMBS-Nautilus Toolset for building GNOME projects                      |
|          | Product     | libglib2.0-dev            | NMBS-Nautilus                                                          |
|          | Product     | libnautilus-extension-dev | NMBS-Nautilus                                                          |
|          | Development | lintian                   | Tool for verifying quality of .deb packages                            |
|          | Development | devscripts                | Scripts containing dch used for the debian changelog                   |
|          | Development | libxml2-doc               |                                                                        |
|          | Development | valgrind                  | Runs in CLion directly to check memory leaks                           |
|          | Development | git                       |                                                                        |
|          | Recomended  | exiv2                     | CLI tools for Exiv2. Usefull for debugging                             |

## External Resources

This project ships with a small number of external resources. Many of these were shipped with the NATO Standards. 
Important to note is that only PUBLIC UNMARKED resources have been included. Unfortunately several XSD resources are
still marked NATO UNCLASSIFIED. It will take some time to write "clean room" implementations from the standards to ship.

Their original sources are as follows.

| File                                       | Source                                                                                     | Description                                                                                                            |
|--------------------------------------------|--------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------|
| nl-cl.xsd                                  | [ADatP-4774.5 EDA V1.zip](https://nso.nato.int/nso/nsdd/main/standards/srd-details/245/EN) | XSD for ConfidentialityLabel                                                                                           |
| nl-mb.xsd                                  | [ADatP-4774.5 EDA V1.zip](https://nso.nato.int/nso/nsdd/main/standards/srd-details/245/EN) | XSD for BindingInformation                                                                                             |
| xmlspif.xsd                                | [xmlspif.org](http://www.xmlspif.org/schema/xmlspif.xsd)                                   | XSD for the "Security Policy Information File" format used to describe available classifications.                      |
| 20140916_PU_PUBLIC Security Policy-v1.spif | [ADatP-4774.5 EDA V1.zip](https://nso.nato.int/nso/nsdd/main/standards/srd-details/245/EN) | This contains a list of the standard classifications used. It is not normative, but till now is the best I have found. |
| 20210506_PU_NATO Security Policy-v88.spif  | [ADatP-4774.5 EDA V1.zip](https://nso.nato.int/nso/nsdd/main/standards/srd-details/245/EN) | This contains a list of the standard classifications used. It is not normative, but till now is the best I have found. |
