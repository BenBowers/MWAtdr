#pragma once

#include <map>
#include <set>
#include <vector>

// Contains the observation details, and input and output file directories
// Entered as command line arguments
struct AppConfig {
	std::string inputDirectoryPath;
	unsigned observationID;
	unsigned signalStartTime;
	std::string invPolyphaseFilterPath;
	std::string outputDirectoryPath;
};


struct AntennaInputPhysID {
	unsigned tile;
	char signalChain;
};

struct AntennaConfig {
	std::vector<AntennaInputPhysID> antennaInputs;
	std::set<unsigned> frequencyChannels;
};


struct AntennaInputProcessingResults {
	bool success;
	std::set<unsigned> usedChannels;
};

// Contains antenna input id as key, processing results as value
struct ObservationProcessingResults {
	std::map<unsigned, AntennaInputProcessingResults> results;
};