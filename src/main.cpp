#include "node_antenna_input_assigner.hpp"

#include <iostream>

int main() {
	auto ranges = assignNodeAntennaInputs(4, 4);

    for (auto const& i : ranges) {
        std::cout << i.value().begin;
	}
}
