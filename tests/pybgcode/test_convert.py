import pybgcode
import ctypes

def test_main():
    print(pybgcode.__version__)
    # assert pybgcode.__version__ == "0.0.1"

    in_f = open("/home/quarky/Workspace/prusa3d/libbingcode/libbgcode-src-master/tests/data/mini_cube_b_ref.gcode", "r");
    out_f = open("mini_cube_b_ref.bgcode", "w");
    
    assert(in_f)
    assert(out_f)
    res = pybgcode.from_ascii_to_binary(ctypes.convert_file(in_f), ctypes.convert_file(out_f));
    print(res)

if __name__ == '__main__':
    test_main()