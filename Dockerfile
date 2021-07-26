# Base image for all the executables.
FROM debian:buster AS base

WORKDIR /tmp

# Install some core packages so we can install other things.
RUN apt-get update -qq
RUN apt-get install -qq --no-install-recommends -y ca-certificates wget gnupg2

# Add Intel repositories to apt for MKL and TBB installation. Steps taken from here:
# https://software.intel.com/content/www/us/en/develop/documentation/installation-guide-for-intel-oneapi-toolkits-linux/top/installation/install-using-package-managers/apt.html
RUN wget -q https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB
RUN apt-key add GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB
RUN rm GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB
RUN echo "deb https://apt.repos.intel.com/oneapi all main" | tee /etc/apt/sources.list.d/oneAPI.list

# Install required packages.
RUN apt-get update -qq
RUN apt-get install -qq --no-install-recommends -y gcc g++ make cmake intel-oneapi-mkl-devel intel-oneapi-tbb-devel lbzip2

# Install Open MPI. Configuration taken from https://hub.docker.com/r/pawsey/openmpi-base
RUN wget -q https://download.open-mpi.org/release/open-mpi/v4.0/openmpi-4.0.2.tar.bz2
RUN tar -xf openmpi-4.0.2.tar.bz2
RUN cd openmpi-4.0.2 && \
	./configure --enable-fast=all,O3 --prefix=/usr && \
	make -j4 && \
	make install
RUN rm -rf openmpi-4.0.2.tar.bz2 openmpi-4.0.2

# Set up Intel OneAPI library environment vars.
# exports will be discarded after the RUN completes.
ENV MKLROOT="/opt/intel/oneapi/mkl/latest"
ENV TBBROOT="/opt/intel/oneapi/tbb/latest"

# Set to 1 when building with Docker for non-Garrawarla use.
ARG DOCKER_BUILD=0

# Without Singularity, SSH is required to enable MPI communication.
RUN if [ "$DOCKER_BUILD" = "1" ] ; then apt-get install -qq --no-install-recommends -y ssh ; fi

WORKDIR /app

# Change user so application doesn't run as root (also MPI doesn't like to be run as root).
RUN adduser --system --group app
RUN chown -R app:app /app
USER app

# Copy project resources.
COPY --chown=app:app CMakeLists.txt ./
COPY --chown=app:app src/ src/
COPY --chown=app:app local_test/ local_test/
COPY --chown=app:app mpi_test/ mpi_test/


# Image for main application executable.
FROM base AS main

# Build "main" CMake target only.
RUN mkdir build && \
	cd build && \
	cmake .. && \
	cmake --build . --target main

ENTRYPOINT ["mpirun", "./build/main"]


# Image for non-MPI tests.
FROM base AS local_test

# Build "local_test" CMake target only.
RUN mkdir build && \
	cd build && \
	cmake .. && \
	cmake --build . --target local_test

ENTRYPOINT ["./build/local_test"]


# Image for MPI tests.
FROM base AS mpi_test

# Build "mpi_test" CMake target only.
RUN mkdir build && \
	cd build && \
	cmake .. && \
	cmake --build . --target mpi_test

ENTRYPOINT ["mpirun", "./build/mpi_test"]
