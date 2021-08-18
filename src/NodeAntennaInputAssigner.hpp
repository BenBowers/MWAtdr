#pragma once

#include <optional>
#include <vector>

struct AntennaInputRange {
	unsigned int begin;
	unsigned int end;
};

std::vector<std::optional<AntennaInputRange>> assignNodeAntennaInputs(unsigned numNodes, unsigned numAntennaInputs);


// Comparison mainly for testing purposes.
bool operator==(AntennaInputRange const& lhs, AntennaInputRange const& rhs);
