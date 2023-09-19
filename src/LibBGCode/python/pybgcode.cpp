#include <cstdio>
#include <pybind11/pybind11.h>

#include "convert/convert.hpp"

namespace py = pybind11;

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

PYBIND11_MODULE(pybgcode, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: pygcode

        .. autosummary::
           :toctree: _generate

           from_ascii_to_binary
           from_binary_to_ascii
    )pbdoc";

    py::enum_<bgcode::core::EResult>(m, "EResult")
        .value("Success", bgcode::core::EResult::Success)
        .value("ReadError", bgcode::core::EResult::ReadError)
        .value("WriteError", bgcode::core::EResult::WriteError)
        .value("InvalidMagicNumber", bgcode::core::EResult::InvalidMagicNumber)
        .value("InvalidVersionNumber", bgcode::core::EResult::InvalidVersionNumber)
        .value("InvalidChecksumType", bgcode::core::EResult::InvalidChecksumType)
        .value("InvalidBlockType", bgcode::core::EResult::InvalidBlockType)
        .value("InvalidCompressionType", bgcode::core::EResult::InvalidCompressionType)
        .value("InvalidMetadataEncodingType", bgcode::core::EResult::InvalidMetadataEncodingType)
        .value("InvalidGCodeEncodingType", bgcode::core::EResult::InvalidGCodeEncodingType)
        .value("DataCompressionError", bgcode::core::EResult::DataCompressionError)
        .value("DataUncompressionError", bgcode::core::EResult::DataUncompressionError)
        .value("MetadataEncodingError", bgcode::core::EResult::MetadataEncodingError)
        .value("MetadataDecodingError", bgcode::core::EResult::MetadataDecodingError)
        .value("GCodeEncodingError", bgcode::core::EResult::GCodeEncodingError)
        .value("GCodeDecodingError", bgcode::core::EResult::GCodeDecodingError)
        .value("BlockNotFound", bgcode::core::EResult::BlockNotFound)
        .value("InvalidChecksum", bgcode::core::EResult::InvalidChecksum)
        .value("InvalidThumbnailFormat", bgcode::core::EResult::InvalidThumbnailFormat)
        .value("InvalidThumbnailWidth", bgcode::core::EResult::InvalidThumbnailWidth)
        .value("InvalidThumbnailHeight", bgcode::core::EResult::InvalidThumbnailHeight)
        .value("InvalidThumbnailDataSize", bgcode::core::EResult::InvalidThumbnailDataSize)
        .value("InvalidBinaryGCodeFile", bgcode::core::EResult::InvalidBinaryGCodeFile)
        .value("InvalidAsciiGCodeFile", bgcode::core::EResult::InvalidAsciiGCodeFile)
        .value("InvalidSequenceOfBlocks", bgcode::core::EResult::InvalidSequenceOfBlocks)
        .value("InvalidBuffer", bgcode::core::EResult::InvalidBuffer)
        .value("AlreadyBinarized", bgcode::core::EResult::AlreadyBinarized)
        ;

    py::enum_<bgcode::core::ECompressionType>(m, "BGCode_CompressionType")
        .value("none", bgcode::core::ECompressionType::None)
        .value("Deflate", bgcode::core::ECompressionType::Deflate)
        .value("Heatshrink_11_4", bgcode::core::ECompressionType::Heatshrink_11_4)
        .value("Heatshrink_12_4", bgcode::core::ECompressionType::Heatshrink_12_4);
    py::enum_<bgcode::core::EGCodeEncodingType>(m, "BGCode_GCodeEncodingType")
        .value("none", bgcode::core::EGCodeEncodingType::None)
        .value("MeatPack", bgcode::core::EGCodeEncodingType::MeatPack)
        .value("MeatPackComments", bgcode::core::EGCodeEncodingType::MeatPackComments);
    py::enum_<bgcode::core::EMetadataEncodingType>(m, "BGCode_MetadataEncodingType")
        .value("INI", bgcode::core::EMetadataEncodingType::INI);
    py::enum_<bgcode::core::EChecksumType>(m, "BGCode_ChecksumType")
        .value("none", bgcode::core::EChecksumType::None)
        .value("CRC32", bgcode::core::EChecksumType::CRC32);

    py::class_<bgcode::binarize::BinarizerConfig::Compression>(m, "BGCode_BinarizerCompression")
        .def_readwrite("file_metadata", &bgcode::binarize::BinarizerConfig::Compression::file_metadata)
        .def_readwrite("printer_metadata", &bgcode::binarize::BinarizerConfig::Compression::printer_metadata)
        .def_readwrite("print_metadata", &bgcode::binarize::BinarizerConfig::Compression::print_metadata)
        .def_readwrite("slicer_metadata", &bgcode::binarize::BinarizerConfig::Compression::slicer_metadata)
        .def_readwrite("gcode", &bgcode::binarize::BinarizerConfig::Compression::gcode);
    py::class_<bgcode::binarize::BinarizerConfig>(m, "BGCode_BinarizerConfig")
        .def_readwrite("compression", &bgcode::binarize::BinarizerConfig::compression)
        .def_readwrite("gcode_encoding", &bgcode::binarize::BinarizerConfig::gcode_encoding)
        .def_readwrite("metadata_encoding", &bgcode::binarize::BinarizerConfig::metadata_encoding)
        .def_readwrite("checksum", &bgcode::binarize::BinarizerConfig::checksum);

    m.def("get_config", &get_config);

    m.def("fopen", [](const char * name, const char *mode) {
        FILE * fptr = std::fopen(name, mode);

        return py::capsule(fptr, "FILE*");
    });

    m.def("fclose", [](py::capsule fileptr) { std::fclose(fileptr.get_pointer<FILE>()); });

    m.def("from_ascii_to_binary", [](py::capsule infile, py::capsule outfile, const bgcode::binarize::BinarizerConfig &config) {
            return bgcode::convert::from_ascii_to_binary(*infile.get_pointer<FILE>(), *outfile.get_pointer<FILE>(), config); },
        R"pbdoc(Convert ascii gcode to binary format)pbdoc"
    );

    m.def("from_binary_to_ascii", [] (py::capsule infile, py::capsule outfile, bool verify_checksum) {
            return bgcode::convert::from_binary_to_ascii(*infile.get_pointer<FILE>(), *outfile.get_pointer<FILE>(), verify_checksum);
        } , R"pbdoc(
        Convert binary gcode to textual format
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
