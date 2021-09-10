""" Utilities for manipulating inverse polyphase filter files. """


from os import PathLike

import numpy


__ALL__ = [
    'FREQUENCY_CHANNELS',
    'InversePolyphaseFilterFileParseError',
    'read_inv_polyphase_filter',
    'write_inv_polyphase_filter'
]


FREQUENCY_CHANNELS = 256
""" The number of frequency channels within the inverse polyphase filter file (i.e. the number of frequency channels the
    MWA has)."""


def read_inv_polyphase_filter(path: PathLike) -> numpy.array:
    """ Reads an inverse polyphase filter from file.

        :param path: Path of the file to read.
        :return The inverse polyphase filter. A 2D array of 32-bit float real numbers. The outer dimension is time (of
            size 1 to 255), the inner dimension is frequency channel (of size 256).
        :except InversePolyphaseFilterFileParseError: If the file has invalid format.
        :except OSError: If the file could not be opened/read.
    """

    with open(path, 'rb') as file:
        data = file.read()
    if len(data) == 0:
        raise InversePolyphaseFilterFileParseError('File is empty')
    filter_length = data[0]
    if len(data) != 1 + FREQUENCY_CHANNELS * filter_length * 4:
        raise InversePolyphaseFilterFileParseError('File has incorrect length for specified filter length')
    filter_data = numpy.frombuffer(data, dtype='<f4', offset=1).reshape((filter_length, FREQUENCY_CHANNELS))
    return filter_data


def write_inv_polyphase_filter(path: PathLike, filter_data: numpy.array) -> None:
    """ Writes an inverse polyphase filter to file.

    :param path: Path of the file to write to. Will be created or overwritten.
    :param filter_data: The inverse polyphase filter. Must be a 2D array: the outer dimension is time (of size 1 to
        255), the inner dimension is frequency channel (of size 256). Should be an array of 32-bit float real numbers,
        if it is not, it will be cast.
    :except OSError: If the file could not be opened or written to.
    :except ValueError: If the filter has invalid dimensions.
    """

    if filter_data.ndim != 2:
        raise ValueError('filter_data must have 2 dimensions')
    filter_length, channels = filter_data.shape
    if filter_length < 1 or filter_length > 255:
        raise ValueError('Length of filter_data in time dimension must be >= 1 and <= 255')
    if channels != FREQUENCY_CHANNELS:
        raise ValueError(f'Length of filter_data in frequency channel dimension must be exactly {FREQUENCY_CHANNELS}')

    filter_length_bytes = filter_length.to_bytes(length=1, byteorder='little', signed=False)
    filter_bytes = filter_data.astype('<f4', order='C').tobytes()

    with open(path, 'wb') as file:
        file.write(filter_length_bytes)
        file.write(filter_bytes)


class InversePolyphaseFilterFileParseError(Exception):
    """ Raised when an inverse polyphase filter file cannot be parsed due to invalid format. """
