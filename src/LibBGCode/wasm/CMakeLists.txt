add_executable(bgcode_wa bgcode_wa.cpp index.html)

target_link_libraries(bgcode_wa bgcode_convert embind)
#set_target_properties(bgcode_wa PROPERTIES SUFFIX)
target_link_options(bgcode_wa PUBLIC
#    -sEXPORTED_RUNTIME_METHODS=stringToNewUTF8,FS
#    -sEXPORTED_FUNCTIONS=_free
)

configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/bgcode_wa.html)

