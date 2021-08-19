#!/usr/bin/env bash

# Runs the MPI tests with arguments passed through to mpirun.

mpirun "$@" ./build/mpi_test
