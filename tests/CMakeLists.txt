find_package(Catch2 2.9 REQUIRED)

include(Catch)

set(CATCH_EXTRA_ARGS "" CACHE STRING "Extra arguments for catch2 test suites.")

add_library(test_common INTERFACE)
target_include_directories(test_common INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(test_common INTERFACE Catch2::Catch2)

add_subdirectory(core)

if (${PROJECT_NAME}_BUILD_COMPONENT_Base)
    add_subdirectory(base)
endif()

if (${PROJECT_NAME}_BUILD_COMPONENT_Convert)
    add_subdirectory(convert)
endif ()
