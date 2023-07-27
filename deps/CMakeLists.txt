cmake_minimum_required(VERSION 3.12)
project(BinGCode_Deps)

include(${PROJECT_SOURCE_DIR}/../cmake/modules/AddCMakeProject.cmake)

include(heatshrink/heatshrink.cmake)
include(Catch2/Catch2.cmake)