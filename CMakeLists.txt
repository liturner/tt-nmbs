cmake_minimum_required(VERSION 3.23)
project(
        TT-NMBS
        LANGUAGES CXX
        VERSION 1.1.0
)

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_VENDOR "TurnerTech")
set(CPACK_PACKAGE_CONTACT "luke.ian.turner@outlook.de")

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "d")

include(CPack)
include(GoogleTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(exiv2 REQUIRED CONFIG)                               # libexiv2-dev (Debian 13: 0.28.5, Ubuntu 24.04: 0.27.6)
find_package(argparse "3.0.0...3.2.0" REQUIRED CONFIG)            # libargparse-dev
find_package(GTest "1.15.0" REQUIRED CONFIG)                      # libgtest-dev
find_package(Doxygen "1.9.8" OPTIONAL_COMPONENTS dot mscgen dia)  # doxygen

enable_testing()

add_subdirectory(targets)