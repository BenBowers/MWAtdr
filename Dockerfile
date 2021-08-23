# Base image for all the executables.
FROM debian:bullseye-slim AS base

WORKDIR /tmp

# Install required packages.
# Note SSH is only required for MPI communication outside of Garrawarla.
RUN apt-get update -qq && \
	apt-get install -qq --no-install-recommends -y ca-certificates gnupg2 wget gcc g++ make cmake lbzip2 ssh && \
# Add Intel repositories to apt for MKL and TBB installation. Steps taken from here:
# https://software.intel.com/content/www/us/en/develop/documentation/installation-guide-for-intel-oneapi-toolkits-linux/top/installation/install-using-package-managers/apt.html
	wget -q https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB && \
	apt-key add GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB && \
	echo "deb https://apt.repos.intel.com/oneapi all main" | tee /etc/apt/sources.list.d/oneAPI.list && \
# Install Intel MKL and TBB.
	apt-get update -qq && \
	apt-get install -qq --no-install-recommends -y intel-oneapi-mkl-devel intel-oneapi-tbb-devel && \
# Install Open MPI. Configuration taken from https://hub.docker.com/r/pawsey/openmpi-base
	wget -q https://download.open-mpi.org/release/open-mpi/v4.0/openmpi-4.0.2.tar.bz2 && \
	tar -xf openmpi-4.0.2.tar.bz2 && \
	cd openmpi-4.0.2 && \
	./configure --enable-fast=all,O3 --prefix=/usr && \
	make -j4 && \
	make install && \
	cd ../ && \
# Clean up files and packages no longer required.
	apt-get purge --auto-remove -qq -y ca-certificates gnupg2 wget lbzip2 && \
	apt-get clean -qq -y && \
	rm -rf ./* /var/lib/apt/lists/*

# Set up Intel OneAPI library environment variables required for code compilation.
ENV MKLROOT="/opt/intel/oneapi/mkl/latest"
ENV TBBROOT="/opt/intel/oneapi/tbb/latest"

WORKDIR /app

# Change user so application doesn't run as root (also MPI doesn't like to be run as root).
RUN adduser --system --group app && \
	chown -R app:app /app
USER app

# Copy project resources.
COPY --chown=app:app CMakeLists.txt ./
COPY --chown=app:app src/ src/
COPY --chown=app:app test/ test/

# The type of build to do with CMake.
ARG BUILD_TYPE=Release

# Just configure the CMake build at this stage.
RUN mkdir build && \
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..


# Image for non-MPI tests.
FROM base AS local_test

# Build "local_test" CMake target only.
RUN cd build && \
	cmake --build . --target local_test

ENTRYPOINT ["./build/local_test"]


# Image for MPI tests.
FROM base AS mpi_test

# Build "mpi_test" CMake target only.
RUN cd build && \
	cmake --build . --target mpi_test

RUN chmod +x ./test/mpi/run.sh
ENTRYPOINT ["./test/mpi/run.sh"]


# Image for main application executable.
FROM base AS main

# Build "main" CMake target only.
RUN cd build && \
	cmake --build . --target main

ENTRYPOINT ["mpirun", "./build/main"]
