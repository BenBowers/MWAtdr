"""Unit tests for mwatdr_utils.inv_polyphase_filter module."""


import struct

import numpy
import pytest

from helpers import compute_file_key
from mwatdr_utils.inv_polyphase_filter import InversePolyphaseFilterFileParseError, read_inv_polyphase_filter, \
    write_inv_polyphase_filter


class TestReadInvPolyphaseFilter:
    """Tests for read_inv_polyphase_filter()"""

    @staticmethod
    def test_valid(tmpdir) -> None:
        path = tmpdir / 'inverse_polyphase_filter_valid.bin'
        with open(path, 'wb') as file:
            file.write(b'\x08')
            for time in range(8):
                for channel in range(256):
                    value = (time * 256 + channel) / 10
                    file.write(struct.pack('<f', value))

        file_key_before = compute_file_key(path)
        filter_data = read_inv_polyphase_filter(path)
        file_key_after = compute_file_key(path)

        assert file_key_before == file_key_after

        assert filter_data.shape == (8, 256)
        assert filter_data.dtype == numpy.float32

        for time in range(8):
            for channel in range(256):
                actual = filter_data[time, channel]
                expected = value = (time * 256 + channel) / 10
                assert abs(actual - expected) < 1e-5


    @staticmethod
    def test_nonexistent_file(tmpdir) -> None:
        path = tmpdir / 'inverse_polyphase_filter_nonexistent.bin'

        with pytest.raises(OSError):
            read_inv_polyphase_filter(path)
        assert not path.check()


    @staticmethod
    def test_empty_file(tmpdir) -> None:
        path = tmpdir / 'inverse_polyphase_filter_empty.bin'
        path.ensure()

        file_key_before = compute_file_key(path)
        with pytest.raises(InversePolyphaseFilterFileParseError):
            read_inv_polyphase_filter(path)
        file_key_after = compute_file_key(path)

        assert file_key_before == file_key_after


    @staticmethod
    def test_file_too_short(tmpdir) -> None:
        path = tmpdir / 'inverse_polyphase_filter_short.bin'
        with open(path, 'wb') as file:
            file.write(b'\x12')
            file.write(b'\xDE' * 18)

        file_key_before = compute_file_key(path)
        with pytest.raises(InversePolyphaseFilterFileParseError):
            read_inv_polyphase_filter(path)
        file_key_after = compute_file_key(path)

        assert file_key_before == file_key_after


    @staticmethod
    def test_file_too_long(tmpdir) -> None:
        path = tmpdir / 'inverse_polyphase_filter_long.bin'
        with open(path, 'wb') as file:
            file.write(b'\x4D')
            file.write(b'\xDE' * (0x4D * 256 * 4 + 102))

        file_key_before = compute_file_key(path)
        with pytest.raises(InversePolyphaseFilterFileParseError):
            read_inv_polyphase_filter(path)
        file_key_after = compute_file_key(path)

        assert file_key_before == file_key_after




class TestWriteInvPolyphaseFilter:
    """Tests for write_inv_polyphase_filter()"""

    @staticmethod
    def test_valid(tmpdir) -> None:
        path = tmpdir / 'inverse_polyphase_filter_valid.bin'

        filter_data = numpy.zeros((21, 256), dtype=numpy.float32)
        for time in range(21):
            for channel in range(256):
                value = (time * 256 + channel) / 75.123
                filter_data[time, channel] = value
        filter_data.flags.writeable = False

        ret = write_inv_polyphase_filter(path, filter_data)
        assert ret is None

        with open(path, 'rb') as file:
            actual = file.read()

        expected = bytearray(1 + 21 * 256 * 4)
        expected[0] = 21
        i = 1
        for time in range(21):
            for channel in range(256):
                value = (time * 256 + channel) / 75.123
                expected[i:i+4] = struct.pack('<f', value)
                i += 4

        assert actual == bytes(expected)


    @staticmethod
    def test_invalid_dims(tmpdir) -> None:
        path = tmpdir / 'inverse_polyphase_filter_invalid.bin'

        filter_datas = [
            numpy.zeros((2, 8, 256), dtype=numpy.float32),
            numpy.zeros((256,), dtype=numpy.float32),
            numpy.zeros((0,), dtype=numpy.float32),
            numpy.zeros((102, 256, 5), dtype=numpy.float32),
            numpy.zeros((0, 256), dtype=numpy.float32),
            numpy.zeros((256, 256), dtype=numpy.float32),
            numpy.zeros((52, 0), dtype=numpy.float32),
            numpy.zeros((196, 257), dtype=numpy.float32)
        ]

        for filter_data in filter_datas:
            filter_data.flags.writeable = False
            with pytest.raises(ValueError):
                write_inv_polyphase_filter(path, filter_data)
            assert not path.check()


    @staticmethod
    def test_overwrite(tmpdir) -> None:
        path = tmpdir / 'inverse_polyphase_filter_existing.bin'

        with open(path, 'w') as file:
            file.write('Hello, world!')

        filter_data = numpy.zeros((88, 256), dtype=numpy.float32)
        for time in range(88):
            for channel in range(256):
                value = (time * 256 + channel) / 267.54
                filter_data[time, channel] = value
        filter_data.flags.writeable = False

        ret = write_inv_polyphase_filter(path, filter_data)
        assert ret is None

        with open(path, 'rb') as file:
            actual = file.read()

        expected = bytearray(1 + 88 * 256 * 4)
        expected[0] = 88
        i = 1
        for time in range(88):
            for channel in range(256):
                value = (time * 256 + channel) / 267.54
                expected[i:i+4] = struct.pack('<f', value)
                i += 4

        assert actual == bytes(expected)


    @staticmethod
    def test_type_cast(tmpdir) -> None:
        path = tmpdir / 'inverse_polyphase_filter_cast.bin'

        filter_data = numpy.zeros((49, 256), dtype=numpy.int16)
        for time in range(49):
            for channel in range(256):
                value = ((time * 2345 + channel * 653) % 1000) - 500
                filter_data[time, channel] = value
        filter_data.flags.writeable = False

        ret = write_inv_polyphase_filter(path, filter_data)
        assert ret is None

        with open(path, 'rb') as file:
            actual = file.read()
        
        assert len(actual) == 1 + 49 * 256 * 4
        assert actual[0] == 49

        i = 1
        for time in range(49):
            for channel in range(256):
                expected_val = ((time * 2345 + channel * 653) % 1000) - 500
                actual_val, = struct.unpack('<f', actual[i:i+4])
                assert abs(actual_val - expected_val) < 1e-5
                i += 4




def test_read_write_roundtrip(tmpdir) -> None:
    path = tmpdir / 'inverse_polyphase_filter.bin'

    filter_data = numpy.zeros((112, 256), dtype=numpy.float32)
    for time in range(112):
        for channel in range(256):
            value = (time * 256 + channel) / 453.231
            filter_data[time, channel] = value
    filter_data.flags.writeable = False

    ret = write_inv_polyphase_filter(path, filter_data)
    file_key_before = compute_file_key(path)
    actual = read_inv_polyphase_filter(path)
    file_key_after = compute_file_key(path)

    assert ret is None
    assert file_key_before == file_key_after
    assert actual.shape == (112, 256)
    assert actual.dtype == numpy.float32

    for time in range(112):
        for channel in range(256):
            actual = filter_data[time, channel]
            expected = (time * 256 + channel) / 453.231
            assert abs(actual - expected) < 1e-5
