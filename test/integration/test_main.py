from pathlib import Path
from helpers import run_application
import os
import sys


def test_dummy(run_script: Path, working_dir: Path) -> None:
    input_dir = working_dir / 'input_data'
    inverse_polyphase_filter_file = working_dir / 'inverse_polyphase_filter.bin'
    output_dir = working_dir / 'output_dir'
    result = run_application(run_script, input_dir, '12345678', '12345678', inverse_polyphase_filter_file, output_dir, 'true')
    assert result.returncode == 0


def test_case_2()
    input_dir = working_dir / 'input_data'
    
    
    file = open(working_dir / "inverse_polyphase_filter.bin", "wb")
    for i in range(1,256): 
        file.write(b"This binary string will be written to sample.bin")
    file.close()
    
    inverse_polyphase_filter_file = working_dir / 'inverse_polyphase_filter.bin'
    output_dir = working_dir / 'output_dir'
    result = run_application(run_script, input_dir, '12345678', '12345678', inverse_polyphase_filter_file, output_dir, 'true')
