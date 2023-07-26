include(ExternalProject)

set(DESTDIR "${CMAKE_CURRENT_BINARY_DIR}/destdir" CACHE PATH "Destination directory")
set(DEP_DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "Path for downloaded source packages.")

get_property(_is_multi GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if (NOT _is_multi AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Forcing CMAKE_BUILD_TYPE to Release as it was not specified.")
endif ()

function(add_cmake_project projectname)
    cmake_parse_arguments(P_ARGS "" "INSTALL_DIR;BUILD_COMMAND;INSTALL_COMMAND" "CMAKE_ARGS" ${ARGN})

    set(_configs_line -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})
    if (_is_multi OR MSVC)
        set(_configs_line "")
    endif ()

    ExternalProject_Add(
        dep_${projectname}
        INSTALL_DIR         ${DESTDIR}/usr/local
        DOWNLOAD_DIR        ${DEP_DOWNLOAD_DIR}/${projectname}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:STRING=${DESTDIR}/usr/local
            -DCMAKE_MODULE_PATH:STRING=${PROJECT_SOURCE_DIR}/../cmake/modules
            -DCMAKE_PREFIX_PATH:STRING=${DESTDIR}/usr/local
            -DCMAKE_DEBUG_POSTFIX:STRING=d
            -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
            -DBUILD_SHARED_LIBS:BOOL=OFF
            "${_configs_line}"
            ${P_ARGS_CMAKE_ARGS}
       ${P_ARGS_UNPARSED_ARGUMENTS}
       BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release -- ${_build_j}
       INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config Release
    )

endfunction(add_cmake_project)