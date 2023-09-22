import pybgcode
import filecmp

def test_main():
    assert(pybgcode.__version__ == "0.1")

    in_f  = pybgcode.open("test.gcode", "r");
    out_f = pybgcode.open("test.bgcode", "w");

    assert(in_f)
    assert(out_f)

    assert(pybgcode.is_open(in_f))
    assert(pybgcode.is_open(out_f))

    cfg = pybgcode.get_config();
    cfg.compression.file_metadata = pybgcode.BGCode_CompressionType.Heatshrink_11_4
    res = pybgcode.from_ascii_to_binary(in_f, out_f, cfg);

    assert(res == pybgcode.EResult.Success)

    pybgcode.close(out_f)
    pybgcode.close(in_f)

    in_f = pybgcode.open("test.bgcode", "r");
    out_f = pybgcode.open("test_reverse.gcode", "w");

    res = pybgcode.from_binary_to_ascii(in_f, out_f, True)
    assert(res == pybgcode.EResult.Success)

    pybgcode.close(out_f)

    assert(filecmp.cmp("test.gcode", "test_reverse.gcode", shallow=False))

if __name__ == '__main__':
    test_main()