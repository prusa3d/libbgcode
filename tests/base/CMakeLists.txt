add_executable(base_tests base_tests.cpp)

target_link_libraries(base_tests ${_libname}_base test_common)

catch_discover_tests(base_tests EXTRA_ARGS ${CATCH_EXTRA_ARGS})
