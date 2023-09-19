#include <pybind11/pybind11.h>

#include "convert/convert.hpp"

namespace py = pybind11;

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

    m.def("from_ascii_to_binary", &bgcode::convert::from_ascii_to_binary, R"pbdoc(
        Convert ascii gcode to binary format
    )pbdoc");

    m.def("from_binary_to_ascii", &bgcode::convert::from_ascii_to_binary, R"pbdoc(
        Convert binary gcode to textual format
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
