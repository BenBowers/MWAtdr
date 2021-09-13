#!/usr/bin/env bash

# Runs the non-MPI unit tests locally with Docker.
# Must be run with the current directory as the project root directory.

set -e

if [[ $# -ne 0 ]] ; then
    echo "Usage: docker_run_local_unit_test.sh"
    exit 1
fi

hostInputDir="$(pwd)/test/input_data"
hostOutputDir="$(pwd)/test/output_data"
containerInputDir="/mnt/test_input"
containerOutputDir="/mnt/test_output"

exec docker run \
    -v "$hostInputDir:$containerInputDir:ro" \
    -v "$hostOutputDir:$containerOutputDir:rw" \
    -t "mwatdr/local_unit_test"