# MWA Time Data Reconstructor

Developed by Zak Booth, Benjamin Bowers, Dylan Chanter, and Reece Jones.

## Purpose

This application reconstructs high resolution time domain data from the frequency domain data provided by the MWA correlator.

The application is designed to run on the Garrawarla supercomputer to parallelise processing.

For specifics on the operation of the application, please refer to the Software Requirements Specification.

See also `InversePolyphaseFilterFileSpec.md` and `OutputSignalFileSpec.md` for documentation of the application's custom file formats.

## Basic Project Overview

The application is fully containerised with Docker and Singularity.

There are 3 "targets" corresponding to Docker build stages:

- `main` - The main application (if you are a standard user, probably what you are interested in). (Also used for integration testing.)
- `local_unit_test` - Unit tests that do not involve parallelism.
- `mpi_unit_test` - Unit tests that do involve parallelism.

Building the application works roughly as follows:

1. First, build with Docker to produce a Docker image.
2. Second, use Singularity to convert that Docker image into a Singularity image for running on Garrawarla.

Running the application works roughly as follows:

- For usage on your personal machine, just run the Docker image.
- For usage on Garrawarla, use SLURM to schedule a job that uses `srun` to invoke the Singularity image.

Please see the sections below for detailed instruction on how to build and run the application for various use cases.

## Requirements

For building:

- Docker. Any new-ish version with support for multi-stage builds should work, e.g. Docker Engine 20.x.
- Singularity. Targeted version 3.5.3. (Not required for building on personal machine.)

For running on Garrawarla:

- Singularity. Targeted version 3.5.3.
- SLURM. Targeted version 20.02.4.
- Open MPI. Targeted version 4.0.2.

For integration testing:

- Python. Targeted version 3.8.2.
- Pytest. Targeted version 4.6.9.

## Building and Running `main` for Use on Garrawarla

The instructions here are for building and running the main application for use on Garrawarla - if you are a standard user, this is probably what you want to do.

### Building

First, build the `main` target with Docker on your personal machine (since Garrawarla does not have Docker), using the provided script:

```bash
./docker_build.sh main Release garrawarla singularity
```

This invokes Docker to build the main application image, using a configuration optimised for high performance on Garrawarla, and designed to run with Singularity.
The name of the image is `mwatdr/main`.  
Note that you MUST perform this step on an x86 machine, to match the CPU architecture of Garrawarla.
By default, Docker build will use base images which match the current system architecture.
You can modify the Dockerfile to manually specify the architecture if you wish to build on other types of systems.

The build will probably take quite a while the first time, due to some large libraries which are installed and built in the image.
Do not be alarmed if it takes over 20 minutes.

Next, you need to convert the Docker image to a Singularity image.  
There are a few options of which system to perform the conversion on:

- Use Singularity installed on your personal machine. Installation of Singularity is easy if you use Linux or Mac, but difficult for Windows.
- Use Singularity already installed on Garrawarla, either on the login nodes or on the compute nodes with an interactive SLURM job. Using the compute nodes is likely fastest to build, but it does require transferring the >5GB Docker image to Garrawarla first.

Convert the Docker image to tar format:

```bash
docker save mwatdr/main -o main.tar
```

Then ensure `main.tar` is in the current directory on your chosen machine, and build with Singularity:

```bash
singularity build --tmpdir=<some_dir> main.sif docker-archive://main.tar
```

This takes the `main.tar` Docker image we just saved and produces `main.sif`, the corresponding Singularity image.

By default, Singularity uses the system temporary directory for working space, and may use a few gigabytes of space, which may be an issue for some systems - particularly Linux, where `/tmp` may be in RAM.  
Therefore it is highly recommended to set `<some_dir>` to somewhere on disk.

The Singularity build may take several minutes to complete.

### Running

Running the application on Garrawarla is done with SLURM and `srun`.

On the Garrawarla login nodes, ensure that the `main.sif` Singularity image created in the previous steps is present in the current directory. Then queue the job using the provided SLURM script:

```bash
sbatch slurm_main.sh <args>
```

`<args>` are the application command line arguments. Please see the "User Interface" section for details.

`slurm_main.sh` uses a node configuration which we believe to be sensible/optimal. Feel free to modify it or create your own script to suit your needs, if you require.

## Building and Running for Use on Personal Machine

NOTE: the application is designed to run on Garrawarla, and your personal machine may not be powerful enough to feasibly run the `main` target.  
For 24 frequency channels and 256 antenna inputs, we found `main` requires approximately 8GB of memory per process, and running with a few processes will take on the order of a few hours to complete.  
It is possible to manually specify the number of processes to `mpirun` in `entrypoint.sh`, but there will still be a tradeoff between memory usage and run time.

The instructions here are for building and running the main application for use on your personal, standard computer.
This may be useful for testing purposes.

First, build the `main` target with Docker, using the provided script:

```bash
./docker_build.sh main Release personal docker
```

This invokes Docker to build the main application image, configured for a standard computer, and designed to run with Docker.

Then simply run the `main` target with Docker, using the provided script:

```bash
./docker_run_main.sh <args>
```

`<args>` are the application command line arguments. Please see the "User Interface" section for details.

## User Interface

The `main` target has the following positional command line arguments:

- `<inputDir>` - Path to the directory which contains the input data (metafits and signal files).
- `<observationID>` - The ID (GPS time) of the observation to process.
- `<startTime>` - The beginning time of the 8 second block within the observation that is processed (must be a multiple of 8).
- `<invPolyphaseFilterFile>` - Path to the user-supplied inverse polyphase filter. Please see `InversePolyphaseFilterFileSpec.md` for details.
- `<outputDir>` - Path to the directory which the application will write output data to.
- `<ignoreErrors>` - If `true`, try to ignore any runtime errors, possibly excluding antenna inputs or frequency channels. If `false`, quit processing and exit immediately upon any runtime errors.

Note that when running on Garrawarla, `<inputDir>`, `<invPolyphaseFilterFile>`, and `<outputDir>` must be accessible and shared on all nodes which run the application, e.g. network attached storage.  
Additionally, the container requires permissions to access these directories and files.
If you have particularly restrictive file permissions set (e.g. on Linux, deny read/write to "other"), you may need to relax them.

## Testing

The project contains various tests which were used in developed to verify that the application works.
As a standard user, you probably don't need to care about testing, but a brief overview will be given here anyway.

There are two types of tests: unit tests and integration tests.  
Unit tests individually test components within the application. These are split further into two types: nonparallelised tests and parallelised tests.  
Integration tests test the application as a whole.  

### Unit Testing

The nonparallelised and parallelised unit tests are contained in the `local_unit_test` and `mpi_unit_test` targets, respectively.
These are built similarly to the `main` target.
For example, for building nonparallelised tests on your personal machine:

```bash
./docker_build.sh local_unit_test Release personal docker
```

Running the test targets is different to the main application, however, as test data is required to be mounted into the container at runtime.  
The `docker_run_local_unit_test.sh` and `docker_run_mpi_unit_test.sh` scripts are provided, which do the required setup.

For example, running the nonparallelised tests on your personal machine:

```bash
./docker_run_local_unit_test.sh ./test_tmp
```

Where `./test_tmp` is a directory to use as working space.

Please see `docker_run_local_unit_test.sh` and `docker_run_mpi_unit_test.sh` for details.

### Integration Testing

The integration testing is performed by a Python Pytest suite which invokes the `main` target, such that tests are performed externally to the application.

The general form of running the integration tests is as follows:

```bash
python3 -m pytest --runScript=<runScript> --workingDir=<workingDir> ./test/integration/
```

`<runScript>` is an executable (script or binary) which invokes the application as if by a standard user.

`<workingDir>` is a directory to use for temporary working space (i.e. input and output for the application). To run the entire test suite at once, hundreds of gigabytes of space is required in this directory.
Note that this directory must be accessible and shared by all nodes/processes which run the application.

`./test/integration/` is the path which Pytest will search for the test code.

For running the integration tests on your personal machine, the `docker_run_integration_test.sh` script is provided, which uses `docker_run_main.sh` as `<runScript>`.  
However, it is probably not a good idea to run the integration tests on your personal machine, at least not the entire suite at once, unless you have >32GB of RAM, hundreds of gigabytes of spare disk space, and hours of compute time to spare.

## Advanced Building

In general, the project is built as follows using the provided build script:

```bash
./docker_build.sh $target $buildType $runtimeSystem $containerRuntime
```

Which uses the following Docker command:

```bash
docker build --target "$target" -t "mwatdr/$target" --build-arg BUILD_TYPE=$buildType --build-arg RUNTIME_SYSTEM=$runtimeSystem --build-arg CONTAINER_RUNTIME=$containerRuntime .
```

`$target` is the application target: `main`, `local_unit_test`, or `mpi_unit_test`.

`$buildType` is the [CMake build type](https://cmake.org/cmake/help/v3.10/variable/CMAKE_BUILD_TYPE.html).
If not specified, defaults to `Release`, which is an optimised build suitable for real-world use.

`$runtimeSystem` is the environment in which the application will run.
Options are `personal` (standard computer) or `garrawarla` (Garrawarla supercomputer),
If not specified, defaults to `garrawarla`.

`$containerRuntime` indicates what containerisation environment will be used to run the build. Options are `docker` or `singularity`.
If not specified, defaults to `singularity`.

The name of the built Docker image is `mwatdr/$target`.

## Utility Library: mwatdr_utils

This is a Python library which may assist interfacing with the MWATDR application.  
In particular, it provides functions for reading and writing the custom file formats.

Please see `mwatdr_utils/README.md` for details.

## Internal Specifics

The application is coded in C++, except for the integration tests and utility library. Standard C++17 is targeted.  
The code is compiled within the container image with CMake and GCC. Configuration is in `CMakeLists.txt`.

For efficient signal processing operations (e.g. FFT, convolution), the Intel Math Kernel Library is used (installed within the container).

For multithreading, the Intel Threading Building Blocks library is used (installed within the container).

Open MPI is installed within the container for parallelisation.  
When running with Docker, the parallelism occurs inside a single container instance.
On Garrawarla, parallelism is achieved outside the containerisation with Singularity and SLURM, and Singularity swaps out the Open MPI binaries inside the container for those running on the host machines.

## Project Structure

Files of note in the root directory:

- `Dockerfile` - Specifies the Docker image configuration for all three targets.
- `CMakeLists.txt` - CMake configuration.
- `entrypoint.sh` - The entrypoint of the `main` target within the container.
- Scripts for building and running the application (e.g. `docker_build.sh`).
- `InversePolyphaseFilterFileSpec.md` - Specification of the inverse polyphase filter file format.
- `OutputSignalFileSpec.md` - Specification of the output signal file format.

`src/` directory - Application source code.

`test/` directory - Test code.  
`test/integration/` directory - Integration test code.  
`test/unit/` directory - Unit test code.  
`test/unit/local/` directory - Nonparallelised unit test code.  
`test/unit/mpi/` directory - Parallelised unit test code.  
`test/input_data/` directory - Test data. See `test/README.md` for details.

`mwatdr_utils/` directory - Utility library. See `mwatdr_utils/README.md` for details.
