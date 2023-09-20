find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

set(WHEEL_DEPLOY_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

include(python-wheel)

pybind11_add_module(pybgcode pybgcode.cpp)

target_link_libraries(pybgcode PRIVATE bgcode_convert)

# Adapt using the following parameters:
# NAME
#  Wheel name. Defaults to extension module cmake target name.
# VERSION
#  Python package version.
# AUTHOR
#  Package author name.
# URL
#  Package website.
# PYTHON_REQUIRES
#  Python version requirement. Default: >=3.8
# DESCRIPTION
#  Python package short description.
# DEPLOY_FILES
#  Additional files that should be packaged with your wheel.
# TARGET_DEPENDENCIES
#  CMake targets which belong into the same wheel.
# MODULE_DEPENDENCIES
#  Python module dependencies (requirements.txt content)
# SUBMODULES
#  Any pybind11 submodules must be listed here to support imports like 
#  "from mod.sub import x". A nested submodule must be listed like
#  "sub.subsub". Parent submodules must be listed explicitly.
add_wheel(pybgcode
  NAME pybgcode
  VERSION "0.1"
  AUTHOR "Tomas Meszaros"
#   URL "http://python.org"
  PYTHON_REQUIRES ">=3.8"
  DESCRIPTION "Prusa Block & Binary G-code reader / writer / converter"
  DEPLOY_FILES "${PROJECT_SOURCE_DIR}/LICENSE")