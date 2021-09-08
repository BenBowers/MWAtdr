# MWA Time Data Reconstructor

## Project Structure

The project builds 3 "targets" (executables):

- `main` - The actual application.
- `local_unit_test` - Runs unit tests that do not involve MPI.
- `mpi_unit_test` - Runs unit tests that do involve MPI.

The project is fully containerised. On your local machine, the project will run with Docker. On Garrawarla, the Docker configuration is converted into a Singularity configuration for running with Singularity.  
Multiple Docker build stages are used to create appropriate environments for the different targets.

TODO: file/folder structure explanation

The executables are built with CMake.

## Dependencies

Ensure Docker is installed on your system. View Docker's website for installation
details.

<https://docs.docker.com/engine/install/>

## Building

Building a project target is as simple as building the corresponding Docker stage:

```bash
docker build --target "$target" -t "mwa_time_data_reconstructor/$target" --build-arg BUILD_TYPE=$buildType --build-arg CONTAINER_RUNTIME=$containerRuntime .
```

`$buildType` is the [CMake build type](https://cmake.org/cmake/help/v3.10/variable/CMAKE_BUILD_TYPE.html).
If not specified, defaults to `Release`, which is an optimised build suitable for real-world use.

`$containerRuntime` indicates what containerisation environment will be used to run the build. Options are `docker` or `singularity`.
If not specified, defaults to `singularity`.

Convenience scripts are provided for building:

Bash:
```bash
./docker_build.sh $target $buildType $containerRuntime
```

PowerShell:
```powershell
./docker_build.ps1 $target $buildType $containerRuntime
```

Note: this can take some time to run on the first time. Subsequent builds will be quicker.

## Running

Running a project target is as simple as running the corresponding Docker stage:

```bash
docker run -t mwa_time_data_reconstructor/$target
```

Convenience scripts are provided for running:

Bash:
```bash
./docker_run.sh $target
```

PowerShell:
```powershell
./docker_run.ps1 $target
```
