"""Unit tests for mwatdr_utils.output_signal module."""


import re
import struct

import numpy
import pytest

from helpers import compute_file_key
from mwatdr_utils.output_signal import make_output_signal_filename, OUTPUT_SIGNAL_FILENAME_REGEX, \
    OutputSignalFileParseError, read_output_signal, write_output_signal



def test_filename_regex() -> None:
    match = re.fullmatch(OUTPUT_SIGNAL_FILENAME_REGEX, '76452354_5463092_12_Y.bin')
    assert match is not None
    assert match.groups() == ('76452354', '5463092', '12', 'Y')

    match = re.fullmatch(OUTPUT_SIGNAL_FILENAME_REGEX, '4756_1254789_1254_F.bin')
    assert match is not None
    assert match.groups() == ('4756', '1254789', '1254', 'F')

    match = re.fullmatch(OUTPUT_SIGNAL_FILENAME_REGEX, '-234_ab1234s_1324g_.bin')
    assert match is None

    match = re.fullmatch(OUTPUT_SIGNAL_FILENAME_REGEX, '98045762_39408564_01_V')
    assert match is None



def test_make_filename() -> None:
    actual = make_output_signal_filename(356723454, 356723568, 216, 'X')
    expected = '356723454_356723568_216_X.bin'
    assert actual == expected




class TestReadOutputSignal:
    """Tests for read_output_signal()"""

    @staticmethod
    def test_valid(tmpdir) -> None:
        path = tmpdir / 'output_signal_valid.bin'

        with open(path, 'wb') as file:
            for i in range(73459):
                value = (i % 10000) - 5000
                file.write(struct.pack('<h', value))

        file_key_before = compute_file_key(path)
        signal_data = read_output_signal(path)
        file_key_after = compute_file_key(path)

        assert file_key_before == file_key_after
        assert signal_data.shape == (73459,)
        assert signal_data.dtype == numpy.int16

        for i in range(73459):
            actual = signal_data[i]
            expected = (i % 10000) - 5000
            assert actual == expected


    @staticmethod
    def test_empty_file(tmpdir) -> None:
        path = tmpdir / 'output_signal_empty.bin'
        path.ensure()

        file_key_before = compute_file_key(path)
        with pytest.raises(OutputSignalFileParseError):
            read_output_signal(path)
        file_key_after = compute_file_key(path)
        assert file_key_before == file_key_after

        file_key_before = compute_file_key(path)
        signal_data = read_output_signal(path, allow_empty=True)
        file_key_after = compute_file_key(path)
        assert file_key_before == file_key_after
        assert signal_data.shape == (0,)
        assert signal_data.dtype == numpy.int16


    @staticmethod
    def test_invalid_format(tmpdir) -> None:
        path = tmpdir / 'output_signal_invalid.bin'

        with open(path, 'wb') as file:
            file.write(b'\x3141' * 1234)
            file.write(b'\x31')
        
        with pytest.raises(OutputSignalFileParseError):
            read_output_signal(path)


    @staticmethod
    def test_nonexistent_file(tmpdir) -> None:
        path = tmpdir / 'output_signal_nonexistent.bin'

        with pytest.raises(OSError):
            read_output_signal(path)




class TestWriteOutputSignal:
    """Tests for write_output_signal()"""

    @staticmethod
    def test_valid(tmpdir) -> None:
        path = tmpdir / 'output_signal_valid.bin'

        signal_data = numpy.zeros((2134,), dtype=numpy.int16)
        for i in range(2134):
            signal_data[i] = ((3 * i) % 10034) - 3416
        signal_data.flags.writeable = False

        ret = write_output_signal(path, signal_data)
        assert ret is None

        with open(path, 'rb') as file:
            actual = file.read()
        
        expected = bytearray(2134 * 2)
        for i in range(2134):
            value = ((3 * i) % 10034) - 3416
            expected[2*i:2*(i+1)] = value.to_bytes(2, 'little', signed=True)
        
        assert actual == expected


    @staticmethod
    def test_empty_data(tmpdir) -> None:
        path = tmpdir / 'output_signal_invalid.bin'

        signal_data = numpy.zeros((0,), dtype=numpy.int16)
        signal_data.flags.writeable = False

        with pytest.raises(ValueError):
            write_output_signal(path, signal_data)
        assert not path.check()

        ret = write_output_signal(path, signal_data, allow_empty=True)
        assert ret is None
        with open(path, 'rb') as file:
            actual = file.read()
        expected = b''
        assert actual == expected


    @staticmethod
    def test_invalid_dims(tmpdir) -> None:
        path = tmpdir / 'output_signal_invalid.bin'

        signal_datas = [
            numpy.zeros((123, 0), dtype=numpy.int16),
            numpy.zeros((9082, 753), dtype=numpy.int16),
            numpy.zeros((123, 213, 542), dtype=numpy.int16),
        ]

        for signal_data in signal_datas:
            signal_data.flags.writeable = False
            with pytest.raises(ValueError):
                write_output_signal(path, signal_data)
            assert not path.check()


    @staticmethod
    def test_overwrite(tmpdir) -> None:
        path = tmpdir / 'output_signal_existing.bin'

        with open(path, 'w') as file:
            file.write('Hello again! There is something already here')
        
        signal_data = numpy.zeros((7652,), dtype=numpy.int16)
        for i in range(7652):
            signal_data[i] = ((7 * i // 2) % 6754) - 24367
        signal_data.flags.writeable = False

        ret = write_output_signal(path, signal_data)
        assert ret is None

        with open(path, 'rb') as file:
            actual = file.read()
        
        expected = bytearray(7652 * 2)
        for i in range(7652):
            value = ((7 * i // 2) % 6754) - 24367
            expected[2*i:2*(i+1)] = value.to_bytes(2, 'little', signed=True)
        
        assert actual == expected


    @staticmethod
    def test_type_cast(tmpdir) -> None:
        path = tmpdir / 'output_signal_cast.bin'

        signal_data = numpy.zeros((543,), dtype=float)
        for i in range(543):
            signal_data[i] = i * i / 17 - 12 * i
        signal_data.flags.writeable = False

        ret = write_output_signal(path, signal_data)
        assert ret is None

        with open(path, 'rb') as file:
            actual = file.read()
        
        assert len(actual) == 543 * 2
        for i in range(543):
            expected_val = int(i * i / 17 - 12 * i)
            actual_val, = struct.unpack('<h', actual[2*i:2*(i+1)])
            assert expected_val == actual_val




def test_read_write_roundtrip(tmpdir) -> None:
    path = tmpdir / 'output_signal.bin'

    signal_data = numpy.zeros((9132,), dtype=numpy.int16)
    for i in range(9132):
        signal_data[i] = ((i * i // 5) % 12346) - 909
    signal_data.flags.writeable = False

    ret = write_output_signal(path, signal_data)
    file_key_before = compute_file_key(path)
    actual = read_output_signal(path)
    file_key_after = compute_file_key(path)

    assert ret is None
    assert file_key_before == file_key_after
    assert actual.shape == (9132,)
    assert actual.dtype == numpy.int16

    for i in range(9132):
        actual = signal_data[i]
        expected = ((i * i // 5) % 12346) - 909
        assert actual == expected
