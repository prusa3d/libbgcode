find_package(Python REQUIRED COMPONENTS Interpreter Development)
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

pybind11_add_module(pybgcode MODULE pybgcode.cpp)

target_link_libraries(pybgcode PRIVATE bgcode_convert)

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
