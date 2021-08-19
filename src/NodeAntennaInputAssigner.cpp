#include "NodeAntennaInputAssigner.hpp"

// Creates a vector of size numNodes,
// with each AntennaInputRange entry covering a near equal distribution of antenna inputs.
// Returned vector contains AntennaInputRange or null entries.
std::vector<std::optional<AntennaInputRange>> assignNodeAntennaInputs(unsigned numNodes, unsigned numAntennaInputs) {
    std::vector<std::optional<AntennaInputRange>> ranges;
    AntennaInputRange temp;

    // Calculate average number (rounded down) of antenna inputs per node
    unsigned averageInputsToProcess = numAntennaInputs / numNodes;

    // Assign each node the average number of antenna inputs
    for (int i = 0; i < numNodes; i++) {
        temp.begin = i * averageInputsToProcess;
        temp.end = (i + 1) * (averageInputsToProcess - 1);
        ranges.push_back(temp);
    }

    return ranges;
}