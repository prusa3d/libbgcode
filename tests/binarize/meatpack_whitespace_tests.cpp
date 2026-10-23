#include <catch2/catch_test_macros.hpp>

#include "binarize/meatpack.hpp"

#include <cstdint>
#include <string>
#include <vector>

static std::vector<uint8_t> binarize_gcode_line(const std::string& gcode_line, uint8_t meatpack_flags)
{
    MeatPack::MPBinarizer binarizer(meatpack_flags);
    std::vector<uint8_t> binarized_data;
    binarizer.initialize(binarized_data);
    REQUIRE(binarizer.binarize_line(gcode_line, binarized_data));
    binarizer.finalize(binarized_data);
    return binarized_data;
}

static std::string unbinarize_to_text(const std::vector<uint8_t>& binarized_data)
{
    std::string text;
    MeatPack::unbinarize(binarized_data, text);
    return text;
}

static uint8_t four_bit_code_in_no_spaces_mode(char character)
{
    const size_t index = std::string("0123456789.E\nGX").find(character);
    return index == std::string::npos ? 0xF : static_cast<uint8_t>(index);
}

// Written according to the MeatPack specification, independently of the library.
static std::vector<uint8_t> expected_stream(const std::vector<std::string>& packed_lines)
{
    constexpr uint8_t EnablePackingCommand = 251;
    constexpr uint8_t EnableNoSpacesCommand = 247;

    std::vector<uint8_t> stream = { 0xFF, 0xFF, EnablePackingCommand, 0xFF, 0xFF, EnableNoSpacesCommand };
    for (const std::string& packed_line : packed_lines) {
        for (size_t i = 0; i < packed_line.size(); i += 2) {
            const char first = packed_line[i];
            const char second = (i + 1 < packed_line.size()) ? packed_line[i + 1] : '\n';
            const uint8_t first_code = four_bit_code_in_no_spaces_mode(first);
            const uint8_t second_code = four_bit_code_in_no_spaces_mode(second);

            stream.push_back(static_cast<uint8_t>((second_code << 4) | first_code));

            if (first_code == 0xF) {
                stream.push_back(static_cast<uint8_t>(first));
            }

            if (second_code == 0xF) {
                stream.push_back(static_cast<uint8_t>(second));
            }
        }
    }

    return stream;
}

struct CompactionCase
{
    std::string gcode_line;
    std::string packed_line;
    std::string packed_by_previous_versions;
    std::string decoded_from_previous_versions;
};

static const std::vector<CompactionCase> compaction_cases = {
    // gcode line                Packed                     Previous versions          Decoded from previous versions
    { "M104 S200\n",             "M104 S200\n",             "M104 S200\n",             "M104 S200\n" },
    { "G1 X10 Y10 E1.5 F3000\n", "G1X10Y10E1.5F3000\n\n",   "G1X10Y10E1.5F3000\n\n",   "G1 X10 Y10 E1.5 F3000\n" },
    { "G01 X1 Y2\n",             "G01X1Y2\n\n",             "G01X1Y2\n\n",             "G01 X1 Y2\n" },
    { "G2 X10 Y10 I5 J5\n",      "G2X10Y10I5J5\n\n",        "G2X10Y10I5J5\n\n",        "G2 X10 Y10 I5 J5\n" },
    { "G10\n",                   "G10\n",                   "G10\n\n",                 "G10\n" },
    { "G28 W\n",                 "G28 W\n",                 "G28W\n\n",                "G28 W\n" },
    { "G28 XY\n",                "G28 XY\n",                "G28XY\n\n",               "G28 X Y\n" },
    { "N5 G1 X10\n",             "N5 G1 X10\n",             "N5G1X10\n\n",             "N5G1X10\n" },
    { "G12 P1 S3 T5\n",          "G12 P1 S3 T5\n",          "G12P1S3T5\n\n",           "G12 P1 S3T5\n" },
    { "G12 Quick clean\n",       "G12 Quick clean\n",       "G12QuickclEan\n\n",       "G12Quickcl Ean\n" },
    { "G12 Quick_Clean\n",       "G12 Quick_Clean\n",       "G12Quick_ClEan\n\n",      "G12Quick_ Cl Ean\n" },
    { "G12 QUICK STOP\n",        "G12 QUICK STOP\n",        "G12QUICKSTOP\n\n",        "G12QU I CK STO P\n" },
    { "G FS\n",                  "G FS\n",                  "G FS\n",                  "G FS\n" },
    { "g1 x10 y10\n",            "g1 x10 y10\n",            "g1 x10 y10\n",            "g1 x10 y10\n" },
};

TEST_CASE("MeatPack compacts only G0-G3 lines", "[Binarize][MeatPack]")
{
    for (const CompactionCase& compaction_case : compaction_cases) {
        CAPTURE(compaction_case.gcode_line);
        CHECK(binarize_gcode_line(compaction_case.gcode_line, MeatPack::Flag_OmitWhitespaces) == expected_stream({ compaction_case.packed_line }));
    }
}

TEST_CASE("MeatPack decodes every line back exactly", "[Binarize][MeatPack]")
{
    std::vector<std::string> packed_lines;
    std::string gcode_lines;
    for (const CompactionCase& compaction_case : compaction_cases) {
        CAPTURE(compaction_case.gcode_line);
        CHECK(unbinarize_to_text(binarize_gcode_line(compaction_case.gcode_line, MeatPack::Flag_OmitWhitespaces)) == compaction_case.gcode_line);

        packed_lines.push_back(compaction_case.packed_line);
        gcode_lines += compaction_case.gcode_line;
    }

    CHECK(unbinarize_to_text(expected_stream(packed_lines)) == gcode_lines);
}

TEST_CASE("MeatPack decodes streams of previous versions as before", "[Binarize][MeatPack]")
{
    std::vector<std::string> packed_lines;
    std::string decoded_lines;
    for (const CompactionCase& compaction_case : compaction_cases) {
        CAPTURE(compaction_case.gcode_line);
        CHECK(unbinarize_to_text(expected_stream({ compaction_case.packed_by_previous_versions })) == compaction_case.decoded_from_previous_versions);

        packed_lines.push_back(compaction_case.packed_by_previous_versions);
        decoded_lines += compaction_case.decoded_from_previous_versions;
    }

    CHECK(unbinarize_to_text(expected_stream(packed_lines)) == decoded_lines);
}
