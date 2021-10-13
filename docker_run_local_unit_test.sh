#!/usr/bin/env bash

# Runs the non-MPI unit tests locally with Docker.
# Must be run with the current directory as the project root directory.

set -e

if [[ $# -ne 1 ]] ; then
    echo "Usage: docker_run_local_unit_test.sh <workingDir>"
    exit 1
fi

hostInputDir="$(pwd)/test/input_data"
hostOutputDir="$(pwd)/test/output_data"
containerInputDir="/mnt/test_input"
containerOutputDir="/mnt/test_output"
hostTmpDir=$(realpath -m $1)

exec docker run \
    -v "$hostInputDir:$containerInputDir:ro" \
    -v "$hostOutputDir:$containerOutputDir:rw" \
    -v "$hostTmpDir:/tmp:rw" \
    -t "mwatdr/local_unit_test"