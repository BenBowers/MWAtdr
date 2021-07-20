# MWA Time Data Reconstructor

## Dependencies

Ensure docker is installed on your system. View dockers website for installtion details

<https://docs.docker.com/engine/install/>

## Building

To build the program, run the included bash script [docker\_build](docker_build)
like so:

```bash
./docker_build
```

Alternatively, this can be run with the following command:

```bash
docker build -t mwa_time_data_reconstructor --build-arg DOCKER_BUILD=1 .
```

Note: this can take some time to run on the first time.

## Running

The program is run by executing the [docker\_run](docker_run) file like so

```bash
./docker_run
```

Alternatively, this can be run with the following command:

```bash
docker run -t mwa_time_data_reconstructor
``
