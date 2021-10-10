#!/usr/bin/env bash

# Runs the integration tests locally with Docker.

python3 -m pytest --runScript="./docker_run_main.sh" --workingDir="/tmp" ./test/integration/
