#pragma once

#include <map>
#include <set>
#include <vector>


struct AppConfig {
	std::string inputDirectoryPath;
	unsigned int observationID;
	unsigned int signalStartTime;
	std::string invPolyphaseFilterPath;
	std::string outputDirectoryPath;
};


struct AntennaInputPhysID {
	unsigned int tile;
	char signalChain;
};

struct AntennaConfig {
	std::vector<AntennaInputPhysID> antennaInputs;
	std::set<unsigned int> frequencyChannels;
};


struct AntennaInputProcessingResults {
	bool success;
	std::set<unsigned int> usedChannels;
};

struct ObservationProcessingResults {
	std::map<unsigned int, AntennaInputProcessingResults> results;
};