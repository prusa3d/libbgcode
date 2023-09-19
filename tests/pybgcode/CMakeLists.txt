get_target_property(_pybgcode_bindir pybgcode BINARY_DIR)
add_test(NAME test_pybgcode COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/test_convert.py)
set_property(TEST test_pybgcode PROPERTY ENVIRONMENT PYTHONPATH=${_pybgcode_bindir})
