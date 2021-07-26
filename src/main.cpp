#include <any>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <mpi.h>
#include <mkl.h>
#include <tbb/tbb.h>

#include "print.hpp"


// Test various C++ stuff, including C++17, MPI, MKL, TBB


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    
    int process_count = 1;
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    
    int rank = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    std::vector<double, tbb::cache_aligned_allocator<double>> v1(10);
    std::vector<double, tbb::cache_aligned_allocator<double>> v2(10);
    std::vector<double, tbb::cache_aligned_allocator<double>> v3(10, 0.0f);
    std::iota(v1.begin(), v1.end(), rank);
    std::iota(v2.begin(), v2.end(), rank + 1);
    vdAdd(10, v1.data(), v2.data(), v3.data());
    
    auto const vector_sum = tbb::parallel_reduce(
        tbb::blocked_range(v3.cbegin(), v3.cend()), 0.0,
        [](auto const& range, auto acc) {
            return std::accumulate(range.begin(), range.end(), acc);
        },
        std::plus<>()
    );
    
	std::string const message = "Hello from process " + std::to_string(rank) + " of " + std::to_string(process_count)
        + ", sum is " + std::to_string(vector_sum);
	
    std::optional opt{std::any{std::string_view{message}}};
    auto const message_view = std::any_cast<std::string_view>(opt.value());
    
	print(message_view);
    
    MPI_Finalize();
}
