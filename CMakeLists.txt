cmake_minimum_required(VERSION 3.10)

project(MWA_TIME_DATA_RECONSTRUCTOR)

find_package(MPI REQUIRED)

set(MAIN_EXECUTABLE "main")
set(LOCAL_TEST_EXECUTABLE "local_test")
set(MPI_TEST_EXECUTABLE "mpi_test")

set(MAIN_SOURCE_DIR "src")
set(TEST_SOURCE_DIR "test")
set(LOCAL_TEST_SOURCE_DIR "${TEST_SOURCE_DIR}/local")
set(MPI_TEST_SOURCE_DIR "${TEST_SOURCE_DIR}/mpi")

set(MAIN_SOURCE_FILES
    "${MAIN_SOURCE_DIR}/Main.cpp"
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

set(LOCAL_TEST_SOURCE_FILES
    "${TEST_SOURCE_DIR}/TestHelper.cpp"
    "${LOCAL_TEST_SOURCE_DIR}/Main.cpp"
    "${LOCAL_TEST_SOURCE_DIR}/ReadCoeDataTest.cpp"
    "${MAIN_SOURCE_DIR}/ReadCoeData.cpp"
    "${LOCAL_TEST_SOURCE_DIR}/ChannelRemappingTest.cpp"
    "${MAIN_SOURCE_DIR}/ChannelRemapping.cpp"
    "${LOCAL_TEST_SOURCE_DIR}/NodeAntennaInputAssignerTest.cpp"
    "${MAIN_SOURCE_DIR}/NodeAntennaInputAssigner.cpp"
)

set(MPI_TEST_SOURCE_FILES
    "${TEST_SOURCE_DIR}/TestHelper.cpp"
    "${MPI_TEST_SOURCE_DIR}/MPITestHelper.cpp"
    "${MPI_TEST_SOURCE_DIR}/Main.cpp"
    "${MPI_TEST_SOURCE_DIR}/InternodeCommunicationTest.cpp"
    "${MAIN_SOURCE_DIR}/InternodeCommunication.cpp"
    "${MAIN_SOURCE_DIR}/ChannelRemapping.cpp"
    "${MAIN_SOURCE_DIR}/Common.cpp"
    "${MAIN_SOURCE_DIR}/NodeAntennaInputAssigner.cpp"
)

# This must come before the add_executable(), otherwise they won't have any effect.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable("${MAIN_EXECUTABLE}" ${MAIN_SOURCE_FILES})
add_executable("${LOCAL_TEST_EXECUTABLE}" ${LOCAL_TEST_SOURCE_FILES})
add_executable("${MPI_TEST_EXECUTABLE}" ${MPI_TEST_SOURCE_FILES})

# Intel MKL library configuration is sourced from this tool:
# https://software.intel.com/content/www/us/en/develop/tools/oneapi/components/onemkl/link-line-advisor.html

target_compile_options("${MAIN_EXECUTABLE}" PRIVATE -m64)
target_compile_options("${LOCAL_TEST_EXECUTABLE}" PRIVATE -m64)
target_compile_options("${MPI_TEST_EXECUTABLE}" PRIVATE -m64)

target_link_options("${MAIN_EXECUTABLE}" PRIVATE "LINKER:--no-as-needed")
target_link_options("${LOCAL_TEST_EXECUTABLE}" PRIVATE "LINKER:--no-as-needed")

target_compile_definitions("${MAIN_EXECUTABLE}" PRIVATE MKL_ILP64)
target_compile_definitions("${LOCAL_TEST_EXECUTABLE}" PRIVATE MKL_ILP64)

target_include_directories("${MAIN_EXECUTABLE}" PRIVATE "$ENV{MKLROOT}/include" "$ENV{TBBROOT}/include")
target_include_directories("${LOCAL_TEST_EXECUTABLE}" PRIVATE "$ENV{MKLROOT}/include" "$ENV{TBBROOT}/include")

target_link_directories("${MAIN_EXECUTABLE}" PRIVATE "$ENV{MKLROOT}/lib/intel64" "$ENV{TBBROOT}/lib/intel64/gcc4.8")
target_link_directories("${LOCAL_TEST_EXECUTABLE}" PRIVATE "$ENV{MKLROOT}/lib/intel64" "$ENV{TBBROOT}/lib/intel64/gcc4.8")

target_link_libraries("${MAIN_EXECUTABLE}" PRIVATE MPI::MPI_C mkl_intel_ilp64 mkl_tbb_thread mkl_core pthread m dl tbb stdc++fs)
target_link_libraries("${LOCAL_TEST_EXECUTABLE}" PRIVATE mkl_intel_ilp64 mkl_tbb_thread mkl_core pthread m dl tbb stdc++fs)
target_link_libraries("${MPI_TEST_EXECUTABLE}" PRIVATE MPI::MPI_C)