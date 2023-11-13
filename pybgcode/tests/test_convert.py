"""Convert test functions."""
import filecmp

import pytest

import pybgcode
from pybgcode import (
    EResult,
    EThumbnailFormat,
    read_thumbnails,
)

# pylint: disable=missing-function-docstring
TEST_GCODE = "test.gcode"
TEST_REVERSE_GCODE = "test_reverse.gcode"
TEST_BGCODE = "test.bgcode"
TEST_THUMBNAILS = 2


def get_thumbnail_extension(format_id):
    ret = "unknown"
    if format_id == int(EThumbnailFormat.PNG):
        ret = "png"
    elif format_id == int(EThumbnailFormat.JPG):
        ret = "jpg"
    elif format_id == int(EThumbnailFormat.QOI):
        ret = "qoi"

    return ret


def test_main():
    assert pybgcode.__version__ == pybgcode.version()

    in_f = pybgcode.open(TEST_GCODE, "r")
    out_f = pybgcode.open(TEST_BGCODE, "wb")

    assert in_f
    assert out_f

    assert pybgcode.is_open(in_f)
    assert pybgcode.is_open(out_f)

    cfg = pybgcode.get_config()
    cfg.compression.file_metadata = pybgcode.CompressionType.Heatshrink_11_4
    res = pybgcode.from_ascii_to_binary(in_f, out_f, cfg)

    assert res == EResult.Success

    pybgcode.close(out_f)
    pybgcode.close(in_f)

    thumb_f = pybgcode.open(TEST_BGCODE, "rb")
    thumbnails = read_thumbnails(thumb_f)
    assert len(thumbnails) == TEST_THUMBNAILS
    pybgcode.close(thumb_f)

    # write thumbnails to png files
    thumcnt = 0
    for thumb in thumbnails:
        with open("thumb" + str(thumcnt) + "." +
                  get_thumbnail_extension(thumb["meta"].format), "wb") as out:
            out.write(thumb["bytes"])
            thumcnt = thumcnt + 1

    in_f = pybgcode.open(TEST_BGCODE, "rb")
    out_f = pybgcode.open(TEST_REVERSE_GCODE, "w")

    res = pybgcode.from_binary_to_ascii(in_f, out_f, True)
    assert res == EResult.Success

    pybgcode.close(out_f)

    assert filecmp.cmp(TEST_GCODE, TEST_REVERSE_GCODE, shallow=False)


if __name__ == "__main__":
    pytest.main()
