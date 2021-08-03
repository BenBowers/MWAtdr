#!/usr/bin/env bash

set -e

if [[ $# -ne 1 ]] ; then
    echo "Usage: docker_build.sh <target>"
    exit 1
fi

target=$1

docker build --target "$target" -t "mwa_time_data_reconstructor/$target" --build-arg DOCKER_BUILD=1 .
