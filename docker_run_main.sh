#!/usr/bin/env bash

# Runs the application locally with Docker.

set -e

if [[ $# -ne 6 ]] ; then
    echo "Usage: docker_run_main.sh <inputDir> <obsId> <startTime> <invPolyphaseFilterFile> <outputDir> <ignoreErrors>"
    exit 1
fi

hostInputDir=$(realpath -m $1)
obsId=$2
startTime=$3
hostInvPolyphaseFilterFile=$(realpath -m $4)
hostOutputDir=$(realpath -m $5)
ignoreErrors=$6

containerInputDir="/mnt/input_data"
containerInvPolyphaseFilterFile="/mnt/inverse_polyphase_filter"
containerOutputDir="/mnt/output_data"

exec docker run \
    -v "$hostInputDir:$containerInputDir:ro" \
    -v "$hostInvPolyphaseFilterFile:$containerInvPolyphaseFilterFile:ro" \
    -v "$hostOutputDir:$containerOutputDir:rw" \
    -t "mwa_time_data_reconstructor/main" \
    "$containerInputDir" "$obsId" "$startTime" "$containerInvPolyphaseFilterFile" "$containerOutputDir" "$ignoreErrors"
