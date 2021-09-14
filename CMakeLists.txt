cmake_minimum_required(VERSION 3.10)

project(MWA_TIME_DATA_RECONSTRUCTOR)

find_package(MPI REQUIRED)

set(MAIN_EXECUTABLE "main")
set(LOCAL_UNIT_TEST_EXECUTABLE "local_unit_test")
set(MPI_UNIT_TEST_EXECUTABLE "mpi_unit_test")
set(COMMON_LIBRARY "mwatdr_common")

set(MAIN_SOURCE_DIR "src")
set(UNIT_TEST_SOURCE_DIR "test/unit")
set(LOCAL_UNIT_TEST_SOURCE_DIR "${UNIT_TEST_SOURCE_DIR}/local")
set(MPI_UNIT_TEST_SOURCE_DIR "${UNIT_TEST_SOURCE_DIR}/mpi")

set(MAIN_SOURCE_FILES
    "${MAIN_SOURCE_DIR}/Main.cpp"
)

set(LOCAL_UNIT_TEST_SOURCE_FILES
    "${UNIT_TEST_SOURCE_DIR}/TestHelper.cpp"
    "${LOCAL_UNIT_TEST_SOURCE_DIR}/Main.cpp"
    "${LOCAL_UNIT_TEST_SOURCE_DIR}/OutSignalWriterTest.cpp"
    "${LOCAL_UNIT_TEST_SOURCE_DIR}/ReadCoeDataTest.cpp"
    "${LOCAL_UNIT_TEST_SOURCE_DIR}/ChannelRemappingTest.cpp"
    "${LOCAL_UNIT_TEST_SOURCE_DIR}/NodeAntennaInputAssignerTest.cpp"
)

set(MPI_UNIT_TEST_SOURCE_FILES
    "${UNIT_TEST_SOURCE_DIR}/TestHelper.cpp"
    "${MPI_UNIT_TEST_SOURCE_DIR}/MPITestHelper.cpp"
    "${MPI_UNIT_TEST_SOURCE_DIR}/Main.cpp"
    "${MPI_UNIT_TEST_SOURCE_DIR}/InternodeCommunicationTest.cpp"
)

set(COMMON_SOURCE_FILES
    "${MAIN_SOURCE_DIR}/ReadInputFile.cpp"
    "${MAIN_SOURCE_DIR}/OutSignalWriter.cpp"
    "${MAIN_SOURCE_DIR}/ChannelRemapping.cpp"
    "${MAIN_SOURCE_DIR}/InternodeCommunication.cpp"
    "${MAIN_SOURCE_DIR}/SignalProcessing.cpp"
    "${MAIN_SOURCE_DIR}/NodeAntennaInputAssigner.cpp"
    "${MAIN_SOURCE_DIR}/MetadataFileReader.cpp"
    "${MAIN_SOURCE_DIR}/OutputLogFileWriter.cpp"
    "${MAIN_SOURCE_DIR}/ReadCoeData.cpp"
    "${MAIN_SOURCE_DIR}/Common.cpp"
)

# This must come before adding the build targets, otherwise they won't have any effect.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable("${MAIN_EXECUTABLE}" ${MAIN_SOURCE_FILES})
add_executable("${LOCAL_UNIT_TEST_EXECUTABLE}" ${LOCAL_UNIT_TEST_SOURCE_FILES})
add_executable("${MPI_UNIT_TEST_EXECUTABLE}" ${MPI_UNIT_TEST_SOURCE_FILES})
add_library("${COMMON_LIBRARY}" ${COMMON_SOURCE_FILES})

# Intel MKL library configuration is sourced from this tool:
# https://software.intel.com/content/www/us/en/develop/tools/oneapi/components/onemkl/link-line-advisor.html

set(COMPILE_OPTIONS -m64)
set(COMPILE_DEFINITIONS MKL_ILP64)
set(COMPILE_INCLUDE_DIRS "$ENV{MKLROOT}/include" "$ENV{TBBROOT}/include")
set(LINK_OPTIONS "LINKER:--no-as-needed")
set(LINK_LIBRARY_DIRS "$ENV{MKLROOT}/lib/intel64" "$ENV{TBBROOT}/lib/intel64/gcc4.8")
set(LINK_LIBRARIES MPI::MPI_C mkl_intel_ilp64 mkl_tbb_thread mkl_core pthread m dl tbb stdc++fs "${COMMON_LIBRARY}")

target_compile_options("${MAIN_EXECUTABLE}" PRIVATE ${COMPILE_OPTIONS})
target_compile_options("${LOCAL_UNIT_TEST_EXECUTABLE}" PRIVATE ${COMPILE_OPTIONS})
target_compile_options("${MPI_UNIT_TEST_EXECUTABLE}" PRIVATE ${COMPILE_OPTIONS})
target_compile_options("${COMMON_LIBRARY}" PRIVATE ${COMPILE_OPTIONS})

target_compile_definitions("${MAIN_EXECUTABLE}" PRIVATE ${COMPILE_DEFINITIONS})
target_compile_definitions("${LOCAL_UNIT_TEST_EXECUTABLE}" PRIVATE ${COMPILE_DEFINITIONS})
target_compile_definitions("${MPI_UNIT_TEST_EXECUTABLE}" PRIVATE ${COMPILE_DEFINITIONS})
target_compile_definitions("${COMMON_LIBRARY}" PRIVATE ${COMPILE_DEFINITIONS})

target_include_directories("${MAIN_EXECUTABLE}" PRIVATE ${COMPILE_INCLUDE_DIRS})
target_include_directories("${LOCAL_UNIT_TEST_EXECUTABLE}" PRIVATE ${COMPILE_INCLUDE_DIRS} ${MAIN_SOURCE_DIR} ${UNIT_TEST_SOURCE_DIR})
target_include_directories("${MPI_UNIT_TEST_EXECUTABLE}" PRIVATE ${COMPILE_INCLUDE_DIRS} ${MAIN_SOURCE_DIR} ${UNIT_TEST_SOURCE_DIR})
target_include_directories("${COMMON_LIBRARY}" PRIVATE ${COMPILE_INCLUDE_DIRS})

target_link_options("${MAIN_EXECUTABLE}" PRIVATE ${LINK_OPTIONS})
target_link_options("${LOCAL_UNIT_TEST_EXECUTABLE}" PRIVATE ${LINK_OPTIONS})
target_link_options("${MPI_UNIT_TEST_EXECUTABLE}" PRIVATE ${LINK_OPTIONS})

target_link_directories("${MAIN_EXECUTABLE}" PRIVATE ${LINK_LIBRARY_DIRS})
target_link_directories("${LOCAL_UNIT_TEST_EXECUTABLE}" PRIVATE ${LINK_LIBRARY_DIRS})
target_link_directories("${MPI_UNIT_TEST_EXECUTABLE}" PRIVATE ${LINK_LIBRARY_DIRS})

target_link_libraries("${MAIN_EXECUTABLE}" PRIVATE ${LINK_LIBRARIES})
target_link_libraries("${LOCAL_UNIT_TEST_EXECUTABLE}" PRIVATE ${LINK_LIBRARIES})
target_link_libraries("${MPI_UNIT_TEST_EXECUTABLE}" PRIVATE ${LINK_LIBRARIES})
