#include <cstdio>
#include <cerrno>
#include <cstring>
#include <pybind11/pybind11.h>

#include <boost/nowide/cstdio.hpp>

#include "convert/convert.hpp"

namespace py = pybind11;

bgcode::binarize::BinarizerConfig get_config()
{
    using namespace bgcode;

    binarize::BinarizerConfig config;
    config.checksum = core::EChecksumType::CRC32;
    config.compression.file_metadata = core::ECompressionType::None;
    config.compression.print_metadata = core::ECompressionType::None;
    config.compression.printer_metadata = core::ECompressionType::None;
    config.compression.slicer_metadata = core::ECompressionType::Deflate;
    config.compression.gcode = core::ECompressionType::Heatshrink_12_4;
    config.gcode_encoding = core::EGCodeEncodingType::MeatPackComments;
    config.metadata_encoding = core::EMetadataEncodingType::INI;

    return config;
}

struct FILEWrapper {
    FILE *fptr = nullptr;

    explicit FILEWrapper (FILE *f = nullptr) : fptr{f} {}
    FILEWrapper(FILEWrapper &&) = delete;
    FILEWrapper& operator=(FILEWrapper &&) = delete;

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
    using namespace bgcode;

    static constexpr size_t MaxBuffSz = 2048;

    m.doc() = R"pbdoc(
        Python binding of the libbgcode conversion functions
        -----------------------

        .. currentmodule:: pygcode

        .. autosummary::
           :toctree: _generate

           open
           close
           is_open
           translate_result
           is_valid_binary_gcode
           read_header
           get_config
           from_ascii_to_binary
           from_binary_to_ascii
    )pbdoc";

    py::class_<FILEWrapper> file_io_binding(m,
                                            "FILEWrapper",
                                            R"pbdoc(A FILE* wrapper to pass around in Python code)pbdoc");

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
    },
    R"pbdoc(Open a gcode file to process using pybgcode)pbdoc",
                                            py::arg("name"),
                                            py::arg("mode"));

    m.def("close", [](FILEWrapper &f) { f.close(); }, R"pbdoc(Close a previously opened file)pbdoc", py::arg("file"));
    m.def("is_open", [](const FILEWrapper &f) { return f.fptr != nullptr; }, R"pbdoc(Check if file is open)pbdoc", py::arg("file"));

    py::enum_<core::EResult>(m, "EResult")
        .value("Success", core::EResult::Success)
        .value("ReadError", core::EResult::ReadError)
        .value("WriteError", core::EResult::WriteError)
        .value("InvalidMagicNumber", core::EResult::InvalidMagicNumber)
        .value("InvalidVersionNumber", core::EResult::InvalidVersionNumber)
        .value("InvalidChecksumType", core::EResult::InvalidChecksumType)
        .value("InvalidBlockType", core::EResult::InvalidBlockType)
        .value("InvalidCompressionType", core::EResult::InvalidCompressionType)
        .value("InvalidMetadataEncodingType", core::EResult::InvalidMetadataEncodingType)
        .value("InvalidGCodeEncodingType", core::EResult::InvalidGCodeEncodingType)
        .value("DataCompressionError", core::EResult::DataCompressionError)
        .value("DataUncompressionError", core::EResult::DataUncompressionError)
        .value("MetadataEncodingError", core::EResult::MetadataEncodingError)
        .value("MetadataDecodingError", core::EResult::MetadataDecodingError)
        .value("GCodeEncodingError", core::EResult::GCodeEncodingError)
        .value("GCodeDecodingError", core::EResult::GCodeDecodingError)
        .value("BlockNotFound", core::EResult::BlockNotFound)
        .value("InvalidChecksum", core::EResult::InvalidChecksum)
        .value("InvalidThumbnailFormat", core::EResult::InvalidThumbnailFormat)
        .value("InvalidThumbnailWidth", core::EResult::InvalidThumbnailWidth)
        .value("InvalidThumbnailHeight", core::EResult::InvalidThumbnailHeight)
        .value("InvalidThumbnailDataSize", core::EResult::InvalidThumbnailDataSize)
        .value("InvalidBinaryGCodeFile", core::EResult::InvalidBinaryGCodeFile)
        .value("InvalidAsciiGCodeFile", core::EResult::InvalidAsciiGCodeFile)
        .value("InvalidSequenceOfBlocks", core::EResult::InvalidSequenceOfBlocks)
        .value("InvalidBuffer", core::EResult::InvalidBuffer)
        .value("AlreadyBinarized", core::EResult::AlreadyBinarized)
        .value("MissingPrinterMetadata", core::EResult::MissingPrinterMetadata)
        .value("MissingPrintMetadata", core::EResult::MissingPrintMetadata)
        .value("MissingSlicerMetadat", core::EResult::MissingSlicerMetadata)
        ;

    py::enum_<core::ECompressionType>(m, "CompressionType")
        .value("none", core::ECompressionType::None)
        .value("Deflate", core::ECompressionType::Deflate)
        .value("Heatshrink_11_4", core::ECompressionType::Heatshrink_11_4)
        .value("Heatshrink_12_4", core::ECompressionType::Heatshrink_12_4);
    py::enum_<core::EGCodeEncodingType>(m, "GCodeEncodingType")
        .value("none", core::EGCodeEncodingType::None)
        .value("MeatPack", core::EGCodeEncodingType::MeatPack)
        .value("MeatPackComments", core::EGCodeEncodingType::MeatPackComments);
    py::enum_<core::EMetadataEncodingType>(m, "MetadataEncodingType")
        .value("INI", core::EMetadataEncodingType::INI);
    py::enum_<core::EChecksumType>(m, "ChecksumType")
        .value("none", core::EChecksumType::None)
        .value("CRC32", core::EChecksumType::CRC32);
    py::enum_<core::EBlockType>(m, "EBlockType")
        .value("FileMetadata", core::EBlockType::FileMetadata)
        .value("GCode", core::EBlockType::GCode)
        .value("SlicerMetadata,", core::EBlockType::SlicerMetadata)
        .value("PrinterMetadata", core::EBlockType::PrinterMetadata)
        .value("PrintMetadata", core::EBlockType::PrintMetadata)
        .value("Thumbnail", core::EBlockType::Thumbnail);
    py::enum_<core::EThumbnailFormat>(m, "ThumbnailFormat")
        .value("PNG", core::EThumbnailFormat::PNG)
        .value("JPG", core::EThumbnailFormat::JPG)
        .value("QOI", core::EThumbnailFormat::QOI);

    py::class_<binarize::BinarizerConfig::Compression>(m, "BinarizerCompression")
        .def_readwrite("file_metadata", &binarize::BinarizerConfig::Compression::file_metadata)
        .def_readwrite("printer_metadata", &binarize::BinarizerConfig::Compression::printer_metadata)
        .def_readwrite("print_metadata", &binarize::BinarizerConfig::Compression::print_metadata)
        .def_readwrite("slicer_metadata", &binarize::BinarizerConfig::Compression::slicer_metadata)
        .def_readwrite("gcode", &binarize::BinarizerConfig::Compression::gcode);
    py::class_<binarize::BinarizerConfig>(m, "BinarizerConfig")
        .def_readwrite("compression", &binarize::BinarizerConfig::compression)
        .def_readwrite("gcode_encoding", &binarize::BinarizerConfig::gcode_encoding)
        .def_readwrite("metadata_encoding", &binarize::BinarizerConfig::metadata_encoding)
        .def_readwrite("checksum", &binarize::BinarizerConfig::checksum);

    py::class_<core::Checksum>(m, "Checksum")
        .def(py::init<core::EChecksumType>())
        .def("get_type", &core::Checksum::get_type)
        .def("append", static_cast<void (core::Checksum::*)(const std::vector<uint8_t> &)> (&core::Checksum::append))
        .def("matches", &core::Checksum::matches)
        .def("read", [](core::Checksum &self, FILEWrapper &file) {
            return self.read(*file.fptr);
        })
        .def("write", [](core::Checksum &self, FILEWrapper &file) {
            return self.write(*file.fptr);
        });

    py::class_<core::FileHeader>(m, "FileHeader")
        .def_readonly("magic", &core::FileHeader::magic)
        .def_readonly("version", &core::FileHeader::version)
        .def_readonly("checksum_type", &core::FileHeader::checksum_type)
        .def("read", [](core::FileHeader &self, FILEWrapper &file) {
            return self.read(*file.fptr, nullptr);
        })
        .def("write", [](core::FileHeader &self, FILEWrapper &file) {
            return self.write(*file.fptr);
        });

    py::class_<core::BlockHeader>(m, "BlockHeader")
        .def(py::init<>())
        .def_readonly("type", &core::BlockHeader::type)
        .def_readonly("compression", &core::BlockHeader::compression)
        .def_readonly("uncompressed_size", &core::BlockHeader::type)
        .def_readonly("compressed_size", &core::BlockHeader::compressed_size)
        .def("update_checksum", &core::BlockHeader::update_checksum)
        .def("get_size()", &core::BlockHeader::get_size)
        .def("read", [](core::BlockHeader &self, FILEWrapper &file) {
            return self.read(*file.fptr);
        })
        .def("write", [](core::BlockHeader &self, FILEWrapper &file) {
            return self.write(*file.fptr);
        });

    py::class_<core::ThumbnailParams>(m, "ThumbnailParams")
        .def_readonly("format", &core::ThumbnailParams::format)
        .def_readonly("width", &core::ThumbnailParams::width)
        .def_readonly("height", &core::ThumbnailParams::height)
        .def("read", [](core::ThumbnailParams &self, FILEWrapper &file) {
            return self.read(*file.fptr);
        })
        .def("write", [](core::ThumbnailParams &self, FILEWrapper &file) {
            return self.write(*file.fptr);
        });

    m.def("translate_result",
          &core::translate_result,
          R"pbdoc(Returns a string description of the given result)pbdoc",
          py::arg("result")
    );

    m.def(
        "is_valid_binary_gcode",
        [](FILEWrapper& file, bool check_contents) {
            std::vector<uint8_t> cs_buffer;

            if (check_contents)
                cs_buffer.resize(MaxBuffSz);

            size_t cs_buffer_size = cs_buffer.size();

            return core::is_valid_binary_gcode(*file.fptr, check_contents, cs_buffer.data(), cs_buffer_size);
        },
        R"pbdoc(
            Returns EResult.Success if the given file is a valid binary gcode.
            If check_contents is set to true, the order of the blocks is checked.
            Does not modify the file position.
            Caller is responsible for providing buffer for checksum calculation, if needed.
        )pbdoc",
        py::arg("file"), py::arg("check_contents") = false
    );

    m.def(
         "read_header",
        [](FILEWrapper& file, core::FileHeader& header) {
            return core::read_header(*file.fptr, header, nullptr);
        },
        R"pbdoc(
            Reads the file header.
            If max_version is not null, version is checked against the passed value.
            If return == EResult.Success:
            - header will contain the file header.
            - file position will be set at the start of the 1st block header.
        )pbdoc",
        py::arg("file"), py::arg("header"));

    m.def("read_header", [](FILEWrapper& file, core::FileHeader& header, uint32_t max_version) {
            return core::read_header(*file.fptr, header, &max_version);
        }, py::arg("file"), py::arg("header"), py::arg("max_version"));

    m.def(
        "read_next_block_header",
        [](FILEWrapper &file, const core::FileHeader &file_header, core::BlockHeader &block_header) {
            return core::read_next_block_header(*file.fptr, file_header, block_header, nullptr, 0);
        },
        R"pbdoc(
            Reads next block header from the current file position.
            File position must be at the start of a block header.
            If return == EResult::Success:
            - block_header will contain the header of the block.
            - file position will be set at the start of the block parameters data.
            Caller is responsible for providing buffer for checksum calculation, if needed.
        )pbdoc",
        py::arg("file"), py::arg("file_header"), py::arg("block_header")
    );

    m.def(
        "read_next_block_header",
        [](FILEWrapper &file,
           const core::FileHeader &file_header, core::BlockHeader &block_header, core::EBlockType block_type) {
            return core::read_next_block_header(*file.fptr, file_header, block_header,
                                                block_type, nullptr, 0);
        },
        R"pbdoc(
            Searches and reads next block header with the given type from the current file position.
            File position must be at the start of a block header.
            If return == EResult::Success:
            - block_header will contain the header of the block with the required type.
            - file position will be set at the start of the block parameters data.
            otherwise:
            - file position will keep the current value.
        )pbdoc",
        py::arg("file"), py::arg("file_header"), py::arg("block_header"), py::arg("block_type"));

    m.def(
        "verify_block_checksum",
        [](FILEWrapper& file, const core::FileHeader& file_header, const core::BlockHeader& block_header){
            std::array<uint8_t, MaxBuffSz> buff;
            return core::verify_block_checksum(*file.fptr, file_header, block_header, buff.data(), buff.size());
        },
        R"pbdoc(
            Calculates block checksum and verify it against checksum stored in file.
            If return == EResult::Success:
            - file position will be set at the start of the next block header.
        )pbdoc",
        py::arg("file"), py::arg("file_header"), py::arg("block_header")
    );

    m.def(
         "skip_block_content",
        [](FILEWrapper &file, const core::FileHeader &file_header, const core::BlockHeader &block_header) {
            return core::skip_block_content(*file.fptr, file_header, block_header);
        },
        R"pbdoc(
            Skips the content (parameters + data + checksum) of the block with the given block header.
            File position must be at the start of the block parameters.
            If return == EResult::Success:
            - file position will be set at the start of the next block header.
        )pbdoc",
        py::arg("file"), py::arg("file_header"), py::arg("block_header")
    );

    m.def(
        "skip_block",
        [](FILEWrapper &file, const core::FileHeader &file_header, const core::BlockHeader& block_header) {
            return core::skip_block(*file.fptr, file_header, block_header);
        },
        R"pbdoc(
            Skips the block with the given block header.
            File position must be set by a previous call to BlockHeader::write() or BlockHeader::read().
            If return == EResult::Success:
            - file position will be set at the start of the next block header.
        )pbdoc",
        py::arg("file"), py::arg("file_header"), py::arg("block_header")
    );

    m.def(
        "block_parameters_size",
        &core::block_parameters_size,
        R"pbdoc(
            Returns the size of the parameters of the given block type, in bytes.
        )pbdoc",
        py::arg("type")
    );

    m.def(
        "block_payload_size",
        &core::block_payload_size,
        R"pbdoc(
            Returns the size of the payload (parameters + data) of the block with the given header, in bytes.
        )pbdoc",
        py::arg("block_header")
    );

    m.def(
        "checksum_size",
        &core::checksum_size,
        R"pbdoc(
            Returns the size of the checksum of the given type, in bytes.
        )pbdoc",
        py::arg("type")
    );

    m.def(
        "block_content_size",
        &core::block_content_size,
        R"pbdoc(
            Returns the size of the content (parameters + data + checksum) of the block with the given header, in bytes.
        )pbdoc",
        py::arg("file_header"), py::arg("block_header")
    );

    m.def("get_config", &get_config,  R"pbdoc(Create a default configuration for ascii to binary gcode conversion)pbdoc");

    m.def("from_ascii_to_binary", [](FILEWrapper &infile, FILEWrapper &outfile, const binarize::BinarizerConfig &config) {
            return convert::from_ascii_to_binary(*infile.fptr, *outfile.fptr, config);
        },
        R"pbdoc(Convert ascii gcode to binary format)pbdoc",
        py::arg("infile"), py::arg("outfile"), py::arg("config") = get_config()
    );

    m.def("from_binary_to_ascii", [] (FILEWrapper &infile, FILEWrapper &outfile, bool verify_checksum) {
            return convert::from_binary_to_ascii(*infile.fptr, *outfile.fptr, verify_checksum);
        },
        R"pbdoc(Convert binary gcode to textual format)pbdoc",
        py::arg("infile"), py::arg("outfile"), py::arg("verify_checksum") = true
    );

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
