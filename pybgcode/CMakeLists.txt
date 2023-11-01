cmake_minimum_required(VERSION 3.12)

include(../cmake/ProjectVersion.cmake)

project(PyBGCode VERSION ${LibBGCode_VERSION})

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PyBGCode_LINK_SYSTEM_LIBBGCODE "Link libbgcode installed in the system" ON)

# Default values for libbgcode subproject
option(LibBGCode_BUILD_CMD_TOOL "" OFF)
option(LibBGCode_BUILD_TESTS  "" OFF)
option(LibBGCode_BUILD_DEPS "" ON)
set(LibBGCode_DEPS_PRESET "python-module" CACHE STRING "" )

if (PyBGCode_LINK_SYSTEM_LIBBGCODE)
    find_package(LibBGCode REQUIRED COMPONENTS Convert)
else ()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/upstream)
endif ()

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

set(PY_VERSION_SUFFIX "")
set(PY_FULL_VERSION ${PROJECT_VERSION}${PY_VERSION_SUFFIX})

# Make sure that the Python and CMake versions match
if (DEFINED PY_BUILD_CMAKE_PACKAGE_VERSION)
    if (NOT "${PY_BUILD_CMAKE_PACKAGE_VERSION}" MATCHES "^${PY_FULL_VERSION}$")
        message(FATAL_ERROR "Version number does not match "
                             "(${PY_BUILD_CMAKE_PACKAGE_VERSION} - ${PY_FULL_VERSION}).")
    endif()
endif()

find_package(Boost ${Boost_VER} REQUIRED COMPONENTS nowide)

pybind11_add_module(pybgcode MODULE pybgcode.cpp)

target_link_libraries(pybgcode PRIVATE LibBGCode::bgcode_convert Boost::nowide)

target_compile_definitions(pybgcode PRIVATE
    MODULE_NAME=pybgcode
    VERSION_INFO="${PY_FULL_VERSION}"
)

# Hide all symbols by default (including external libraries on Linux)
set_target_properties(pybgcode PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN true)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_options(pybgcode PRIVATE "LINKER:--exclude-libs,ALL")
endif()

if (PY_BUILD_CMAKE_MODULE_NAME)
    # Install the Python module
    install(TARGETS pybgcode
            EXCLUDE_FROM_ALL
            COMPONENT python_modules
            DESTINATION ${PY_BUILD_CMAKE_MODULE_NAME})
    # Install the debug file for the Python module (Windows only)
    if (WIN32)
        install(FILES $<TARGET_PDB_FILE:pybgcode>
                EXCLUDE_FROM_ALL
                COMPONENT python_modules
                DESTINATION ${PY_BUILD_CMAKE_MODULE_NAME}
                OPTIONAL)
    endif()
endif ()

enable_testing()

set (TEST_DATA_DIR ${CMAKE_CURRENT_LIST_DIR}/../tests/data)

# Prepare the test files to be available
configure_file(${TEST_DATA_DIR}/mini_cube_b_ref.gcode ${CMAKE_CURRENT_BINARY_DIR}/test.gcode)

# Define the test for ctest and inject the correct PYTHONPATH env var for its runtime
add_test(NAME test_pybgcode COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_convert.py CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES})
set_property(TEST test_pybgcode PROPERTY ENVIRONMENT PYTHONPATH=$<TARGET_FILE_DIR:pybgcode>)

add_custom_target(build_and_test COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS pybgcode)