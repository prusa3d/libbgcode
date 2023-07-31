#ifndef CONVERT_HPP
#define CONVERT_HPP

#include "convert/export.h"
#include "core/core.hpp"

namespace bgcode { namespace convert {

// Converts the gcode file contained into src_file from ascii to binary format and save the results into dst_file
extern BGCODE_CONVERT_EXPORT core::EResult from_ascii_to_binary(FILE& src_file, FILE& dst_file);

// Converts the gcode file contained into src_file from binary to ascii format and save the results into dst_file
extern BGCODE_CONVERT_EXPORT core::EResult from_binary_to_ascii(FILE& src_file, FILE& dst_file, bool verify_checksum);

}} // bgcode::core

#endif // CONVERT_HPP
