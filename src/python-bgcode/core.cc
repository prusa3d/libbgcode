#include <core/core.hpp>
#include <boost/python.hpp>

namespace c = bgcode::core;
namespace p = boost::python;


typedef const std::vector<uint8_t>& data_vector;


BOOST_PYTHON_MODULE(core)
{
    p::object module = p::scope();
    module.attr("VERSION") = c::VERSION;
    module.attr("MAX_CHECKSUM_SIZE") = MAX_CHECKSUM_SIZE;
    module.attr("MAGIC") = p::make_tuple(c::MAGIC[0], c::MAGIC[1],
                                         c::MAGIC[2], c::MAGIC[3]);

    p::enum_<c::EResult>("EResult")
        .value("Success", c::EResult::Success)
        .value("ReadError", c::EResult::ReadError)
        .value("WriteError", c::EResult::WriteError)
        .value("InvalidMagicNumber", c::EResult::InvalidMagicNumber)
        .value("InvalidVersionNumber", c::EResult::InvalidVersionNumber)
        .value("InvalidChecksumType", c::EResult::InvalidChecksumType)
        .value("InvalidBlockType", c::EResult::InvalidBlockType)
        .value("InvalidCompressionType", c::EResult::InvalidCompressionType)
        .value("InvalidMetadataEncodingType", c::EResult::InvalidMetadataEncodingType)
        .value("InvalidGCodeEncodingType", c::EResult::InvalidGCodeEncodingType)
        .value("DataCompressionError", c::EResult::DataCompressionError)
        .value("DataUncompressionError", c::EResult::DataUncompressionError)
        .value("MetadataEncodingError", c::EResult::MetadataEncodingError)
        .value("MetadataDecodingError", c::EResult::MetadataDecodingError)
        .value("GCodeEncodingError", c::EResult::GCodeEncodingError)
        .value("GCodeDecodingError", c::EResult::GCodeDecodingError)
        .value("BlockNotFound", c::EResult::BlockNotFound)
        .value("InvalidChecksum", c::EResult::InvalidChecksum)
        .value("InvalidThumbnailFormat", c::EResult::InvalidThumbnailFormat)
        .value("InvalidThumbnailWidth", c::EResult::InvalidThumbnailWidth)
        .value("InvalidThumbnailHeight", c::EResult::InvalidThumbnailHeight)
        .value("InvalidThumbnailDataSize", c::EResult::InvalidThumbnailDataSize)
        .value("InvalidBinaryGCodeFile", c::EResult::InvalidBinaryGCodeFile)
        .value("InvalidAsciiGCodeFile", c::EResult::InvalidAsciiGCodeFile)
        .value("InvalidSequenceOfBlocks", c::EResult::InvalidSequenceOfBlocks)
        .value("InvalidBuffer", c::EResult::InvalidBuffer)
        .value("AlreadyBinarized", c::EResult::AlreadyBinarized)
    ;

    p::enum_<c::EChecksumType>("EChecksumType")
        .value("None", c::EChecksumType::None)
        .value("CRC32", c::EChecksumType::CRC32)
    ;

    p::enum_<c::EBlockType>("EBlockType")
        .value("FileMetadata", c::EBlockType::FileMetadata)
        .value("GCode", c::EBlockType::GCode)
        .value("SlicerMetadata", c::EBlockType::SlicerMetadata)
        .value("PrinterMetadata", c::EBlockType::PrinterMetadata)
        .value("PrintMetadata", c::EBlockType::PrintMetadata)
        .value("Thumbnail", c::EBlockType::Thumbnail)
    ;

    p::enum_<c::ECompressionType>("ECompressionType")
        .value("None", c::ECompressionType::None)
        .value("Deflate", c::ECompressionType::Deflate)
        .value("Heatshrink_11_4", c::ECompressionType::Heatshrink_11_4)
        .value("Heatshrink_12_4", c::ECompressionType::Heatshrink_12_4)
    ;

    p::enum_<c::EMetadataEncodingType>("EMetadataEncodingType")
        .value("INI", c::EMetadataEncodingType::INI)
    ;

    p::enum_<c::EGCodeEncodingType>("EGCodeEncodingType")
        .value("None", c::EGCodeEncodingType::None)
        .value("MeatPack", c::EGCodeEncodingType::MeatPack)
        .value("MeatPackComments", c::EGCodeEncodingType::MeatPackComments)
    ;

    p::enum_<c::EThumbnailFormat>("EThumbnailFormat")
        .value("PNG", c::EThumbnailFormat::PNG)
        .value("JPG", c::EThumbnailFormat::JPG)
        .value("QOI", c::EThumbnailFormat::QOI)
    ;

    p::class_<c::Checksum>("Checksum",
                p::init<c::EChecksumType>(
                    p::args("EChecksumType"),
                    "Constructs a checksum of the given type."))
        .def("get_type", &c::Checksum::get_type)
        .def("append", &c::Checksum::append<uint8_t*, size_t>,
                "Append vector to the checksum")
        //.def("append", &c::Checksum::append<data_vector>,
        //        "Append data to the checksum")
        .def("matches", &c::Checksum::matches, p::args("other"),
                "Returns true if the given checksum is equal to this one")
        ;
}
