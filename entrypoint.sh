#!/usr/bin/env bash

# Runs the main application, with command line arguments passed through.

if [[ "${CONTAINER_RUNTIME,,}" == 'singularity' ]] ; then
    # If we're running in Singularity then the container was already started with srun.
    exec ./build/main "$@"
else
    exec mpirun ./build/main "$@"
fi
