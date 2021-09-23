#!/usr/bin/env bash

set -e

if [[ $# -ne 4 ]] ; then
    echo "Usage: docker_build.sh <target> <buildType> <runtimeSystem> <containerRuntime>"
    exit 1
fi

target=$1
buildType=$2
runtimeSystem=$3
containerRuntime=$4

docker build \
    --target "$target" \
    -t "mwatdr/$target" \
    --build-arg "BUILD_TYPE=$buildType" \
    --build-arg "RUNTIME_SYSTEM=$runtimeSystem" \
    --build-arg "CONTAINER_RUNTIME=$containerRuntime" \
    --progress plain \
    .
