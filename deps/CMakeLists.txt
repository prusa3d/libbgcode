cmake_minimum_required(VERSION 3.12)
project(BinGCode_Deps)

include(${PROJECT_SOURCE_DIR}/../cmake/modules/AddCMakeProject.cmake)

set(DEP_Boost_COMPONENTS "headers")
include(Boost/Boost.cmake)
include(heatshrink/heatshrink.cmake)
include(Catch2/Catch2.cmake)
if (NOT UNIX) # Unix systems offer zlib out of the box
    include(ZLIB/ZLIB.cmake)
endif ()