add_executable(convert_tests convert_tests.cpp)

target_link_libraries(convert_tests ${_libname}_convert test_common)

catch_discover_tests(convert_tests EXTRA_ARGS ${CATCH_EXTRA_ARGS})
