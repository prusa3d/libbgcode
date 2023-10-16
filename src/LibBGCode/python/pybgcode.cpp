#include <cstdio>
#include <cerrno>
#include <cstring>
#include <pybind11/pybind11.h>

#include <boost/nowide/cstdio.hpp>

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

struct FILEWrapper {
    FILE *fptr = nullptr;

    explicit FILEWrapper (FILE *f = nullptr) : fptr{f} {}

    void close()
    {
        if (fptr) {
            fclose(fptr);
            fptr = nullptr;
        }
    }

    ~FILEWrapper() { close(); }
};

PYBIND11_MODULE(pybgcode, m) {
    m.doc() = R"pbdoc(
        Python binding of the libbgcode conversion functions
        -----------------------

        .. currentmodule:: pygcode

        .. autosummary::
           :toctree: _generate

           open
           close
           is_open
           get_config
           from_ascii_to_binary
           from_binary_to_ascii
    )pbdoc";

    py::class_<FILEWrapper> file_io_binding(m, "FILEWrapper");

    m.def("open", [](const char * name, const char *mode) {
        FILE * fptr = boost::nowide::fopen(name, mode);

        if (!fptr) {
#if _MSC_VER
            static constexpr size_t bufsz = 100;
            char buf[bufsz];
            strerror_s(buf, bufsz, errno);
            throw std::runtime_error(buf);
#else
                throw std::runtime_error(std::strerror(errno));
#endif
        }

        return std::make_unique<FILEWrapper>(fptr);
    });

    m.def("close", [](FILEWrapper &f) { f.close(); }, R"pbdoc(Close a previously opened file)pbdoc");
    m.def("is_open", [](const FILEWrapper &f) { return f.fptr != nullptr; }, R"pbdoc(Check if file is open)pbdoc");

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
        .value("MissingPrinterMetadata", bgcode::core::EResult::MissingPrinterMetadata)
        .value("MissingPrintMetadata", bgcode::core::EResult::MissingPrintMetadata)
        .value("MissingSlicerMetadat", bgcode::core::EResult::MissingSlicerMetadata)
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
    py::enum_<bgcode::core::EBlockType>(m, "BGCode_EBlockType")
        .value("FileMetadata", bgcode::core::EBlockType::FileMetadata)
        .value("GCode", bgcode::core::EBlockType::GCode)
        .value("SlicerMetadata,", bgcode::core::EBlockType::SlicerMetadata)
        .value("PrinterMetadata", bgcode::core::EBlockType::PrinterMetadata)
        .value("PrintMetadata", bgcode::core::EBlockType::PrintMetadata)
        .value("Thumbnail", bgcode::core::EBlockType::Thumbnail);
    py::enum_<bgcode::core::EThumbnailFormat>(m, "BGCode_ThumbnailFormat")
        .value("PNG", bgcode::core::EThumbnailFormat::PNG)
        .value("JPG", bgcode::core::EThumbnailFormat::JPG)
        .value("QOI", bgcode::core::EThumbnailFormat::QOI);

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

    py::class_<bgcode::core::Checksum>(m, "Checksum")
        .def(py::init<bgcode::core::EChecksumType>())
        .def("get_type", &bgcode::core::Checksum::get_type)
        .def("append", static_cast<void (bgcode::core::Checksum::*)(const std::vector<uint8_t> &)> (&bgcode::core::Checksum::append))
        .def("matches", &bgcode::core::Checksum::matches)
        .def("read", [](bgcode::core::Checksum &self, FILEWrapper &file) {
            self.read(*file.fptr);
        })
        .def("write", [](bgcode::core::Checksum &self, FILEWrapper &file) {
            self.write(*file.fptr);
        });

    m.def("get_config", &get_config,  R"pbdoc(Create a default configuration for ascii to binary gcode conversion)pbdoc");

    m.def("from_ascii_to_binary", [](FILEWrapper &infile, FILEWrapper &outfile, const bgcode::binarize::BinarizerConfig &config) {
            return bgcode::convert::from_ascii_to_binary(*infile.fptr, *outfile.fptr, config);
        },
        R"pbdoc(Convert ascii gcode to binary format)pbdoc"
    );

    m.def("from_binary_to_ascii", [] (FILEWrapper &infile, FILEWrapper &outfile, bool verify_checksum = true) {
            return bgcode::convert::from_binary_to_ascii(*infile.fptr, *outfile.fptr, verify_checksum);
        },
    R"pbdoc(Convert binary gcode to textual format)pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
