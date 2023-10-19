#ifndef _BGCODE_CORE_HPP_
#define _BGCODE_CORE_HPP_

#include "core/export.h"

#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <climits>
#include <type_traits>
#include <iterator>
#include <array>
#include <vector>
#include <string>
#include <string_view>

namespace bgcode { namespace core {

static constexpr const std::array<char, 4> MAGIC{ 'G', 'C', 'D', 'E' };
// Library version
static constexpr const uint32_t VERSION = 1;
// Max size of checksum buffer data, in bytes
// Increase this value if you implement a checksum algorithm needing a bigger buffer
static constexpr const size_t MAX_CHECKSUM_SIZE = 4;

template<class I, class T = I>
using IntegerOnly = std::enable_if_t<std::is_integral_v<I>, T>;

template<class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template<class BufT> constexpr bool IsBufferType =
    std::is_convertible_v<remove_cvref_t<BufT>, std::byte> ||
    std::is_convertible_v<remove_cvref_t<BufT>, unsigned char> ||
    std::is_convertible_v<remove_cvref_t<BufT>, char>;

template<class It> constexpr bool IsBufferIterator =
    IsBufferType< typename std::iterator_traits<It>::value_type>;

template<class BufT, class T = BufT>
using BufferTypeOnly = std::enable_if_t<IsBufferType<BufT>, T>;

template<class It, class T = It>
using BufferIteratorOnly = std::enable_if_t<IsBufferIterator<It>, T>;

// For LE byte sequences only
template<class IntT, class It, class = BufferIteratorOnly<It>>
constexpr IntegerOnly<IntT> load_integer(It from, It to) noexcept
{
    IntT result{};

    size_t i = 0;
    for (It it = from; it != to && i < sizeof(IntT); ++it) {
        result |= (static_cast<IntT>(*it) << (i++ * sizeof(std::byte) * CHAR_BIT));
    }

    return result;
}

template<class IntT, class OutIt>
constexpr BufferIteratorOnly<OutIt, void>
store_integer_le(IntT value, OutIt out, size_t sz = sizeof(IntT))
{
    for (size_t i = 0; i < std::min(sizeof(IntT), sz); ++i)
    {
        *out++ = static_cast<typename std::iterator_traits<OutIt>::value_type>(
            (value >> (i * CHAR_BIT)) & UCHAR_MAX
        );
    }
}

template<class IntT, class OutIt>
std::enable_if_t<!IsBufferIterator<OutIt>, void>
store_integer_le(IntT value, OutIt out, size_t sz = sizeof(IntT))
{
    for (size_t i = 0; i < std::min(sizeof(IntT), sz); ++i)
    {
        *out++ = (value >> (i * CHAR_BIT)) & UCHAR_MAX;
    }
}

template<class It, class = BufferIteratorOnly<It>>
static constexpr uint32_t crc32_sw(It from, It to, uint32_t crc)
{
    constexpr uint32_t ui32Max = 0xFFFFFFFF;
    constexpr uint32_t crcMagic = 0xEDB88320;

    uint32_t value = crc ^ ui32Max;
    for (auto it = from; it != to; ++it) {
        value ^= load_integer<uint32_t>(it, std::next(it));
        for (int bit = 0; bit < CHAR_BIT; bit++) {
            if (value & 1)
                value = (value >> 1) ^ crcMagic;
            else
                value >>= 1;
        }
    }
    value ^= ui32Max;

    return value;
}

constexpr auto MAGICi32 = load_integer<uint32_t>(std::begin(MAGIC), std::end(MAGIC));

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
    InvalidBuffer,
    AlreadyBinarized,
    MissingPrinterMetadata,
    MissingPrintMetadata,
    MissingSlicerMetadata,
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

    // Append vector of data to checksum
    void append(const std::vector<std::byte>& data);

    // Append data to the checksum
    template<class BufT>
    void append(const BufT* data, size_t size)
    {
        if (data == nullptr || size == 0)
            return;

        switch (m_type)
        {
        case EChecksumType::None:
        {
            break;
        }
        case EChecksumType::CRC32:
        {
            static_assert(sizeof(m_checksum) >= sizeof(uint32_t), "CRC32 checksum requires at least 4 bytes");
            const auto old_crc = load_integer<uint32_t>(m_checksum.begin(), m_checksum.end()); //*(uint32_t*)m_checksum.data();
            const uint32_t new_crc = crc32_sw(data, data + size, old_crc);
            store_integer_le(new_crc, m_checksum.begin(), m_checksum.size());
            break;
        }
        }
    }

    // Append any aritmetic data to the checksum (shorthand for aritmetic types)
    template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    void append(T& data) { append(reinterpret_cast<const std::byte*>(&data), sizeof(data)); }

    // Returns true if the given checksum is equal to this one
    bool matches(Checksum& other);

    EResult write(FILE& file);
    EResult read(FILE& file);

private:
    EChecksumType m_type;
    // actual size of checksum buffer, type dependent
    size_t m_size;
    std::array<std::byte, MAX_CHECKSUM_SIZE> m_checksum;
};

struct FileHeader
{
    uint32_t magic{ MAGICi32 };
    uint32_t version{ VERSION };
    uint16_t checksum_type{ static_cast<uint16_t>(EChecksumType::None) };

    EResult write(FILE& file) const;
    EResult read(FILE& file, const uint32_t* const max_version);
};

struct BlockHeader
{
    uint16_t type{ 0 };
    uint16_t compression{ 0 };
    uint32_t uncompressed_size{ 0 };
    uint32_t compressed_size{ 0 };

    BlockHeader() = default;
    BlockHeader(uint16_t type, uint16_t compression, uint32_t uncompressed_size, uint32_t compressed_size = 0);

    // Updates the given checksum with the data of this BlockHeader
    void update_checksum(Checksum& checksum) const;

    // Returns the position of this block in the file.
    // Position is set by calling write() and read() methods.
    long get_position() const;

    EResult write(FILE& file) const;
    EResult read(FILE& file);

    // Returs the size of this BlockHeader, in bytes
    size_t get_size() const;

private:
    mutable long m_position{ 0 };
};

struct ThumbnailParams
{
    uint16_t format;
    uint16_t width;
    uint16_t height;

    EResult write(FILE& file) const;
    EResult read(FILE& file);
};

// Returns a string description of the given result
extern BGCODE_CORE_EXPORT std::string_view translate_result(EResult result);

// Returns EResult::Success if the given file is a valid binary gcode
// If check_contents is set to true, the order of the blocks is checked
// Does not modify the file position
// Caller is responsible for providing buffer for checksum calculation, if needed.
extern BGCODE_CORE_EXPORT EResult is_valid_binary_gcode(FILE& file, bool check_contents = false, std::byte* cs_buffer = nullptr,
    size_t cs_buffer_size = 0);

// Reads the file header.
// If max_version is not null, version is checked against the passed value.
// If return == EResult::Success:
// - header will contain the file header.
// - file position will be set at the start of the 1st block header.
extern BGCODE_CORE_EXPORT EResult read_header(FILE& file, FileHeader& header, const uint32_t* const max_version);

// Reads next block header from the current file position.
// File position must be at the start of a block header.
// If return == EResult::Success:
// - block_header will contain the header of the block.
// - file position will be set at the start of the block parameters data.
// Caller is responsible for providing buffer for checksum calculation, if needed.
extern BGCODE_CORE_EXPORT EResult read_next_block_header(FILE& file, const FileHeader& file_header, BlockHeader& block_header,
    std::byte* cs_buffer = nullptr, size_t cs_buffer_size = 0);

// Searches and reads next block header with the given type from the current file position.
// File position must be at the start of a block header.
// If return == EResult::Success:
// - block_header will contain the header of the block with the required type.
// - file position will be set at the start of the block parameters data.
// otherwise:
// - file position will keep the current value.
// Caller is responsible for providing buffer for checksum calculation, if needed.
extern BGCODE_CORE_EXPORT EResult read_next_block_header(FILE& file, const FileHeader& file_header, BlockHeader& block_header, EBlockType type,
    std::byte* cs_buffer = nullptr, size_t cs_buffer_size = 0);

// Calculates block checksum and verify it against checksum stored in file.
// Caller is responsible for providing buffer for checksum calculation, bigger buffer means faster calculation and vice versa.
// If return == EResult::Success:
// - file position will be set at the start of the next block header.
extern BGCODE_CORE_EXPORT EResult verify_block_checksum(FILE& file, const FileHeader& file_header, const BlockHeader& block_header, std::byte* buffer,
    size_t buffer_size);

// Skips the content (parameters + data + checksum) of the block with the given block header.
// File position must be at the start of the block parameters.
// If return == EResult::Success:
// - file position will be set at the start of the next block header.
extern BGCODE_CORE_EXPORT EResult skip_block_content(FILE& file, const FileHeader& file_header, const BlockHeader& block_header);

// Skips the block with the given block header.
// File position must be set by a previous call to BlockHeader::write() or BlockHeader::read().
// If return == EResult::Success:
// - file position will be set at the start of the next block header.
extern BGCODE_CORE_EXPORT EResult skip_block(FILE& file, const FileHeader& file_header, const BlockHeader& block_header);

// Returns the size of the parameters of the given block type, in bytes.
extern BGCODE_CORE_EXPORT size_t block_parameters_size(EBlockType type);

// Returns the size of the payload (parameters + data) of the block with the given header, in bytes.
extern BGCODE_CORE_EXPORT size_t block_payload_size(const BlockHeader& block_header);

// Returns the size of the checksum of the given type, in bytes.
extern BGCODE_CORE_EXPORT size_t checksum_size(EChecksumType type);

// Returns the size of the content (parameters + data + checksum) of the block with the given header, in bytes.
extern BGCODE_CORE_EXPORT size_t block_content_size(const FileHeader& file_header, const BlockHeader& block_header);

}} // bgcode::core

#endif // _BGCODE_CORE_HPP_
