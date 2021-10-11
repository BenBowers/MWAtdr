from pathlib import Path
import shutil

import pytest


def pytest_addoption(parser):
    parser.addoption('--runScript', action='store', required=True, help='Path to script that runs the application')
    parser.addoption('--workingDir', action='store', required=True, help='Path to a temporary directory for working space.')


@pytest.fixture(scope="session")
def run_script(request) -> Path:
    return Path(request.config.getoption('--runScript')).resolve()


@pytest.fixture(scope="session")
def working_dir(request) -> Path:
    return Path(request.config.getoption('--workingDir')).resolve() / "mwatdr_integration_test"


@pytest.fixture(scope="session", autouse=True)
def clear_working_dir(working_dir: Path) -> None:
    # Delete working directory so existing files don't mess up test cases.
    try:
        shutil.rmtree(working_dir)
    except FileNotFoundError:
        pass
