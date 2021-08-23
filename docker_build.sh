#!/usr/bin/env bash

set -e

if [[ $# -ne 2 ]] ; then
    echo "Usage: docker_build.sh <target> <buildType>"
    exit 1
fi

target=$1
buildType=$2

docker build --target "$target" -t "mwa_time_data_reconstructor/$target" --build-arg "BUILD_TYPE=$buildType" .
