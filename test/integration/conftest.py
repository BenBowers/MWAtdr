from pathlib import Path

import pytest


def pytest_addoption(parser):
    parser.addoption('--runScript', action='store', required=True, help='Path to script that runs the application')
    parser.addoption('--workingDir', action='store', required=True, help='Path to a temporary directory for working space.')


@pytest.fixture
def run_script(request):
    return Path(request.config.getoption('--runScript')).resolve()


@pytest.fixture
def working_dir(request):
    return Path(request.config.getoption('--workingDir')).resolve() / "mwatdr_integration_test"
