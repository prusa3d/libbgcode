include(ExternalProject)

ExternalProject_Add(dep_python_cmake_wheel
    URL https://github.com/Klebert-Engineering/python-cmake-wheel/archive/refs/tags/v0.9.0.zip

    DOWNLOAD_DIR        ${DEP_DOWNLOAD_DIR}/${projectname}

    CONFIGURE_COMMAND
    BUILD_COMMAND
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy *.* ${DESTDIR}/usr/local/share/cmake/python_cmake_wheel
)