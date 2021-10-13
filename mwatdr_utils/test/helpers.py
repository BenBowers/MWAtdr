from hashlib import md5
from os import PathLike
from pathlib import Path


__all__ = [
    'compute_file_hash',
    'compute_file_key'
]


def compute_file_hash(path: PathLike) -> str:
    """Computes the MD5 hash of a file."""

    with open(path, 'rb') as file:
        hasher = md5()
        while True:
            block = file.read(8192)
            if not block:
                break
            hasher.update(block)
        return hasher.hexdigest()


def compute_file_key(path: PathLike):
    """Computes a value for a file that is dependend on the file content, and can be used to determine if the file has
        been modified."""

    path = Path(path)
    file_hash = compute_file_hash(path)
    modified_time = path.stat().st_mtime
    return (file_hash, modified_time)
