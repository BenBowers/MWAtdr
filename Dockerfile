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
# Install mwalib. Installation guide found here: https://github.com/MWATelescope/mwalib/wiki/Installation
    wget -q "https://github.com/MWATelescope/mwalib/releases/download/v0.10.0/mwalib-v0.10.0-linux_x86_64.tar.gz" -O mwalib.tar.gz && \
	mkdir mwalib && \
    tar xvf mwalib.tar.gz -C mwalib && \
    cp mwalib/libmwalib.* /usr/local/lib && \
    cp mwalib/mwalib.h /usr/local/include && \
	ldconfig && \
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

WORKDIR /app

USER app




# Imports the application source and configures the build.
FROM base AS app_base
ARG BUILD_TYPE

COPY CMakeLists.txt ./
COPY src/ src/
COPY test/unit/ test/unit/

# The system on which the application will be running. Options are 'personal' or 'garrawarla'.
ARG RUNTIME_SYSTEM=garrawarla
ENV RUNTIME_SYSTEM=${RUNTIME_SYSTEM}

# Just configure the CMake build at this stage.
# Unfortunately for the configure to work, all source files built by CMake must be present. We can't include only the
# required source files on a per-target basis.
RUN mkdir build && \
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..

# Can build the common library now so it's cached for the other build stages.
RUN cd build && \
	cmake --build . --target mwatdr_common




# Image for non-MPI unit tests.
FROM base AS local_unit_test

COPY --from=app_base --chown=app:app /app/ /app/

RUN cd build && \
	cmake --build . --target local_unit_test

ENTRYPOINT ["/app/build/local_unit_test"]




# Image for MPI unit tests.
FROM base AS mpi_unit_test
ARG CONTAINER_RUNTIME

COPY --from=app_base --chown=app:app /app/ /app/

RUN chmod +x test/unit/mpi/entrypoint.sh

RUN cd build && \
	cmake --build . --target mpi_unit_test

ENV CONTAINER_RUNTIME=${CONTAINER_RUNTIME}

ENTRYPOINT ["/app/test/unit/mpi/entrypoint.sh"]




# Image for main application executable.
FROM base AS main
ARG CONTAINER_RUNTIME

COPY --chown=app:app entrypoint.sh ./
RUN chmod +x entrypoint.sh

COPY --from=app_base --chown=app:app /app/ /app/

RUN cd build && \
	cmake --build . --target main

ENV CONTAINER_RUNTIME=${CONTAINER_RUNTIME}

ENTRYPOINT ["/app/entrypoint.sh"]
