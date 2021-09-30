#!/usr/bin/env bash

# Runs the MPI unit tests locally with Docker.

set -e

if [[ $# -ne 1 ]] ; then
    echo "Usage: docker_run_mpi_unit_test.sh <nodeCount>"
    exit 1
fi

nodeCount=$1

exec docker run -t "mwatdr/mpi_unit_test" "$nodeCount"