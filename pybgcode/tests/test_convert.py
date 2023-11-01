import pybgcode
import filecmp
import io

def get_thumbnail_extension(format_id):
    ret = "unknown"
    if format_id == int(pybgcode.EThumbnailFormat.PNG):
        ret = "png"
    elif format_id == int(pybgcode.EThumbnailFormat.JPG):
        ret = "jpg"
    elif format_id == int(pybgcode.EThumbnailFormat.QOI):
        ret = "qoi"

    return ret

def read_thumbnails(gcodefile):
    assert(pybgcode.is_open(gcodefile))
    pybgcode.rewind(gcodefile)

    header = pybgcode.FileHeader()
    res = pybgcode.read_header(gcodefile, header)
    if res != pybgcode.EResult.Success:
        raise Exception(pybgcode.translate_result(res))

    block = pybgcode.BlockHeader()
    thumbnails = []
    while res == pybgcode.EResult.Success:
        res = pybgcode.read_next_block_header(gcodefile, header, block, pybgcode.EBlockType.Thumbnail)
        if res != pybgcode.EResult.Success:
            break

        thumbnail_block = pybgcode.ThumbnailBlock()
        res = thumbnail_block.read_data(gcodefile, header, block)
        if res != pybgcode.EResult.Success:
            raise Exception(pybgcode.translate_result(res))

        thumbnails.append({"meta": thumbnail_block.params, "bytes": thumbnail_block.data()})

    return thumbnails


def test_main():
    assert(pybgcode.__version__ == pybgcode.version())

    in_f  = pybgcode.open("test.gcode", "r");
    out_f = pybgcode.open("test.bgcode", "wb");

    assert(in_f)
    assert(out_f)

    assert(pybgcode.is_open(in_f))
    assert(pybgcode.is_open(out_f))

    cfg = pybgcode.get_config();
    cfg.compression.file_metadata = pybgcode.CompressionType.Heatshrink_11_4
    res = pybgcode.from_ascii_to_binary(in_f, out_f, cfg);

    assert(res == pybgcode.EResult.Success)

    pybgcode.close(out_f)
    pybgcode.close(in_f)

    thumb_f = pybgcode.open("test.bgcode", "rb")
    thumbnails = read_thumbnails(thumb_f)
    assert(len(thumbnails) == 2)
    pybgcode.close(thumb_f)

    # write thumbnails to png files
    thumcnt = 0;
    for thumb in thumbnails:
        with open("thumb" + str(thumcnt) + "." + get_thumbnail_extension(thumb["meta"].format), "wb") as outpng:
            outpng.write(thumb["bytes"])
            thumcnt = thumcnt + 1

    in_f = pybgcode.open("test.bgcode", "rb");
    out_f = pybgcode.open("test_reverse.gcode", "w");

    res = pybgcode.from_binary_to_ascii(in_f, out_f, True)
    assert(res == pybgcode.EResult.Success)
    
    pybgcode.close(out_f)

    assert(filecmp.cmp("test.gcode", "test_reverse.gcode", shallow=False))

if __name__ == '__main__':
    test_main()
