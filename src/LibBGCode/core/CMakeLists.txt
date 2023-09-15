set(Core_DOWNSTREAM_DEPS "")

# Core component
add_library(${_libname}_core
   core.cpp
   core.hpp
   # Add more source files here if needed
)

generate_export_header(${_libname}_core
   EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/core/export.h
)

target_include_directories(${_libname}_core
   PUBLIC
       $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/${_srcloc}>
       $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
       $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
       $<INSTALL_INTERFACE:include>
)

add_library(${namespace}${_libname}_core ALIAS ${_libname}_core)

# Set the version of the project
# target_sources(Core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/version.h)
# target_compile_definitions(Core PRIVATE VERSION_MAJOR=${PROJECT_VERSION_MAJOR} VERSION_MINOR=${PROJECT_VERSION_MINOR})

set(Core_DOWNSTREAM_DEPS ${Core_DOWNSTREAM_DEPS} PARENT_SCOPE)
