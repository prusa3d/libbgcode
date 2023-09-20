#include <cstdio>
#include <string>
#include <utility>

#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

#include "convert/convert.hpp"

int ascii2bgcode_cfg(std::string in_fname, std::string out_fname,
                     bgcode::binarize::BinarizerConfig config)
{
    FILE *infile = std::fopen(in_fname.c_str(), "r");
    if (infile == nullptr) {
        std::string ferrstr = std::string("console.log('Can not open infile!')");
        emscripten_run_script(ferrstr.c_str());
        return -1;
    }

    FILE *outfile = std::fopen(out_fname.c_str(), "w");
    if (infile == nullptr) {
        std::string ferrstr = std::string("console.log('Can not open outfile!')");
        emscripten_run_script(ferrstr.c_str());
        return -1;
    }

    auto result = bgcode::convert::from_ascii_to_binary(*infile, *outfile, config);
    if (result != bgcode::core::EResult::Success) {
        std::string astr = std::string("console.error('Error when translating gcode: ");
        astr += translate_result(result);
        astr += "')";
        emscripten_run_script(astr.c_str());
    }

    std::fclose(infile);
    std::fclose(outfile);

    return 0;
}

bgcode::binarize::BinarizerConfig get_config()
{
    bgcode::binarize::BinarizerConfig config;
    config.checksum = bgcode::core::EChecksumType::CRC32;
    config.compression.file_metadata = bgcode::core::ECompressionType::None;
    config.compression.print_metadata = bgcode::core::ECompressionType::None;
    config.compression.printer_metadata = bgcode::core::ECompressionType::None;
    config.compression.slicer_metadata = bgcode::core::ECompressionType::Deflate;
    config.compression.gcode = bgcode::core::ECompressionType::Heatshrink_12_4;
    config.gcode_encoding = bgcode::core::EGCodeEncodingType::MeatPackComments;
    config.metadata_encoding = bgcode::core::EMetadataEncodingType::INI;

    return config;
}

EMSCRIPTEN_BINDINGS(module) {
    emscripten::function("ascii2bgcode_cfg", &ascii2bgcode_cfg);

    emscripten::enum_<bgcode::core::ECompressionType>("BGCode_CompressionType")
        .value("None", bgcode::core::ECompressionType::None)
        .value("Deflate", bgcode::core::ECompressionType::Deflate)
        .value("Heatshrink_11_4", bgcode::core::ECompressionType::Heatshrink_11_4)
        .value("Heatshrink_12_4", bgcode::core::ECompressionType::Heatshrink_12_4);
    emscripten::enum_<bgcode::core::EGCodeEncodingType>("BGCode_GCodeEncodingType")
        .value("None", bgcode::core::EGCodeEncodingType::None)
        .value("MeatPack", bgcode::core::EGCodeEncodingType::MeatPack)
        .value("MeatPackComments", bgcode::core::EGCodeEncodingType::MeatPackComments);
    emscripten::enum_<bgcode::core::EMetadataEncodingType>("BGCode_MetadataEncodingType")
        .value("INI", bgcode::core::EMetadataEncodingType::INI);
    emscripten::enum_<bgcode::core::EChecksumType>("BGCode_ChecksumType")
        .value("None", bgcode::core::EChecksumType::None)
        .value("CRC32", bgcode::core::EChecksumType::CRC32);
    emscripten::value_object<bgcode::binarize::BinarizerConfig::Compression>("BGCode_BinarizerCompression")
        .field("file_metadata", &bgcode::binarize::BinarizerConfig::Compression::file_metadata)
        .field("printer_metadata", &bgcode::binarize::BinarizerConfig::Compression::printer_metadata)
        .field("print_metadata", &bgcode::binarize::BinarizerConfig::Compression::print_metadata)
        .field("slicer_metadata", &bgcode::binarize::BinarizerConfig::Compression::slicer_metadata)
        .field("gcode", &bgcode::binarize::BinarizerConfig::Compression::gcode);
    emscripten::value_object<bgcode::binarize::BinarizerConfig>("BGCode_BinarizerConfig")
        .field("compression", &bgcode::binarize::BinarizerConfig::compression)
        .field("gcode_encoding", &bgcode::binarize::BinarizerConfig::gcode_encoding)
        .field("metadata_encoding", &bgcode::binarize::BinarizerConfig::metadata_encoding)
        .field("checksum", &bgcode::binarize::BinarizerConfig::checksum);

    emscripten::function("get_config", &get_config);
}
