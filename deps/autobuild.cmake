if (NOT ${PROJECT_NAME}_DEPS_PRESET)
    set (${PROJECT_NAME}_DEPS_PRESET "default")
endif ()

set (_output_quiet "")
if (${PROJECT_NAME}_DEPS_OUTPUT_QUIET)
    set (_output_quiet OUTPUT_QUIET)
endif ()

message(STATUS "Building the dependencies with preset ${${PROJECT_NAME}_DEPS_PRESET}")

set(_gen_arg "")
if (CMAKE_GENERATOR)
    set (_gen_arg "-G${CMAKE_GENERATOR}")
endif ()

execute_process(
    COMMAND ${CMAKE_COMMAND} --preset "${${PROJECT_NAME}_DEPS_PRESET}" ${_gen_arg} -B build-${${PROJECT_NAME}_DEPS_PRESET}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    ${_output_quiet}
    ERROR_VARIABLE _deps_configure_output
    RESULT_VARIABLE _deps_configure_result
)

if (NOT _deps_configure_result EQUAL 0)
    message(FATAL_ERROR "Dependency configure failed with output:\n${_deps_configure_output}")
else ()
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build-${${PROJECT_NAME}_DEPS_PRESET}
        ${_output_quiet}
        ERROR_VARIABLE _deps_build_output
        RESULT_VARIABLE _deps_build_result
    )
    if (NOT _deps_build_result EQUAL 0)
        message(FATAL_ERROR "Dependency build failed with output:\n${_deps_build_output}")
    endif ()
endif ()

list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_CURRENT_LIST_DIR}/build-${${PROJECT_NAME}_DEPS_PRESET}/destdir/usr/local)
