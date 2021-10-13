#!/usr/bin/env bash

# Runs the integration tests locally with Docker.
# Must be run with the current directory as the project root directory.

set -e

if [[ $# -ne 1 ]] ; then
    echo "Usage: docker_run_integration_test.sh <workingDir>"
    exit 1
fi

workingDir=$1

# Add mwatdr_utils library to Python package search paths.
export PYTHONPATH=$PYTHONPATH:$(realpath -m ./mwatdr_utils)

exec python3 -m pytest --runScript="./docker_run_main.sh" --workingDir="$workingDir" ./test/integration/