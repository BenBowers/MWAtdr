#include "NodeAntennaInputAssigner.hpp"

// Creates a vector of size numNodes,
// with each AntennaInputRange entry covering a near equal distribution of antenna inputs.
// Returned vector contains AntennaInputRange or null entries.
std::vector<std::optional<AntennaInputRange>> assignNodeAntennaInputs(unsigned numNodes, unsigned numAntennaInputs) {
    // Throw exceptions for invalid arguments
    if (numNodes == 0) {
        throw std::invalid_argument{"numNodes must be > 0"};
    }
    else if (numAntennaInputs == 0) {
        throw std::invalid_argument{"numAntennaInputs must be > 0"};
    }

    std::vector<std::optional<AntennaInputRange>> ranges;
    AntennaInputRange temp;

    // Calculate average number (rounded down) of antenna inputs per node
    unsigned averageInputsToProcess = numAntennaInputs / numNodes;

    if (averageInputsToProcess >= 1) {
        // Assign the average number of antenna inputs to all nodes
        for (unsigned i = 0; i < numNodes; i++) {
            temp.begin = i * averageInputsToProcess;
            temp.end = temp.begin + averageInputsToProcess - 1;
            ranges.push_back(temp);
        }
        // Assign one additional antenna input to each node until all (inputs) have been assigned
        unsigned index = 0;
        while (ranges.back().value().end != numAntennaInputs - 1) {
            ranges.at(index).value().end++;
            index++;

            for (unsigned i = index; i < numNodes; i++) {
                ranges.at(i).value().begin++;
                ranges.at(i).value().end++;
            }
        }
    }
    else {
        // Assign one antenna input to each node until all (inputs) have been assigned
        for (unsigned i = 0; i < numAntennaInputs; i++) {
            temp = {i, i};
            ranges.push_back(temp);
        }
        // Assign null to the remaining nodes
        while (ranges.size() < numNodes) {
            ranges.push_back(std::nullopt);
        }
    }
    return ranges;
}

// Compare that two AntennaInputRange structs are not equal
bool operator!=(AntennaInputRange const& lhs, AntennaInputRange const& rhs) {
    return lhs.begin != rhs.begin || lhs.end != rhs.end;
}