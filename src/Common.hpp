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


// Struct comparisons mainly for testing purposes.
bool operator==(AppConfig const& lhs, AppConfig const& rhs);
bool operator==(AntennaInputPhysID const& lhs, AntennaInputPhysID const& rhs);
bool operator==(AntennaConfig const& lhs, AntennaConfig const& rhs);
bool operator==(AntennaInputProcessingResults const& lhs, AntennaInputProcessingResults const& rhs);
bool operator==(ObservationProcessingResults const& lhs, ObservationProcessingResults const& rhs);
