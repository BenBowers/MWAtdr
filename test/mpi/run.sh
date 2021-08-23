#!/usr/bin/env bash

# Runs the MPI tests (with arguments passed through to mpirun if running with Docker).

if [[ "${CONTAINER_RUNTIME,,}" == 'singularity' ]] ; then
    # If we're running in Singularity then the container was already started with mpirun.
    ./build/mpi_test
else
    mpirun "$@" ./build/mpi_test
fi
