include("build/debug/CPackConfig.cmake")
include("build/release/CPackConfig.cmake")

set(CPACK_INSTALL_CMAKE_PROJECTS
        "build/debug;TT-NMBS;ALL;/"
        "build/release;TT-NMBS;ALL;/"
)