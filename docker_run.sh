#!/usr/bin/env bash

set -e

if [[ $# -lt 1 ]] ; then
    echo "Usage: docker_run.sh <target> [<args>]"
    exit 1
fi

target=$1

docker run -t "mwatdr/$target" "${@:2}"
