import pybgcode

def test_main():
    # print(pybgcode.__version__)
    # assert pybgcode.__version__ == "0.0.1"

    in_f = pybgcode.fopen("mini_cube_b_ref.gcode", "r");
    out_f = pybgcode.fopen("mini_cube_b_ref.bgcode", "w");

    assert(in_f)
    assert(out_f)

    cfg = pybgcode.get_config();
    cfg.compression.file_metadata = pybgcode.BGCode_CompressionType.Heatshrink_11_4
    res = pybgcode.from_ascii_to_binary(in_f, out_f, cfg);
    print(res)

    assert(res == pybgcode.EResult.Success)

    pybgcode.fclose(in_f)
    pybgcode.fclose(out_f)

if __name__ == '__main__':
    test_main()