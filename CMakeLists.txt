cmake_minimum_required(VERSION 3.12)
project(LibBGCode VERSION 0.1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(_libname ${PROJECT_NAME})
string(TOLOWER ${_libname} _libname)
string(REPLACE "lib" "" _libname ${_libname})

option(${PROJECT_NAME}_BUILD_TESTS "Build unit tests" ON)

set(heatshrink_VERSION 0.4)
find_package(heatshrink ${heatshrink_VERSION} REQUIRED)

set(Core_DOWNSTREAM_DEPS "")
if (NOT BUILD_SHARED_LIBS)
    list(APPEND Core_DOWNSTREAM_DEPS "heatshrink_${heatshrink_VERSION}")
    # append all the libs that are required privately for Core
endif ()

# Core component
add_library(${_libname}_core
    src/core/core.cpp
    src/core/core.hpp
    include/libbgcode.hpp
    # Add more source files here if needed
)

target_include_directories(${_libname}_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${_libname}_core PRIVATE heatshrink::heatshrink)

# Base component (rename if needed)
add_library(${_libname}_base
    src/base/base.cpp
    src/base/base.hpp
    include/libbgcode.hpp
    # Add more source files here if needed
)

target_include_directories(${_libname}_base
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${_libname}_base PRIVATE ${_libname}_core)

set(Base_DOWNSTREAM_DEPS "")
if (NOT BUILD_SHARED_LIBS)
    # list(APPEND CORE_DOWNSTREAM_DEPS "heatshrink_${heatshrink_VERSION}")
    # append all the libs that are required privately for Core
endif ()

# Set the version of the project
# target_sources(Core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/version.h)
# target_compile_definitions(Core PRIVATE VERSION_MAJOR=${PROJECT_VERSION_MAJOR} VERSION_MINOR=${PROJECT_VERSION_MINOR})

add_library(${_libname} INTERFACE)
target_link_libraries(${_libname} INTERFACE ${_libname}_core ${_libname}_base)

if(${PROJECT_NAME}_BUILD_TESTS)
    enable_testing ()
    add_subdirectory(tests)
endif()

# Create an export header
include(GenerateExportHeader)
generate_export_header(${_libname}_core
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/core/export.h
)

generate_export_header(${_libname}_base
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/base/export.h
)

# Install targets and headers
install(TARGETS ${_libname}_core
    EXPORT ${PROJECT_NAME}CoreTargets
    INCLUDES DESTINATION include/${PROJECT_NAME}
)

install(TARGETS ${_libname}_base ${_libname}
    EXPORT ${PROJECT_NAME}BaseTargets
    INCLUDES DESTINATION include/${PROJECT_NAME}
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
install(FILES src/core/core.hpp src/base/base.hpp DESTINATION include/)
install(FILES
 ${PROJECT_BINARY_DIR}/core/export.h DESTINATION include/${PROJECT_NAME}/core/
)
install(FILES
 ${PROJECT_BINARY_DIR}/base/export.h DESTINATION include/${PROJECT_NAME}/base/
)

# Create and install the CMake config script
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(version_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
set(namespace "${PROJECT_NAME}::")

write_basic_package_version_file(
    "${version_config}"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "cmake/Config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}"
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${CONFIG_INSTALL_DIR}"
)

install(
    EXPORT "${PROJECT_NAME}CoreTargets"
    NAMESPACE "${namespace}"
    DESTINATION "${CONFIG_INSTALL_DIR}"
)

install(
    EXPORT "${PROJECT_NAME}BaseTargets"
    NAMESPACE "${namespace}"
    DESTINATION "${CONFIG_INSTALL_DIR}"
)
