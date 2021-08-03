#pragma once
#include <vector>

struct AntennaInputRange {
	unsigned int begin;
	unsigned int end;
};

std::vector<AntennaInputRange> assignNodeAntennaInputs(int numNodes, unsigned int numAntennaInputs);