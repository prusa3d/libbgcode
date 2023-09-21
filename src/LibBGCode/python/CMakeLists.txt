find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# set(WHEEL_DEPLOY_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

# include(python-wheel)

pybind11_add_module(pybgcode pybgcode.cpp)

target_link_libraries(pybgcode PRIVATE bgcode_convert)

# Hide all symbols by default (including external libraries on Linux)
# set_target_properties(pybgcode PROPERTIES
#     CXX_VISIBILITY_PRESET "hidden"
#     VISIBILITY_INLINES_HIDDEN true)

# if (CMAKE_SYSTEM_NAME MATCHES "Linux")
#     target_link_options(pybgcode PRIVATE "LINKER:--exclude-libs,ALL")
# endif()

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

# # Adapt using the following parameters:
# # NAME
# #  Wheel name. Defaults to extension module cmake target name.
# # VERSION
# #  Python package version.
# # AUTHOR
# #  Package author name.
# # URL
# #  Package website.
# # PYTHON_REQUIRES
# #  Python version requirement. Default: >=3.8
# # DESCRIPTION
# #  Python package short description.
# # DEPLOY_FILES
# #  Additional files that should be packaged with your wheel.
# # TARGET_DEPENDENCIES
# #  CMake targets which belong into the same wheel.
# # MODULE_DEPENDENCIES
# #  Python module dependencies (requirements.txt content)
# # SUBMODULES
# #  Any pybind11 submodules must be listed here to support imports like 
# #  "from mod.sub import x". A nested submodule must be listed like
# #  "sub.subsub". Parent submodules must be listed explicitly.
# add_wheel(pybgcode
#   NAME pybgcode
#   VERSION "0.1"
#   AUTHOR "Tomas Meszaros"
# #   URL "http://python.org"
#   PYTHON_REQUIRES ">=3.8"
#   DESCRIPTION "Prusa Block & Binary G-code reader / writer / converter"
#   )