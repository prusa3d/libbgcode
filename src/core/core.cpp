#include "core.hpp"
#include <cstring>

namespace bgcode { namespace core {

static size_t g_checksum_max_cache_size = 65536;

static bool write_to_file(FILE& file, const void* data, size_t data_size)
{
    fwrite(data, 1, data_size, &file);
    return !ferror(&file);
}

static bool read_from_file(FILE& file, void* data, size_t data_size)
{
    const size_t rsize = fread(data, 1, data_size, &file);
    return !ferror(&file) && rsize == data_size;
}

static uint32_t crc32_sw(const uint8_t* buffer, uint32_t length, uint32_t crc)
{
    uint32_t value = crc ^ 0xFFFFFFFF;
    while (length--) {
        value ^= (uint32_t)*buffer++;
        for (int bit = 0; bit < 8; bit++) {
            if (value & 1)
                value = (value >> 1) ^ 0xEDB88320;
            else
                value >>= 1;
        }
    }
    value ^= 0xFFFFFFFF;
    return value;
}

static std::vector<uint8_t> encode(const void* data, size_t data_size)
{
    std::vector<uint8_t> ret(data_size);
    memcpy(ret.data(), data, data_size);
    return ret;
}

static EResult checksums_match(FILE& file, const FileHeader& file_header, const BlockHeader& block_header)
{
    // cache file position
    const long curr_pos = ftell(&file);

    Checksum curr_cs((EChecksumType)file_header.checksum_type);
    // update block checksum block header
    block_header.update_checksum(curr_cs);

    // read block payload
    size_t remaining_payload_size = block_payload_size(block_header);
    while (remaining_payload_size > 0) {
        const size_t size_to_read = std::min(remaining_payload_size, g_checksum_max_cache_size);
        std::vector<uint8_t> payload(size_to_read);
        if (!read_from_file(file, payload.data(), payload.size()))
            return EResult::ReadError;
        curr_cs.append(payload);
        remaining_payload_size -= size_to_read;
    }

    // read checksum
    Checksum read_cs((EChecksumType)file_header.checksum_type);
    EResult res = read_cs.read(file);
    if (res != EResult::Success)
        // propagate error
        return res;

    // Verify checksum
    if (!curr_cs.matches(read_cs))
        return EResult::InvalidChecksum;

    // restore file position
    fseek(&file, curr_pos, SEEK_SET);

    return EResult::Success;
}

static uint16_t checksum_types_count()    { return 1 + (uint16_t)EChecksumType::CRC32; }
static uint16_t block_types_count()       { return 1 + (uint16_t)EBlockType::Thumbnail; }
static uint16_t compression_types_count() { return 1 + (uint16_t)ECompressionType::Heatshrink_12_4; }

Checksum::Checksum(EChecksumType type)
: m_type(type)
{
    if (m_type != EChecksumType::None)
        m_checksum = std::vector<uint8_t>(checksum_size(m_type), '\0');
}

EChecksumType Checksum::get_type() const { return m_type; }

void Checksum::append(const std::vector<uint8_t>& data)
{
    size_t remaining_data_size = std::distance(data.begin(), data.end());
    auto it_begin = data.begin();
    while (remaining_data_size + m_cache.size() > g_checksum_max_cache_size) {
        update();
        if (remaining_data_size > g_checksum_max_cache_size) {
            m_cache.insert(m_cache.end(), it_begin, it_begin + g_checksum_max_cache_size);
            it_begin += g_checksum_max_cache_size;
            remaining_data_size -= g_checksum_max_cache_size;
        }
    }

    m_cache.insert(m_cache.end(), it_begin, data.end());
}

bool Checksum::matches(Checksum& other)
{
    update();
    other.update();
    return m_checksum == other.m_checksum;
}

EResult Checksum::write(FILE& file)
{
    if (m_type != EChecksumType::None) {
        update();
        if (!write_to_file(file, (const void*)m_checksum.data(), m_checksum.size()))
            return EResult::WriteError;
    }
    return EResult::Success;
}

EResult Checksum::read(FILE& file)
{
    if (m_type != EChecksumType::None) {
        if (!read_from_file(file, (void*)m_checksum.data(), m_checksum.size()))
            return EResult::ReadError;
    }
    return EResult::Success;
}

void Checksum::update()
{
    if (m_cache.empty())
      return;

    switch (m_type)
    {
    case EChecksumType::None:
    {
        break;
    }
    case EChecksumType::CRC32:
    {
        const uint32_t old_crc = *(uint32_t*)m_checksum.data();
        const uint32_t new_crc = crc32_sw(m_cache.data(), (uint32_t)m_cache.size(), old_crc);
        *(uint32_t*)m_checksum.data() = new_crc;
        break;
    }
    }

    m_cache.clear();
}

EResult FileHeader::write(FILE& file) const
{
    if (magic != *(uint32_t*)(MAGIC.data()))
        return EResult::InvalidMagicNumber;
    if (checksum_type >= checksum_types_count())
        return EResult::InvalidChecksumType;

    if (!write_to_file(file, (const void*)&magic, sizeof(magic)))
       return EResult::WriteError;
    if (!write_to_file(file, (const void*)&version, sizeof(version)))
        return EResult::WriteError;
    if (!write_to_file(file, (const void*)&checksum_type, sizeof(checksum_type)))
        return EResult::WriteError;

    return EResult::Success;
}

EResult FileHeader::read(FILE& file, const uint32_t* const max_version)
{
    if (!read_from_file(file, (void*)&magic, sizeof(magic)))
        return EResult::ReadError;
    if (magic != *(uint32_t*)(MAGIC.data()))
        return EResult::InvalidMagicNumber;

    if (!read_from_file(file, (void*)&version, sizeof(version)))
        return EResult::ReadError;
    if (max_version != nullptr && version > *max_version)
        return EResult::InvalidVersionNumber;

    if (!read_from_file(file, (void*)&checksum_type, sizeof(checksum_type)))
        return EResult::ReadError;
    if (checksum_type >= checksum_types_count())
        return EResult::InvalidChecksumType;

    return EResult::Success;
}

void BlockHeader::update_checksum(Checksum& checksum) const
{
    checksum.append(encode((const void*)&type, sizeof(type)));
    checksum.append(encode((const void*)&compression, sizeof(compression)));
    checksum.append(encode((const void*)&uncompressed_size, sizeof(uncompressed_size)));
    if (compression != (uint16_t)ECompressionType::None)
        checksum.append(encode((const void*)&compressed_size, sizeof(compressed_size)));
}

EResult BlockHeader::write(FILE& file) const
{
    if (!write_to_file(file, (const void*)&type, sizeof(type)))
        return EResult::WriteError;
    if (!write_to_file(file, (const void*)&compression, sizeof(compression)))
        return EResult::WriteError;
    if (!write_to_file(file, (const void*)&uncompressed_size, sizeof(uncompressed_size)))
        return EResult::WriteError;
    if (compression != (uint16_t)ECompressionType::None) {
        if (!write_to_file(file, (const void*)&compressed_size, sizeof(compressed_size)))
            return EResult::WriteError;
    }
    return EResult::Success;
}

EResult BlockHeader::read(FILE& file)
{
    if (!read_from_file(file, (void*)&type, sizeof(type)))
        return EResult::ReadError;
    if (type >= block_types_count())
        return EResult::InvalidBlockType;

    if (!read_from_file(file, (void*)&compression, sizeof(compression)))
        return EResult::ReadError;
    if (compression >= compression_types_count())
        return EResult::InvalidCompressionType;

    if (!read_from_file(file, (void*)&uncompressed_size, sizeof(uncompressed_size)))
        return EResult::ReadError;
    if (compression != (uint16_t)ECompressionType::None) {
        if (!read_from_file(file, (void*)&compressed_size, sizeof(compressed_size)))
            return EResult::ReadError;
    }

    return EResult::Success;
}

BGCODE_CORE_EXPORT size_t get_checksum_max_cache_size() { return g_checksum_max_cache_size; }
BGCODE_CORE_EXPORT void set_checksum_max_cache_size(size_t size) { g_checksum_max_cache_size = size; }

BGCODE_CORE_EXPORT std::string_view translate_result(EResult result)
{
    using namespace std::literals;
    switch (result)
    {
    case EResult::Success:                     { return "Success"sv; }
    case EResult::ReadError:                   { return "Read error"sv; }
    case EResult::WriteError:                  { return "Write error"sv; }
    case EResult::InvalidMagicNumber:          { return "Invalid magic number"sv; }
    case EResult::InvalidVersionNumber:        { return "Invalid version number"sv; }
    case EResult::InvalidChecksumType:         { return "Invalid checksum type"sv; }
    case EResult::InvalidBlockType:            { return "Invalid block type"sv; }
    case EResult::InvalidCompressionType:      { return "Invalid compression type"sv; }
    case EResult::InvalidMetadataEncodingType: { return "Invalid metadata encoding type"sv; }
    case EResult::InvalidGCodeEncodingType:    { return "Invalid gcode encoding type"sv; }
    case EResult::DataCompressionError:        { return "Data compression error"sv; }
    case EResult::DataUncompressionError:      { return "Data uncompression error"sv; }
    case EResult::MetadataEncodingError:       { return "Metadata encoding error"sv; }
    case EResult::MetadataDecodingError:       { return "Metadata decoding error"sv; }
    case EResult::GCodeEncodingError:          { return "GCode encoding error"sv; }
    case EResult::GCodeDecodingError:          { return "GCode decoding error"sv; }
    case EResult::BlockNotFound:               { return "Block not found"sv; }
    case EResult::InvalidChecksum:             { return "Invalid checksum"sv; }
    case EResult::InvalidThumbnailFormat:      { return "Invalid thumbnail format"sv; }
    case EResult::InvalidThumbnailWidth:       { return "Invalid thumbnail width"sv; }
    case EResult::InvalidThumbnailHeight:      { return "Invalid thumbnail height"sv; }
    case EResult::InvalidThumbnailDataSize:    { return "Invalid thumbnail data size"sv; }
    case EResult::InvalidBinaryGCodeFile:      { return "Invalid binary GCode file"sv; }
    case EResult::InvalidAsciiGCodeFile:       { return "Invalid ascii GCode file"sv; }
    case EResult::InvalidSequenceOfBlocks:     { return "Invalid sequence of blocks"sv; }
    case EResult::AlreadyBinarized:            { return "Already binarized"sv; }
    }
    return std::string_view();
}

BGCODE_CORE_EXPORT EResult is_valid_binary_gcode(FILE& file, bool check_contents)
{
    // cache file position
    const long curr_pos = ftell(&file);
    rewind(&file);

    // check magic number
    std::array<uint8_t, 4> magic;
    const size_t rsize = fread((void*)magic.data(), 1, magic.size(), &file);
    if (ferror(&file) && rsize != magic.size())
        return EResult::ReadError;
    else if (magic != MAGIC) {
        // restore file position
        fseek(&file, curr_pos, SEEK_SET);
        return EResult::InvalidMagicNumber;
    }

    // check contents
    if (check_contents) {
        fseek(&file, 0, SEEK_END);
        const long file_size = ftell(&file);
        rewind(&file);

        // read header
        FileHeader file_header;
        EResult res = read_header(file, file_header, nullptr);
        if (res != EResult::Success) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            // propagate error
            return res;
        }
        BlockHeader block_header;
        // read file metadata block header
        res = read_next_block_header(file, file_header, block_header, false);
        if (res != EResult::Success) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            // propagate error
            return res;
        }
        if ((EBlockType)block_header.type != EBlockType::FileMetadata) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            return EResult::InvalidBlockType;
        }

        // read printer metadata block header
        res = skip_block_content(file, file_header, block_header);
        if (res != EResult::Success) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            // propagate error
            return res;
        }
        res = read_next_block_header(file, file_header, block_header, false);
        if (res != EResult::Success) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            // propagate error
            return res;
        }
        if ((EBlockType)block_header.type != EBlockType::PrinterMetadata) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            return EResult::InvalidBlockType;
        }

        // read thumbnails block headers, if present
        res = skip_block_content(file, file_header, block_header);
        if (res != EResult::Success) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            // propagate error
            return res;
        }
        res = read_next_block_header(file, file_header, block_header, false);
        if (res != EResult::Success) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            // propagate error
            return res;
        }
        while ((EBlockType)block_header.type == EBlockType::Thumbnail) {
            res = skip_block_content(file, file_header, block_header);
            if (res != EResult::Success) {
                // restore file position
                fseek(&file, curr_pos, SEEK_SET);
                // propagate error
                return res;
            }
            res = read_next_block_header(file, file_header, block_header, false);
            if (res != EResult::Success) {
                // restore file position
                fseek(&file, curr_pos, SEEK_SET);
                // propagate error
                return res;
            }
        }

        // read print metadata block header
        if ((EBlockType)block_header.type != EBlockType::PrintMetadata) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            return EResult::InvalidBlockType;
        }

        // read slicer metadata block header
        res = skip_block_content(file, file_header, block_header);
        if (res != EResult::Success) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            // propagate error
            return res;
        }
        res = read_next_block_header(file, file_header, block_header, false);
        if (res != EResult::Success) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            // propagate error
            return res;
        }
        if ((EBlockType)block_header.type != EBlockType::SlicerMetadata) {
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            return EResult::InvalidBlockType;
        }

        // read gcode block headers
        do {
            res = skip_block_content(file, file_header, block_header);
            if (res != EResult::Success) {
                // restore file position
                fseek(&file, curr_pos, SEEK_SET);
                // propagate error
                return res;
            }
            if (ftell(&file) == file_size)
                break;
            res = read_next_block_header(file, file_header, block_header, false);
            if (res != EResult::Success) {
                // restore file position
                fseek(&file, curr_pos, SEEK_SET);
                // propagate error
                return res;
            }
            if ((EBlockType)block_header.type != EBlockType::GCode) {
                // restore file position
                fseek(&file, curr_pos, SEEK_SET);
                return EResult::InvalidBlockType;
            }
        } while (!feof(&file));
    }

    fseek(&file, curr_pos, SEEK_SET);
    return EResult::Success;
}

BGCODE_CORE_EXPORT EResult read_header(FILE& file, FileHeader& header, const uint32_t* const max_version)
{
    rewind(&file);
    return header.read(file, max_version);
}

BGCODE_CORE_EXPORT EResult read_next_block_header(FILE& file, const FileHeader& file_header, BlockHeader& block_header, bool verify_checksum)
{
    if (verify_checksum && (EChecksumType)file_header.checksum_type != EChecksumType::None) {
        const EResult res = block_header.read(file);
        if (res != EResult::Success)
            // propagate error
            return res;

        return checksums_match(file, file_header, block_header);
    }
    else
        return block_header.read(file);
}

BGCODE_CORE_EXPORT EResult read_next_block_header(FILE& file, const FileHeader& file_header, BlockHeader& block_header, EBlockType type,
    bool verify_checksum)
{
    // cache file position
    const long curr_pos = ftell(&file);

    do {
        EResult res = read_next_block_header(file, file_header, block_header, false);
        if (res != EResult::Success)
            // propagate error
            return res;
        else if (feof(&file)) {
            // block not found
            // restore file position
            fseek(&file, curr_pos, SEEK_SET);
            return EResult::BlockNotFound;
        }
        else if ((EBlockType)block_header.type == type) {
            // block found
            if (verify_checksum) {
                res = checksums_match(file, file_header, block_header);
                if (res != EResult::Success)
                    // propagate error
                    return res;
                else
                    break;
            }
        }

        if (!feof(&file)) {
            res = skip_block_content(file, file_header, block_header);
            if (res != EResult::Success)
                // propagate error
                return res;
        }
    } while (true);

    return EResult::Success;
}

BGCODE_CORE_EXPORT size_t block_parameters_size(EBlockType type)
{
    switch (type)
    {
    case EBlockType::FileMetadata:    { return sizeof(uint16_t); } /* encoding_type */
    case EBlockType::GCode:           { return sizeof(uint16_t); } /* encoding_type */
    case EBlockType::SlicerMetadata:  { return sizeof(uint16_t); } /* encoding_type */
    case EBlockType::PrinterMetadata: { return sizeof(uint16_t); } /* encoding_type */
    case EBlockType::PrintMetadata:   { return sizeof(uint16_t); } /* encoding_type */
    case EBlockType::Thumbnail:       { return sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t); } /* format, width, height */
    }
    return 0;
}

BGCODE_CORE_EXPORT EResult skip_block_payload(FILE& file, const BlockHeader& block_header)
{
    fseek(&file, (long)block_payload_size(block_header), SEEK_CUR);
    return ferror(&file) ? EResult::ReadError : EResult::Success;
}

BGCODE_CORE_EXPORT EResult skip_block_content(FILE& file, const FileHeader& file_header, const BlockHeader& block_header)
{
    fseek(&file, (long)block_content_size(file_header, block_header), SEEK_CUR);
    return ferror(&file) ? EResult::ReadError : EResult::Success;
}

BGCODE_CORE_EXPORT size_t block_payload_size(const BlockHeader& block_header)
{
    size_t ret = block_parameters_size((EBlockType)block_header.type);
    ret += ((ECompressionType)block_header.compression == ECompressionType::None) ?
        block_header.uncompressed_size : block_header.compressed_size;
    return ret;
}

BGCODE_CORE_EXPORT size_t checksum_size(EChecksumType type)
{
  switch (type)
  {
  case EChecksumType::None: { return 0; }
  case EChecksumType::CRC32: { return 4; }
  }
  return 0;
}

BGCODE_CORE_EXPORT size_t block_content_size(const FileHeader& file_header, const BlockHeader& block_header)
{
    return block_payload_size(block_header) + checksum_size((EChecksumType)file_header.checksum_type);
}

} // namespace core
} // namespace bgcode
