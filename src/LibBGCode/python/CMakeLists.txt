find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

pybind11_add_module(pybgcode pybgcode.cpp)

target_link_libraries(pybgcode PRIVATE bgcode_convert)
