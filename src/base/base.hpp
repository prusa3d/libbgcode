#ifndef BGCODE_BASE_HPP
#define BGCODE_BASE_HPP

#include "base/export.h"
#include "core/core.hpp"

namespace bgcode { namespace base {

struct BaseMetadataBlock
{
    // type of data encoding
    uint16_t encoding_type{ 0 };
    // data in key/value form
    std::vector<std::pair<std::string, std::string>> raw_data;

    // write block header and data in encoded format
    core::EResult write(FILE& file, core::EBlockType block_type, core::ECompressionType compression_type, core::Checksum& checksum) const;
    // read block data in encoded format
    core::EResult read_data(FILE& file, const core::BlockHeader& block_header);
};

struct FileMetadataBlock : public BaseMetadataBlock
{
    // write block header and data
    core::EResult write(FILE& file, core::ECompressionType compression_type, core::EChecksumType checksum_type) const;
    // read block data
    core::EResult read_data(FILE& file, const core::FileHeader& file_header, const core::BlockHeader& block_header);
};

struct PrintMetadataBlock : public BaseMetadataBlock
{
    // write block header and data
    core::EResult write(FILE& file, core::ECompressionType compression_type, core::EChecksumType checksum_type) const;
    // read block data
    core::EResult read_data(FILE& file, const core::FileHeader& file_header, const core::BlockHeader& block_header);
};

struct PrinterMetadataBlock : public BaseMetadataBlock
{
    // write block header and data
    core::EResult write(FILE& file, core::ECompressionType compression_type, core::EChecksumType checksum_type) const;
    // read block data
    core::EResult read_data(FILE& file, const core::FileHeader& file_header, const core::BlockHeader& block_header);
};

struct ThumbnailBlock
{
    uint16_t format{ 0 };
    uint16_t width{ 0 };
    uint16_t height{ 0 };
    std::vector<uint8_t> data;

    // write block header and data
    core::EResult write(FILE& file, core::EChecksumType checksum_type) const;
    // read block data
    core::EResult read_data(FILE& file, const core::FileHeader& file_header, const core::BlockHeader& block_header);

private:
    void update_checksum(core::Checksum& checksum) const;
};

struct GCodeBlock
{
    uint16_t encoding_type{ 0 };
    std::string raw_data;

    // write block header and data
    core::EResult write(FILE& file, core::ECompressionType compression_type, core::EChecksumType checksum_type) const;
    // read block data
    core::EResult read_data(FILE& file, const core::FileHeader& file_header, const core::BlockHeader& block_header);
};

struct SlicerMetadataBlock : public BaseMetadataBlock
{
    // write block header and data
    core::EResult write(FILE& file, core::ECompressionType compression_type, core::EChecksumType checksum_type) const;
    // read block data
    core::EResult read_data(FILE& file, const core::FileHeader& file_header, const core::BlockHeader& block_header);
};

}} // bgcode::core

#endif // BGCODE_BASE_HPP
