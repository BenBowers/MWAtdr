#include "node_antenna_input_assigner.hpp"

std::vector<std::optional<AntennaInputRange>> assignNodeAntennaInputs(unsigned numNodes, unsigned numAntennaInputs) {
    std::vector<std::optional<AntennaInputRange>> ranges;
    AntennaInputRange temp;

    for (int i = 0; i < numNodes; i++) {
        temp.begin = i;
        temp.end = i;
        ranges.push_back(temp);
    }

    return ranges;
}