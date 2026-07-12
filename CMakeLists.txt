cmake_minimum_required(VERSION 3.23)
project(
        NMBS
        LANGUAGES C CXX
        VERSION 1.0.0
)

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_VENDOR "TurnerTech")
set(CPACK_PACKAGE_CONTACT "turnertech@outlook.de")

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CPack)
include(GoogleTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(cmake/manpages.cmake)
include(CheckCXXCompilerFlag)

add_compile_options(
        -fstack-protector-strong
        -D_FORTIFY_SOURCE=2
)

add_link_options(
        "-Wl,-z,relro"
        "-Wl,-z,now"
)

# Versions are between Ubuntu 24.04 (Github agent) and Debian 13 (target system)
find_package(exiv2 REQUIRED CONFIG)                               # libexiv2-dev (Debian 13: 0.28.5, Ubuntu 24.04: 0.27.6)
find_package(argparse "3.0.0...3.2.0" REQUIRED CONFIG)            # libargparse-dev
find_package(GTest "1.14.0...1.17.0" REQUIRED CONFIG)             # libgtest-dev (Ubuntu 24.04: 1.14.0 Debian 13: 1.15.0 Debian 14: 1.17.0)
find_package(Doxygen "1.9.8" OPTIONAL_COMPONENTS dot mscgen dia)  # doxygen
find_package(LibXml2 "2.9.14" REQUIRED)                           # libxml2-dev
find_package(PkgConfig REQUIRED)                                  # pkg-config

pkg_check_modules(GLIB REQUIRED glib-2.0)                         # libglib2.0-dev
pkg_check_modules(NAUTILUS REQUIRED libnautilus-extension-4)      # libnautilus-extension-dev

enable_testing()

add_subdirectory(targets)