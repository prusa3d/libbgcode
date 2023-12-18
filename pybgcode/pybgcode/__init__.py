"""Prusa Block & Binary G-code reader / writer / converter"""
# pylint: disable=redefined-builtin
from typing import Type, Union
from ._bgcode import (  # type: ignore
    BlockHeader,
    CompressionType,
    EBlockType,
    EResult,
    EThumbnailFormat,
    FileHeader,
    FILEWrapper,
    PrintMetadataBlock,
    PrinterMetadataBlock,
    SlicerMetadataBlock,
    FileMetadataBlock,
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
        "FileMetadataBlock",
        "PrintMetadataBlock",
        "PrinterMetadataBlock",
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


def get_header(gcodefile: FILEWrapper):
    assert is_open(gcodefile)
    rewind(gcodefile)
    header = FileHeader()
    res = read_header(gcodefile, header)
    if res != EResult.Success:
        raise ResultError(res)
    return res, header

def get_block_header(gcodefile: FILEWrapper, header: FileHeader,
                     block_type: int):
    block = BlockHeader()
    res = read_next_block_header(gcodefile, header, block, block_type)
    if res != EResult.Success:
        return None
    return block

def get_metadata(gcodefile: FILEWrapper, header: FileHeader,
                 block_header: BlockHeader,
                 metadata_block_class: Union[Type[PrinterMetadataBlock],
                                             Type[PrintMetadataBlock],
                                             Type[FileMetadataBlock]]):
    metadata_block = metadata_block_class()
    res = metadata_block.read_data(gcodefile, header, block_header)
    if res != EResult.Success:
        raise ResultError(res)
    return dict(metadata_block.raw_data) if metadata_block else metadata_block

def read_thumbnails(gcodefile: FILEWrapper):
    """Read thumbnails from binary gcode file."""
    res, header = get_header(gcodefile)
    thumbnails = []

    while res == EResult.Success:
        block_header = get_block_header(gcodefile, header,
                                        EBlockType.Thumbnail)
        if not block_header:
            break

        thumbnail_block = ThumbnailBlock()
        res = thumbnail_block.read_data(gcodefile, header, block_header)
        if res != EResult.Success:
            raise ResultError(res)

        thumbnails.append({"meta": thumbnail_block.params,
                           "bytes": thumbnail_block.data()})

    return thumbnails

def read_metadata(gcodefile: FILEWrapper, type: str = 'printer'):
    """Read metadata from binary gcode file.
    Possible variants for metadata type are 'file', 'print', 'printer'
    and 'slicer' with 'printer' as a default."""

    _, header = get_header(gcodefile)

    if type == 'file':
        block_type = EBlockType.FileMetadata
        metadata_block_class = FileMetadataBlock
    elif type == 'print':
        block_type = EBlockType.PrintMetadata
        metadata_block_class = PrintMetadataBlock
    elif type == 'slicer':
        block_type = EBlockType.SlicerMetadata
        metadata_block_class = SlicerMetadataBlock
    else:
        block_type = EBlockType.PrinterMetadata
        metadata_block_class = PrinterMetadataBlock

    block_header = get_block_header(gcodefile, header, block_type)
    if not block_header:
        return None

    return get_metadata(gcodefile, header, block_header, metadata_block_class)

