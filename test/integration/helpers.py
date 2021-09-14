from hashlib import md5
from pathlib import Path
import subprocess


def compute_file_hash(path) -> str:
    """Computes the MD5 hash of a file."""

    with open(path, 'rb') as file:
        hasher = md5()
        while True:
            block = file.read(8192)
            if not block:
                break
            hasher.update(block)
        return hasher.hexdigest()


def compute_file_key(path):
    """Computes a value for a file that can be used to determine if the file has been modified."""

    path = Path(path)
    file_hash = compute_file_hash(path)
    modified_time = path.stat().st_mtime
    return (file_hash, modified_time)


def run_application(run_script_path, input_dir, observation_id, start_time, inv_polyphase_filter_path, output_dir,
        ignore_errors) -> subprocess.CompletedProcess:
    """Runs the application with the given arguments."""

    args = [str(run_script_path), str(input_dir), str(observation_id), str(start_time), str(inv_polyphase_filter_path),
        str(output_dir), str(ignore_errors)]
    return subprocess.run(args)
