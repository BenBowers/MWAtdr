#include <any>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <mkl.h>
#include <tbb/tbb.h>

#include "../src/print.hpp"


// Test various C++ stuff, including C++17, MKL, TBB


int main(int argc, char* argv[]) {
    std::vector<double, tbb::cache_aligned_allocator<double>> v1(10);
    std::vector<double, tbb::cache_aligned_allocator<double>> v2(10);
    std::vector<double, tbb::cache_aligned_allocator<double>> v3(10, 0.0f);
    std::iota(v1.begin(), v1.end(), 0);
    std::iota(v2.begin(), v2.end(), 1);
    vdAdd(10, v1.data(), v2.data(), v3.data());
    
    auto const vector_sum = tbb::parallel_reduce(
        tbb::blocked_range(v3.cbegin(), v3.cend()), 0.0,
        [](auto const& range, auto acc) {
            return std::accumulate(range.begin(), range.end(), acc);
        },
        std::plus<>()
    );
    
	std::string const message = "Sum is " + std::to_string(vector_sum);
	
    std::optional opt{std::any{std::string_view{message}}};
    auto const message_view = std::any_cast<std::string_view>(opt.value());
    
	print(message_view);
}
