# TurnerTech - NATO Metadata Binding Service (tt-nmbs)

This project plans to produce a set of tools for use with NATO Confidentiality Metadata Labels. In particular the first
step aims to provide some basic functionality specifically for XMP metadata in various image formats. The project is
however going to be built in such a way as to allow for expansion.

The project is aimed as a Debian Linux project only. The intention is to produce (at least at first) a set of commands
which can be run directly on a system. This should ensure that there are no IT Security limitations (e.g. sending a TOP
SECRET document to some web service to label it opens increases the security complexity).

A Library containing the core functionality will also be produced, enabling other tools to integrate much of the core
functionality with ease. For example, a GUI tool saving a screenshot can label the file before it even gets saved to
disk.

# Usage

Install the package to your system using apt.

```shell
apt install nmbs
```

The documentation for the CLI is packaged with the tool itself. Doxygen is otherwise available for the code

```shell
# For the CLI, use the supplied man page, or the --help argument
man nmbs-set-xmp
nmbs-set-xmp --help
```

# Building

There are a few methods for building this project. The primary focus is ensuring that the debian package is clean and
stable. This is done as follows (assuming current dir is the project root).

```shell
# Use the gpg fingerprint for the -k field. e.g.
dpkg-buildpackage -kB6B00085FA5CADF5EA06188D11846BA6C8BEAD9A
lintian ../tt-nmbs*.changes

# To update the changelog use dch. In particular, the commands --append, --increment, --edit, --release and --newversion
# Note that debhelper decides e.g. if to sign the build based on the status in the changelog. Its an important file!!!
dch -m

# Finalise the current version in the log
dch -mr --distribution trixie

# After finalisation, increment the build version automatically and start the next section
dch -mi

# To tidy up after, use
dh clean
```

Alternatively, the commands in the .github folder show how the CI packages the project. Note that this is a different
build chain (e.g. with ninja) but should still build. The debhelper chain takes precedence!

Finally, this is a CLion project. The project settings include a "package" target, and just using the GUI will work for
the CMake Targets.

# Standards

- ADatP-4774 (Ed.A V.1 12.2017) CONFIDENTIALITY METADATA LABEL SYNTAX
- ADatP-4778 (Ed.A V.1 10.2018) METADATA BINDING MECHANISM
- ADatP-5636 (Ed.A V.1 11.2022) NATO CORE METADATA SPECIFICATION(NCMS)

# Dependencies

Dependencies are kept to a minimum. Particular focus is paid to ensuring Licenses are all very flexible, APT presence of
packages, and trustworthiness. Ideally, this project will be developed on a Debian distro, with simple calls to install
the libs via APT on a Developer Machine, and APT Dependencies in release.

| Name     | Type         | APT             | Description                                                            |
|----------|--------------|-----------------|------------------------------------------------------------------------|
| Exiv2    | Product      | libexiv2-dev    | C++ Lib for writing metadata to numerous formats, in particular images |
| argparse | Product      | libargparse-dev | C++17 Headers for parsing CLI options                                  |
|          | Product      | libxml2         | C XML library. Most "native" and maintained lib possible in Debian     |
|          | Build        | libxml2-dev     |                                                                        |
|          | Build        | build-essential |                                                                        |
|          | Build        | libgtest-dev    |                                                                        |
|          | Build        | doxygen         |                                                                        |
|          | Build        | ninja-build     |                                                                        |
|          | Build        | help2man        |                                                                        |
|          | Build        | debhelper       | Toolset for building .deb files                                        |
|          | Development  | lintian         | Tool for verifying quality of .deb packages                            |
|          | Development  | devscripts      | Scripts containing dch used for the debian changelog                   |
|          | Development  | libxml2-doc     |                                                                        |
|          | Recomended   | exiv2           | CLI tools for Exiv2. Usefull for debugging                             |

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
