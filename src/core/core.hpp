#ifndef BGCODE_CORE_HPP
#define BGCODE_CORE_HPP

#include "core/export.h"

#include <cstdio>
#include <cstdint>
#include <array>
#include <vector>

namespace bgcode { namespace core {

static constexpr const std::array<uint8_t, 4> MAGIC{ 'G', 'C', 'D', 'E' };
static constexpr const uint32_t VERSION = 1;

enum class EResult : uint16_t
{
    Success,
    ReadError,
    WriteError,
    InvalidMagicNumber,
    InvalidVersionNumber,
    InvalidChecksumType,
    InvalidBlockType,
    InvalidCompressionType,
    InvalidMetadataEncodingType,
    InvalidGCodeEncodingType,
    DataCompressionError,
    DataUncompressionError,
    MetadataEncodingError,
    MetadataDecodingError,
    GCodeEncodingError,
    GCodeDecodingError,
    BlockNotFound,
    InvalidChecksum,
    InvalidThumbnailFormat,
    InvalidThumbnailWidth,
    InvalidThumbnailHeight,
    InvalidThumbnailDataSize,
    InvalidBinaryGCodeFile,
    InvalidAsciiGCodeFile,
    InvalidSequenceOfBlocks,
    AlreadyBinarized
};

enum class EChecksumType : uint16_t
{
    None,
    CRC32
};

enum class EBlockType : uint16_t
{
    FileMetadata,
    GCode,
    SlicerMetadata,
    PrinterMetadata,
    PrintMetadata,
    Thumbnail
};

enum class ECompressionType : uint16_t
{
    None,
    Deflate,
    Heatshrink_11_4,
    Heatshrink_12_4
};

enum class EMetadataEncodingType : uint16_t
{
    INI
};

enum class EGCodeEncodingType : uint16_t
{
    None,
    MeatPack,
    MeatPackComments
};

enum class EThumbnailFormat : uint16_t
{
    PNG,
    JPG,
    QOI
};

class Checksum
{
public:
    // Constructs a checksum of the given type.
    // The checksum data are sized accordingly.
    explicit Checksum(EChecksumType type);

    EChecksumType get_type() const;

    // Appends the given data to the cache and performs a checksum update if 
    // the size of the cache exceeds the max checksum cache size.
    void append(const std::vector<uint8_t>& data);
    // Returns true if the given checksum is equal to this one
    bool matches(Checksum& other);

    EResult write(FILE& file);
    EResult read(FILE& file);

private:
    EChecksumType m_type;
    std::vector<uint8_t> m_cache;
    std::vector<uint8_t> m_checksum;

    void update();
};

struct FileHeader
{
    uint32_t magic{ *(uint32_t*)(MAGIC.data()) };
    uint32_t version{ VERSION };
    uint16_t checksum_type{ (uint16_t)EChecksumType::None };

    EResult write(FILE& file) const;
    EResult read(FILE& file, const uint32_t* const max_version);
};

struct BlockHeader
{
    uint16_t type{ 0 };
    uint16_t compression{ 0 };
    uint32_t uncompressed_size{ 0 };
    uint32_t compressed_size{ 0 };

    // Updates the given checksum with the data of this BlockHeader
    void update_checksum(Checksum& checksum) const;

    EResult write(FILE& file) const;
    EResult read(FILE& file);
};

// TO REMOVE
extern BGCODE_CORE_EXPORT int foo();

// Get the max size of the cache used to calculate checksums, in bytes
extern BGCODE_CORE_EXPORT size_t get_checksum_max_cache_size();
// Set the max size of the cache used to calculate checksums, in bytes
extern BGCODE_CORE_EXPORT void set_checksum_max_cache_size(size_t size);

// Returns true if the given file is a valid binary gcode
// Does not modify the file position
extern BGCODE_CORE_EXPORT bool is_valid_binary_gcode(FILE& file);

// Reads the file header.
// If max_version is not null, version is checked against the passed value
// If return == EResult::Success:
// - header will contain the file header
// - file position will be set at the start of the 1st block header
extern BGCODE_CORE_EXPORT EResult read_header(FILE& file, FileHeader& header, const uint32_t* const max_version);

// Reads next block header from the current file position.
// File position must be at the start of a block header.
// If return == EResult::Success:
// - block_header will contain the header of the block
// - file position will be set at the start of the block parameters data
extern BGCODE_CORE_EXPORT EResult read_next_block_header(FILE& file, const FileHeader& file_header, BlockHeader& block_header, bool verify_checksum);

// Searches and reads next block header with the given type from the current file position.
// File position must be at the start of a block header.
// If return == EResult::Success:
// - block_header will contain the header of the block with the required type
// - file position will be set at the start of the block parameters data
// otherwise:
// - file position will keep the current value
extern BGCODE_CORE_EXPORT EResult read_next_block_header(FILE& file, const FileHeader& file_header, BlockHeader& block_header, EBlockType type, bool verify_checksum);

// Skips the payload (parameters + data) of the block with the given block header.
// File position must be at the start of the block parameters.
// If return == EResult::Success:
// - file position will be set at the start of the block checksum, if present, or of next block header
extern BGCODE_CORE_EXPORT EResult skip_block_payload(FILE& file, const BlockHeader& block_header);

// Skips the content (parameters + data + checksum) of the block with the given block header.
// File position must be at the start of the block parameters.
// If return == EResult::Success:
// - file position will be set at the start of the next block header
extern BGCODE_CORE_EXPORT EResult skip_block_content(FILE& file, const FileHeader& file_header, const BlockHeader& block_header);

// Returns the size of the parameters of the given block type, in bytes.
extern BGCODE_CORE_EXPORT size_t block_parameters_size(EBlockType type);

// Returns the size of the payload (parameters + data) of the block with the given header, in bytes.
extern BGCODE_CORE_EXPORT size_t block_payload_size(const BlockHeader& block_header);

// Returns the size of the checksum of the given type, in bytes.
extern BGCODE_CORE_EXPORT size_t checksum_size(EChecksumType type);

// Returns the size of the content (parameters + data + checksum) of the block with the given header, in bytes.
extern BGCODE_CORE_EXPORT size_t block_content_size(const FileHeader& file_header, const BlockHeader& block_header);

}} // bgcode::core

#endif // BGCODE_CORE_HPP
