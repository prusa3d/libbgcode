cmake_minimum_required(VERSION 3.10)

project(heatshrink C)

add_library(${PROJECT_NAME} heatshrink.c heatshrink_decoder.c heatshrink_encoder.c)

find_library(MATH_LIBRARY m) # Business as usual
if(MATH_LIBRARY)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${MATH_LIBRARY})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC $<INSTALL_INTERFACE:include>)

# Installation and export:

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION 1.0
    COMPATIBILITY AnyNewerVersion
)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
)

export(EXPORT ${PROJECT_NAME}Targets 
       FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake" 
       NAMESPACE ${PROJECT_NAME}::
)

include(GNUInstallDirs)
set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${ConfigPackageLocation}
)

install(
    FILES
      heatshrink_common.h
      heatshrink_config.h
      heatshrink_encoder.h
      heatshrink_decoder.h
    DESTINATION
      include/${PROJECT_NAME}
  )

install(EXPORT ${PROJECT_NAME}Targets
  FILE
    ${PROJECT_NAME}Targets.cmake
  NAMESPACE
    ${PROJECT_NAME}::
  DESTINATION
    ${ConfigPackageLocation}
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
)