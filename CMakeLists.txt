cmake_minimum_required(VERSION 3.12)
project(LibBGCode VERSION 0.1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(_libname ${PROJECT_NAME})
string(TOLOWER ${_libname} _libname)
string(REPLACE "lib" "" _libname ${_libname})

option(${PROJECT_NAME}_BUILD_TESTS "Build unit tests" ON)

option(${PROJECT_NAME}_BUILD_COMPONENT_Base "Include Base component in the library" ON)

if (NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "d")
endif ()

include(CMakeDependentOption)
cmake_dependent_option(${PROJECT_NAME}_BUILD_COMPONENT_Convert "Include Convert component in the library" ON 
                       "${PROJECT_NAME}_BUILD_COMPONENT_Base" OFF)

set(_selected_components "Core")
set(_selected_libs "${_libname}_core")

set(Core_DOWNSTREAM_DEPS "")
set(Base_DOWNSTREAM_DEPS "")
set(Convert_DOWNSTREAM_DEPS "")
if (${PROJECT_NAME}_BUILD_COMPONENT_Base)
    set(heatshrink_VERSION 0.4)
    set(ZLIB_VERSION 1.0)
    find_package(heatshrink ${heatshrink_VERSION} REQUIRED)
    find_package(ZLIB ${ZLIB_VERSION} REQUIRED)
    
    if (NOT BUILD_SHARED_LIBS)
        list(APPEND Base_DOWNSTREAM_DEPS "heatshrink_${heatshrink_VERSION}")
        list(APPEND Base_DOWNSTREAM_DEPS "ZLIB_${ZLIB_VERSION}")
        # append all the libs that are required privately for Core
    endif ()
endif()

if (${PROJECT_NAME}_BUILD_COMPONENT_Convert)
    set(Boost_VERSION 1.78)
    find_package(Boost ${Boost_VERSION} REQUIRED)
    if (NOT BUILD_SHARED_LIBS)
        list(APPEND Convert_DOWNSTREAM_DEPS "Boost_${Boost_VERSION}")
        # append all the libs that are required privately for Core
    endif ()
endif ()

# Create an export header
include(GenerateExportHeader)

# Core component
add_library(${_libname}_core
    src/core/core.cpp
    src/core/core.hpp
    # Add more source files here if needed
)

generate_export_header(${_libname}_core
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/core/export.h
)

target_include_directories(${_libname}_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
)

if (${PROJECT_NAME}_BUILD_COMPONENT_Base)
    # Base component
    add_library(${_libname}_base
        src/base/base.cpp
        src/base/base.hpp
        # Add more source files here if needed
    )

    generate_export_header(${_libname}_base
        EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/base/export.h
    )

    target_include_directories(${_libname}_base
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    target_link_libraries(${_libname}_base PRIVATE heatshrink::heatshrink_dynalloc ZLIB::ZLIB)
    target_link_libraries(${_libname}_base PUBLIC ${_libname}_core)

    list(APPEND _selected_components Base)
    list(APPEND _selected_libs ${_libname}_base)
endif ()

if (${PROJECT_NAME}_BUILD_COMPONENT_Convert)
    # Convert component
    add_library(${_libname}_convert
        src/convert/convert.cpp
        src/convert/convert.hpp
        # Add more source files here if needed
    )

    generate_export_header(${_libname}_convert
        EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/convert/export.h
    )

    target_include_directories(${_libname}_convert
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            $<INSTALL_INTERFACE:include>
    )

    target_link_libraries(${_libname}_convert PUBLIC  ${_libname}_core)
    target_link_libraries(${_libname}_convert PRIVATE ${_libname}_base Boost::boost)

    list(APPEND _selected_components Convert)
    list(APPEND _selected_libs ${_libname}_convert)
endif ()

# Set the version of the project
# target_sources(Core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/version.h)
# target_compile_definitions(Core PRIVATE VERSION_MAJOR=${PROJECT_VERSION_MAJOR} VERSION_MINOR=${PROJECT_VERSION_MINOR})

add_library(${_libname} INTERFACE)
target_link_libraries(${_libname} INTERFACE ${_selected_libs})

if(${PROJECT_NAME}_BUILD_TESTS)
    enable_testing ()
    add_subdirectory(tests)
endif()

# Create and install the CMake config script
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(version_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
set(namespace "${PROJECT_NAME}::")

list(GET _selected_components -1 _highest_comp)
foreach(_comp ${_selected_components})
    # Install targets and headers
    string(TOLOWER ${_comp} _comp_lower)
    set(_export_targets ${_libname}_${_comp_lower})
    if (_comp STREQUAL _highest_comp)
        list(APPEND _export_targets ${_libname})
    endif ()

    install(TARGETS ${_export_targets}
        EXPORT ${PROJECT_NAME}${_comp}Targets
        INCLUDES DESTINATION include/${PROJECT_NAME}
    )

    install(FILES src/${_comp_lower}/${_comp_lower}.hpp DESTINATION include/${PROJECT_NAME}/${_comp_lower})
    install(FILES
        ${PROJECT_BINARY_DIR}/${_comp_lower}/export.h DESTINATION include/${PROJECT_NAME}/${_comp_lower}/
    )
    install(
        EXPORT "${PROJECT_NAME}${_comp}Targets"
        NAMESPACE "${namespace}"
        DESTINATION "${CONFIG_INSTALL_DIR}"
    )
endforeach()

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
