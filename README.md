# TurnerTech - NATO Metadata Binding Service (tt-nmbs)

This project plans to produce a set of tools for use with NATO Confidentiality Metadata Labels. In particular the first step aims to provide some basic functionality specifically for XMP. The project is however going to be built in such a way as to allow for pretty extensive expansion. For example, we will not include the Cryptographic Binding Mechanisms yet.

The project is aimed as a Debian Linux project only. The intention is to produce (at least at first) a set of commands which can be ran directly on a system. This should ensure that there are no IT Security limitations (e.g. sending a TOP SECRET document to some web service to label it is just going to make Cyber departments sigh....). This intention will stretch through all of the design. Security by design should ensure that users feel comfortable trusting this tool.

# Standards

- ADatP-4774 (Ed.A V.1 12.2017) CONFIDENTIALITY METADATA LABEL SYNTAX
- ADatP-4778 (Ed.A V.1 10.2018) METADATA BINDING MECHANISM

# Dependencies

Dependencies are kept to a minimum. Particular focus is paid to ensuring Licenses are all very flexible, APT presence of packages, and trustworthiness. Ideally, this project will be developed on a Debian distro, with simple calls to install the libs via APT on a Developer Machine, and APT Dependencies in release.

nmbs-lib - depends exiv2 (statically linked)
nmbs-* - depends nmbs-lib

## Product

- Exiv2 - C++ Lib for writing metadata to numerous formats, in particular images (dev: libexiv2-dev (static linking))
- Boost - C++ Lib for reading and writing XML (dev: libboost-all-dev (static linking))

## Build

build-essential
libgtest-dev
doxygen
libboost-all-dev
ninja-build

## Recomended

exiv2 (CLI tools for exiv2)

## External Resources

- nl-cl.xsd [ADatP-4774.5 EDA V1.zip](https://nso.nato.int/nso/nsdd/main/standards/srd-details/245/EN)
- nl-mb.xsd [ADatP-4774.5 EDA V1.zip](https://nso.nato.int/nso/nsdd/main/standards/srd-details/245/EN)

