#include <any>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include <mpi.h>

#include "../src/print.hpp"


// Test various C++ stuff, including C++17, MPI


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    
    int process_count = 1;
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    
    int rank = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
	std::string const message = "Hello from process " + std::to_string(rank) + " of " + std::to_string(process_count);
	
    std::optional opt{std::any{std::string_view{message}}};
    auto const message_view = std::any_cast<std::string_view>(opt.value());
    
	print(message_view);
    
    MPI_Finalize();
}
