# MWA Time Data Reconstructor

## Project Structure

The project builds 3 "targets" (executables):

- `main` - The actual application.
- `local_test` - Runs tests that do not involve MPI.
- `mpi_test` - Runs tests that do involve MPI.

The project is fully containerised. On your local machine, the project will run with Docker. On Garrawarla, the Docker configuration is converted into a Singularity configuration for running with Singularity.  
Multiple Docker build stages are used to create appropriate environments for the different targets.

The source code is structured as follows:

- `src/` - Main application source code.
- `local_test/` - Source code for the `local_test` target.
- `mpi_test/` - Source code for the `mpi_test` target.

The executables are built with CMake.

## Dependencies

Ensure Docker is installed on your system. View Docker's website for installation
details.

<https://docs.docker.com/engine/install/>

## Building

Building a project target is as simple as building the corresponding Docker stage. Convenience scripts are provided for this.

Bash:
```bash
./docker_build.sh <target>
```

PowerShell:
```powershell
./docker_build.ps1 <target>
```

Alternatively, this can be run with the following command:

```bash
docker build --target "$target" -t "mwa_time_data_reconstructor/$target" --build-arg DOCKER_BUILD=1 .
```

Note: this can take some time to run on the first time. Subsequent builds will be quicker.

## Running

Running a project target is as simple as running the corresponding Docker stage. Convenience scripts are provided for this.

Bash:
```bash
./docker_run.sh <target>
```

PowerShell:
```powershell
./docker_run.ps1 <target>
```

Alternatively, this can be run with the following command:

```bash
docker run -t mwa_time_data_reconstructor/$target
```
