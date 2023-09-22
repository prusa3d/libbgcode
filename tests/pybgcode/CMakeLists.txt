find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Prepare the test files to be available
configure_file(${TEST_DATA_DIR}/mini_cube_b_ref.gcode ${CMAKE_CURRENT_BINARY_DIR}/test.gcode)

# Define the test for ctest and inject the correct PYTHONPATH env var for its runtime
add_test(NAME test_pybgcode COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_convert.py CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES})
set_property(TEST test_pybgcode PROPERTY ENVIRONMENT PYTHONPATH=$<TARGET_FILE_DIR:pybgcode>)
