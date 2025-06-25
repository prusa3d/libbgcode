#include <catch2/catch_test_macros.hpp>

#include "binarize/binarize.hpp"

#include <boost/nowide/cstdio.hpp>

#include <iostream>

using namespace bgcode::core;

class ScopedFile
{
public:
    explicit ScopedFile(FILE* file) : m_file(file) {}
    ~ScopedFile() { if (m_file != nullptr) fclose(m_file); }
private:
    FILE* m_file{ nullptr };
};

TEST_CASE("Extract thumbnail 3d", "[Binarize]")
{
    const std::string filename = std::string(TEST_DATA_DIR) + "/thumb3d.bgcode";
    const std::string glb_filename = std::string(TEST_DATA_DIR) + "/thumb3d.glb";
    std::cout << "\nTEST: Extract thumbnail 3d\n";
    std::cout << "File:" << filename << "\n";

    const size_t MAX_CHECKSUM_CACHE_SIZE = 2048;
    std::byte checksum_verify_buffer[MAX_CHECKSUM_CACHE_SIZE];

    FILE* file = boost::nowide::fopen(filename.c_str(), "rb");
    REQUIRE(file != nullptr);
    ScopedFile scoped_file(file);
    REQUIRE(is_valid_binary_gcode(*file, true, checksum_verify_buffer, sizeof(checksum_verify_buffer)) == EResult::Success);

    fseek(file, 0, SEEK_END);
    const long file_size = ftell(file);
    rewind(file);

    FileHeader file_header;
    REQUIRE(read_header(*file, file_header, nullptr) == EResult::Success);

    BlockHeader block_header;

    do
    {
        // search and read block header by type
        REQUIRE(read_next_block_header(*file, file_header, block_header, EBlockType::Thumbnail3d, checksum_verify_buffer, sizeof(checksum_verify_buffer)) == EResult::Success);

        if ((EBlockType)block_header.type == EBlockType::Thumbnail3d) {
            bgcode::binarize::Thumbnail3dBlock thumbnail3d_block;
            REQUIRE(thumbnail3d_block.read_data(*file, file_header, block_header) == EResult::Success);

            FILE* glb_file = boost::nowide::fopen(glb_filename.c_str(), "wb");
            REQUIRE(glb_file != nullptr);
            ScopedFile scoped_glb_file(glb_file);

            REQUIRE(fwrite(thumbnail3d_block.data.data(), 1, thumbnail3d_block.data.size(), glb_file) == thumbnail3d_block.data.size());
            break;
        }

        // move to next block header
        REQUIRE(skip_block(*file, file_header, block_header) == EResult::Success);
        if (ftell(file) == file_size)
            break;
    } while (true);
}
