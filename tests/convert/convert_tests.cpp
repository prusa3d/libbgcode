#include <catch_main.hpp>

#include "convert/convert.hpp"

using namespace bgcode::core;
using namespace bgcode::convert;

class ScopedFile
{
public:
    explicit ScopedFile(FILE* file) : m_file(file) {}
    ~ScopedFile() { if (m_file != nullptr) fclose(m_file); }
private:
    FILE* m_file{ nullptr };
};

// Does not build on Linux
//TEST_CASE("Convert from binary to ascii", "[Convert]")
//{
//    std::cout << "\nTEST: Convert from binary to ascii\n";

//    const std::string src_filename = std::string(TEST_DATA_DIR) + "/mini_cube_binary.gcode";
//    const std::string dst_filename = std::string(TEST_DATA_DIR) + "/mini_cube_binary_converted.gcode";
//    const std::string check_filename = std::string(TEST_DATA_DIR) + "/mini_cube_binary_ascii.gcode";

//    // Open source file
//    FILE* src_file;
//    errno_t err = fopen_s(&src_file, src_filename.c_str(), "rb");
//    REQUIRE(err == 0);
//    ScopedFile scoped_src_file(src_file);

//    // Open destination file
//    FILE* dst_file;
//    err = fopen_s(&dst_file, dst_filename.c_str(), "w+b");
//    REQUIRE(err == 0);
//    ScopedFile scoped_dst_file(dst_file);

//    // Perform conversion
//    EResult res = from_binary_to_ascii(*src_file, *dst_file, true);
//    REQUIRE(res == EResult::Success);

//    // Open check file
//    FILE* check_file;
//    err = fopen_s(&check_file, check_filename.c_str(), "rb");
//    REQUIRE(err == 0);
//    ScopedFile scoped_check_file(check_file);

//    // Compare file sizes
//    fseek(dst_file, 0, SEEK_END);
//    const long dst_file_size = ftell(dst_file);
//    rewind(dst_file);
//    fseek(check_file, 0, SEEK_END);
//    const long check_file_size = ftell(check_file);
//    rewind(check_file);
//    REQUIRE(dst_file_size == check_file_size);

//    // Compare file contents
//    static const size_t buf_size = 4096;
//    std::vector<uint8_t> dst_buf(buf_size);
//    std::vector<uint8_t> check_buf(buf_size);
//    do {
//        const size_t dst_r = fread(dst_buf.data(), 1, buf_size, dst_file);
//        const size_t check_r = fread(check_buf.data(), 1, buf_size, check_file);
//        REQUIRE(dst_r == check_r);
//        REQUIRE(dst_buf == check_buf);
//    } while (!feof(dst_file) || !feof(check_file));
//}

TEST_CASE("Convert from ascii to binary", "[Convert]")
{
    std::cout << "\nTEST: Convert from ascii to binary\n";
}
