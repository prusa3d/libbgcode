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

void binary_to_ascii(const std::string& src_filename, const std::string& dst_filename)
{
    // Open source file
    FILE* src_file;
    int err = fopen_s(&src_file, src_filename.c_str(), "rb");
    REQUIRE(err == 0);
    ScopedFile scoped_src_file(src_file);

    // Open destination file
    FILE* dst_file;
    err = fopen_s(&dst_file, dst_filename.c_str(), "wb");
    REQUIRE(err == 0);
    ScopedFile scoped_dst_file(dst_file);

    // Perform conversion
    EResult res = from_binary_to_ascii(*src_file, *dst_file, true);
    REQUIRE(res == EResult::Success);
}

void compare_files(const std::string& filename1, const std::string& filename2)
{
    // Open file 1
    FILE* file1;
    int err = fopen_s(&file1, filename1.c_str(), "rb");
    REQUIRE(err == 0);
    ScopedFile scoped_file1(file1);

    // Open file 2
    FILE* file2;
    err = fopen_s(&file2, filename2.c_str(), "rb");
    REQUIRE(err == 0);
    ScopedFile scoped_file2(file2);

    // Compare file sizes
    fseek(file1, 0, SEEK_END);
    const long file1_size = ftell(file1);
    rewind(file1);
    fseek(file2, 0, SEEK_END);
    const long file2_size = ftell(file2);
    rewind(file2);
    REQUIRE(file1_size == file2_size);

    // Compare file contents
    static const size_t buf_size = 4096;
    std::vector<uint8_t> buf1(buf_size);
    std::vector<uint8_t> buf2(buf_size);
    do {
        const size_t r1 = fread(buf1.data(), 1, buf_size, file1);
        const size_t r2 = fread(buf2.data(), 1, buf_size, file2);
        REQUIRE(r1 == r2);
        REQUIRE(buf1 == buf2);
    } while (!feof(file1) || !feof(file2));
}

TEST_CASE("Convert from binary to ascii", "[Convert]")
{
    std::cout << "\nTEST: Convert from binary to ascii\n";

    const std::string src_filename = std::string(TEST_DATA_DIR) + "/mini_cube_binary.gcode";
    const std::string dst_filename = std::string(TEST_DATA_DIR) + "/mini_cube_binary_converted.gcode";
    const std::string check_filename = std::string(TEST_DATA_DIR) + "/mini_cube_binary_ascii.gcode";

    // convert from binary to ascii
    binary_to_ascii(src_filename, dst_filename);
    // compare results
    compare_files(dst_filename, check_filename);
}

TEST_CASE("Convert from ascii to binary", "[Convert]")
{
    std::cout << "\nTEST: Convert from ascii to binary\n";
}
