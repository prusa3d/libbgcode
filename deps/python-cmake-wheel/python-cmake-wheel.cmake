include(ExternalProject)

set(_exclude_from_all OFF)

if (NOT ${PROJECT_NAME}_SELECT_pybind11)
  set(_exclude_from_all ON)
endif ()

ExternalProject_Add(dep_python_cmake_wheel
    EXCLUDE_FROM_ALL ${_exclude_from_all}
    
    URL https://github.com/Klebert-Engineering/python-cmake-wheel/archive/refs/tags/v0.9.0.zip

    DOWNLOAD_DIR        ${DEP_DOWNLOAD_DIR}/${projectname}
    BUILD_IN_SOURCE ON
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory .  ${DESTDIR}/usr/local/share/cmake/python_cmake_wheel
)