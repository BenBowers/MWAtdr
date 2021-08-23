#!/usr/bin/env bash

set -e

if [[ $# -ne 3 ]] ; then
    echo "Usage: docker_build.sh <target> <buildType> <containerRuntime>"
    exit 1
fi

target=$1
buildType=$2
containerRuntime=$3

docker build \
    --target "$target" \
    -t "mwa_time_data_reconstructor/$target" \
    --build-arg "BUILD_TYPE=$buildType" \
    --build-arg "CONTAINER_RUNTIME=$containerRuntime" \
    --progress plain \
    .
