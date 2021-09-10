"""Utilities for manipulating output time domain signal files."""


from os import PathLike

import numpy


__all__ = [
    'make_output_signal_filename',
    'OUTPUT_SIGNAL_FILENAME_REGEX',
    'OutputSignalFileParseError',
    'read_output_signal',
    'write_output_signal'
]


OUTPUT_SIGNAL_FILENAME_REGEX = r'([0-9]+)_([0-9]+)_([0-9]+)_(\.).bin'
"""Regex matching an output signal filename."""


def make_output_signal_filename(observation_id: int, start_time: int, tile_id: int, signal_chain: str) -> str:
    """Forms an output time domain signal filename.
    
        :param observation_id: ID (GPS time) of the observation.
        :param start_time: GPS time of the start of the signal.
        :param tile_id: Physical ID of the tile.
        :param signal_chain: Character representing the RF input (typically 'X' or 'Y').
        :return: The name of the corresponding output time domain signal file.
    """

    return f'{observation_id}_{start_time}_{tile_id}_{signal_chain}.bin'


def read_output_signal(path: PathLike, allow_empty: bool = False) -> numpy.ndarray:
    """Reads an output time domain signal from file.
    
        :param path: Path of the file to read.
        :param allow_empty: If `False`, an empty file is considered invalid. (Typically the output signal should not be
            empty.)
        :return: The output time domain signal. A 1D array of 16-bit signed integer real numbers.
        :except OSError: If the file could not be opened/read.
        :except OutputSignalFileParseError: If the file has invalid format.
    """

    with open(path, 'rb') as file:
        data = file.read()
    if not allow_empty and len(data) == 0:
        raise OutputSignalFileParseError('File contains no signal data')
    if len(data) % 2 != 0:
        raise OutputSignalFileParseError('File size should be a multiple of 2 bytes')
    signal_data = numpy.frombuffer(data, dtype='<i2')
    return signal_data


def write_output_signal(path: PathLike, signal_data: numpy.ndarray, allow_empty: bool = False) -> None:
    """Writes an output time domain signal to file.
    
        :param path: Path of the file to write to. Will be created or overwritten.
        :param signal_data: The output time domain signal data. Must be a 1D array. Should be an array of 16-bit signed
            integer real numbers; if it is not, it will be cast.
        :param allow_empty: If `True`, `signal_data` with a size of 0 is considered invalid. (Typically the output
            signal file should not be empty.)
        :except OSError: If the file could not be opened or written to.
        :except ValueError: If `signal_data` has invalid dimensions.
    """

    if signal_data.ndim != 1:
        raise ValueError('signal_data must have exactly 1 dimension')
    if not allow_empty and signal_data.size == 0:
        raise ValueError('signal_data must not be empty')
    
    signal_bytes = signal_data.astype('<i2', order='C').tobytes()

    with open(path, 'wb') as file:
        file.write(signal_bytes)


class OutputSignalFileParseError(Exception):
    """Raised when an output signal file cannot be parsed due to invalid format."""
