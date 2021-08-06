#pragma once

#include <map>
#include <set>
#include <vector>


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
	std::set<unsigned int> usedChannels;
};

struct ObservationProcessingResults {
	std::map<unsigned int, AntennaInputProcessingResults> results;
};