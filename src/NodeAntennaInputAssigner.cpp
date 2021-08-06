#include "NodeAntennaInputAssigner.hpp"

// Creates a vector of size numNodes,
// with each AntennaInputRange covering a near equal distribution of antenna inputs.
// Returned vector contains AntennaInputRange or null entries.
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