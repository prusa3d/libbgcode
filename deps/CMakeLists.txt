cmake_minimum_required(VERSION 3.12)
project(LibBGCode_Deps)

include(${PROJECT_SOURCE_DIR}/../cmake/modules/AddCMakeProject.cmake)

set(supported_packages Boost heatshrink Catch2 ZLIB)

include(CMakeDependentOption)
option(${PROJECT_NAME}_SELECT_ALL "Choose all external projects to be built." ON)

find_package(Git REQUIRED)

# The default command line for patching. Only works for newer 
set(PATCH_CMD ${GIT_EXECUTABLE} apply --verbose --ignore-space-change --whitespace=fix)

foreach (pkg ${supported_packages})
    cmake_dependent_option(${PROJECT_NAME}_SELECT_${pkg} "Select package ${pkg} to be built." OFF  "NOT ${PROJECT_NAME}_SELECT_ALL" OFF)
    if (${PROJECT_NAME}_SELECT_ALL OR ${PROJECT_NAME}_SELECT_${pkg})
        include(${pkg}/${pkg}.cmake)
    endif ()
endforeach()

install(CODE "message(STATUS \"Built packages succesfully.\")")