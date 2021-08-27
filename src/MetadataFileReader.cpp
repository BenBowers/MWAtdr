#include "MetadataFileReader.hpp"

#include <filesystem>
#include <regex>
#include <string>
#include <vector>

// TODO: delete
#include <iostream>

namespace fs = std::filesystem;

// Constructor which creates internal VoltageContext
MetadataFileReader::MetadataFileReader(AppConfig const appConfig) {
	auto metafitsFilename = appConfig.observationID + ".metafits";
	auto voltageFiles = findVoltageFiles(appConfig);
}

std::vector<std::string> MetadataFileReader::findVoltageFiles(AppConfig const appConfig) {
	std::vector<std::string> voltageFilenames;
	std::string target = appConfig.inputDirectoryPath + "/" +
	                     std::to_string(appConfig.observationID) + "_" +
	                     std::to_string(appConfig.signalStartTime) + "_";
    std::regex channel ("[0-9]+");
	std::string fileType = ".sub";
    // Detect and store voltage data file paths
    for (auto const& file : fs::directory_iterator(appConfig.inputDirectoryPath)) {
		auto path = std::string(file.path());
		// Add paths that match the file format (observationID_signalStartTime_channel.sub)
		if (path.length() > target.length()) {
		    if (target.compare(path.substr(0, target.length())) == 0 &&
			    std::regex_search(path.substr(target.length()), channel) &&
				fileType.compare(path.substr(path.length() - fileType.length())) == 0) {
			    voltageFilenames.push_back(path);
		    }
		}
    }
	return voltageFilenames;
}

AntennaConfig MetadataFileReader::getAntennaConfig() {
    AntennaConfig config;

	// TODO: getPhysicalAntennaMappings()
	AntennaInputPhysID arr[] = {{0, 'X'}, {0, 'Y'}, {1, 'X'}, {1, 'Y'},
                                {6, 'X'}, {6, 'Y'}, {7, 'X'}, {7, 'Y'}};
	config.antennaInputs.assign(arr, arr + 8);

	// TODO: getFrequencyChannelsUsed()
    config.frequencyChannels.insert({124, 125, 127, 128, 129});

	return config;
}