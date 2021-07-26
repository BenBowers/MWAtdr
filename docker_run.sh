#!/bin/bash

set -e

if [[ $# -ne 1 ]] ; then
    echo "Usage: docker_run.sh <target>"
    exit 1
fi

target=$1

docker run -t "mwa_time_data_reconstructor/$target"
