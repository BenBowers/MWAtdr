from pathlib import Path

from helpers import run_application


def test_no_data_ignore_errors(run_script: Path, working_dir: Path) -> None:
    # Test with no signal data, but a valid metafits and coefficients filter

    working_dir = working_dir / 'no_data'
    working_dir.mkdir(exist_ok=False, parents=True)

    inv_polyphase_filter_path = working_dir / 'inverse_polyphase_filter.bin'
    inv_polyphase_filter = numpy.zeros((13, 256), dtype=numpy.float32)
    inv_polyphase_filter[:, -1] = 1      # Identity
    write_inv_polyphase_filter(inv_polyphase_filter_path, inv_polyphase_filter)

    input_dir = working_dir / 'input_data'
    input_dir.mkdir(exist_ok=False, parents=True)
    shutil.copyfile(TEST_DATA_PATH / '1294797712.metafits', input_dir / '1294797712.metafits')

    output_dir = working_dir / 'output_dir'
    output_dir.mkdir(exist_ok=False, parents=True)

    result = run_application(run_script, input_dir, '1294797712', '1294797712', inv_polyphase_filter_path, output_dir, 'true')
    
    assert result.returncode == 78


def test_no_data_acknowledge_errors(run_script: Path, working_dir: Path) -> None:
    # Test with no signal data, but a valid metafits and coefficients filter

    working_dir = working_dir / 'no_data'
    working_dir.mkdir(exist_ok=False, parents=True)

    inv_polyphase_filter_path = working_dir / 'inverse_polyphase_filter.bin'
    inv_polyphase_filter = numpy.zeros((13, 256), dtype=numpy.float32)
    inv_polyphase_filter[:, -1] = 1      # Identity
    write_inv_polyphase_filter(inv_polyphase_filter_path, inv_polyphase_filter)

    input_dir = working_dir / 'input_data'
    input_dir.mkdir(exist_ok=False, parents=True)
    shutil.copyfile(TEST_DATA_PATH / '1294797712.metafits', input_dir / '1294797712.metafits')

    output_dir = working_dir / 'output_dir'
    output_dir.mkdir(exist_ok=False, parents=True)

    result = run_application(run_script, input_dir, '1294797712', '1294797712', inv_polyphase_filter_path, output_dir, 'false')
    
    assert result.returncode == 78