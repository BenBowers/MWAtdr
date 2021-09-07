# The type of build to do with CMake. See here: https://cmake.org/cmake/help/v3.10/variable/CMAKE_BUILD_TYPE.html
ARG BUILD_TYPE=Release

# Indicates what container we are in at runtime. Options are 'docker' or 'singularity'.
ARG CONTAINER_RUNTIME=singularity




# Installs all the required packages and configures the system.
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
	rm -rf ./* /var/lib/apt/lists/* /etc/apt/sources.list.d/oneAPI.list

# Set up Intel OneAPI library environment variables required for code compilation.
ENV MKLROOT="/opt/intel/oneapi/mkl/latest"
ENV TBBROOT="/opt/intel/oneapi/tbb/latest"

# Create a new user so the application doesn't run as root (also MPI doesn't like to be run as root).
RUN adduser --system --group app

RUN mkdir /app
RUN chown -R app:app /app




# Imports the application source and configures the build.
FROM base AS app_base
ARG BUILD_TYPE

WORKDIR /app

COPY CMakeLists.txt ./
COPY src/ src/
COPY test/unit/ test/unit/

# Just configure the CMake build at this stage.
# Unfortunately for the configure to work, all source files built by CMake must be present. We can't include only the
# required source files on a per-target basis.
RUN mkdir build && \
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..

# Can build the common library now so it's cached for the other build stages.
RUN cd build && \
	cmake --build . --target mwatdr_common




# Base image for unit tests.
FROM base AS unit_test_base

WORKDIR /app

USER app

COPY --from=app_base --chown=app:app /app/ /app/




# Image for non-MPI unit tests.
FROM unit_test_base AS local_unit_test

RUN cd build && \
	cmake --build . --target local_unit_test

ENTRYPOINT ["/app/build/local_unit_test"]




# Image for MPI unit tests.
FROM unit_test_base AS mpi_unit_test
ARG CONTAINER_RUNTIME

RUN cd build && \
	cmake --build . --target mpi_unit_test

RUN chmod +x ./test/unit/mpi/run.sh

ENV CONTAINER_RUNTIME=${CONTAINER_RUNTIME}

ENTRYPOINT ["/app/test/unit/mpi/run.sh"]




# Image for integration tests.
FROM base as integration_test

RUN apt-get update -qq && \
	apt-get install -qq --no-install-recommends -y python3-minimal python3-pip && \
	python3 -m pip install -qq pytest && \
# Clean up files and packages no longer required.
	apt-get purge --auto-remove -qq -y python3-pip && \
	apt-get clean -qq -y && \
	rm -rf ./* /var/lib/apt/lists/*

WORKDIR /app

USER app

COPY --from=app_base --chown=app:app /app/ /app/

COPY --chown=app:app test/integration/ test/integration/

RUN cd build && \
	cmake --build . --target main

ENTRYPOINT [ "python3", "-m", "pytest", "/app/test/integration" ]




# Image for main application executable.
FROM base AS main

WORKDIR /app

USER app

COPY --from=app_base --chown=app:app /app/ /app/

RUN cd build && \
	cmake --build . --target main

ENTRYPOINT ["mpirun", "/app/build/main"]
