#include "MetadataFileReader.hpp"

#include <mwalib.h>

#include <filesystem>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>
// TODO: delete
#include <iostream>

namespace fs = std::filesystem;

// WARNING: This function is an abomination, I'm truly sorry.
// But also, probably don't try to make it better.

// Constructor which creates internal VoltageContext
MetadataFileReader::MetadataFileReader(AppConfig const appConfig) {
	auto const metafitsFilename = appConfig.inputDirectoryPath +
	                              std::to_string(appConfig.observationID) + ".metafits";
	auto const voltageFiles = findVoltageFiles(appConfig);
	unsigned const numFiles = voltageFiles.size();
    // TODO: Delete this probably
	for (auto i : voltageFiles) {
		std::cout << "Voltage file detected: " << i << std::endl;
	}
	// Copy filenames vector to char* array to create VoltageContext
	const char* filenamesArray[numFiles];
	for (unsigned i = 0; i < numFiles; i++) {
		filenamesArray[i] = voltageFiles.at(i).c_str();
	}
	// Create VoltageContext
	const char* errorMessage;
	const char* metafits = metafitsFilename.c_str();
	if (mwalib_voltage_context_new(metafits, filenamesArray, numFiles,
	    &voltageContext, errorMessage, 1024) != EXIT_SUCCESS) {
		// TODO: throw exception
		std::cout << "Error creating voltage context: " << errorMessage;
	}
}

std::vector<std::string> MetadataFileReader::findVoltageFiles(AppConfig const appConfig) {
	std::vector<std::string> voltageFilenames;
	std::string target = appConfig.inputDirectoryPath +
	                     std::to_string(appConfig.observationID) + "_" +
	                     std::to_string(appConfig.signalStartTime) + "_";
    std::regex channelAndFileType ("[0-9]+\\.sub");
    // Detect and store voltage data file paths
    for (auto const& file : fs::directory_iterator(appConfig.inputDirectoryPath)) {
		auto path = std::string(file.path());
		// Add paths that match the file format (inputDirectoryPath/observationID_signalStartTime_channel.sub)
		if (path.length() > target.length()) {
		    if (target == path.substr(0, target.length()) &&
			    std::regex_match(path.substr(target.length()), channelAndFileType)) {//&&
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