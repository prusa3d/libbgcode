"""Prusa Block & Binary G-code reader / writer / converter"""
# pylint: disable=redefined-builtin
from ._bgcode import (  # type: ignore
    BlockHeader,
    CompressionType,
    EBlockType,
    EResult,
    EThumbnailFormat,
    FileHeader,
    FILEWrapper,
    ThumbnailBlock,
    close,
    from_ascii_to_binary,
    from_binary_to_ascii,
    get_config,
    is_open,
    open,
    read_header,
    read_next_block_header,
    rewind,
    translate_result,
    version,
)

__version__ = version()

__all__ = [
        "BlockHeader",
        "CompressionType",
        "EBlockType",
        "EResult",
        "EThumbnailFormat",
        "FileHeader",
        "ThumbnailBlock",
        "close",
        "from_ascii_to_binary",
        "from_binary_to_ascii",
        "get_config",
        "is_open",
        "open",
        "read_header",
        "read_next_block_header",
        "rewind",
        "translate_result"]


class ResultError(Exception):
    """Exception based on EResult."""

    def __init__(self, res: EResult):
        super().__init__(translate_result(res))


def read_thumbnails(gcodefile: FILEWrapper):
    """Read thumbnails from binary gcode file."""
    assert is_open(gcodefile)
    rewind(gcodefile)

    header = FileHeader()
    res = read_header(gcodefile, header)
    if res != EResult.Success:
        raise ResultError(res)

    block = BlockHeader()
    thumbnails = []
    while res == EResult.Success:
        res = read_next_block_header(gcodefile, header, block,
                                     EBlockType.Thumbnail)
        if res != EResult.Success:
            break

        thumbnail_block = ThumbnailBlock()
        res = thumbnail_block.read_data(gcodefile, header, block)
        if res != EResult.Success:
            raise ResultError(res)

        thumbnails.append({"meta": thumbnail_block.params,
                           "bytes": thumbnail_block.data()})

    return thumbnails
