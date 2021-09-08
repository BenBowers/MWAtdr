#!/usr/bin/env bash

# Runs the application locally with Docker.

set -e

if [[ $# -ne 6 ]] ; then
    echo "Usage: docker_run_main.sh <inputDir> <obsId> <startTime> <invPolyphaseFilterFile> <outputDir> <ignoreErrors>"
    exit 1
fi

inputDir=$(realpath -m $1)
obsId=$2
startTime=$3
invPolyphaseFilterFile=$(realpath -m $4)
outputDir=$(realpath -m $5)
ignoreErrors=$6

exec docker run \
    -v "$inputDir:/app/input_data:ro" \
    -v "$invPolyphaseFilterFile:/app/inverse_polyphase_filter:ro" \
    -v "$outputDir:/app/output_data:rw" \
    -t "mwa_time_data_reconstructor/main" \
    "$inputDir" "$obsId" "$startTime" "$invPolyphaseFilterFile" "$outputDir" "$ignoreErrors"
