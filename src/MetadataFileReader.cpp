#include "MetadataFileReader.hpp"

#include <filesystem>
#include <iostream>
#include <regex>
#include <set>
#include <stdexcept>
#include <vector>

// Used in VoltageContext and MetafitsMetadata creation
#define ERROR_MESSAGE_LENGTH 1024
// Used to check if voltage files are valid
#define VOLTAGE_FILE_SIZE 5275652096

namespace fs = std::filesystem;

// WARNING: This module is an abomination, I'm truly sorry.
// But also, probably don't try to make it better.
// Any function containing mwalib functionality is forced to use inconvenient data types

// Constructor which creates internal MetafitsMetadata (based on VoltageContext)
MetadataFileReader::MetadataFileReader(AppConfig const appConfig) {
	// Verify metafits file exists at specified input directory path
	if (!validMetafits(appConfig)) {
		throw std::runtime_error("Invalid/no metafits file at specified path");
	}
	// Verify voltage files exist at specified input directory path
	auto const voltageFiles = findVoltageFiles(appConfig);
	unsigned const numFiles = voltageFiles.size();
	if (numFiles == 0) {
		throw std::runtime_error("Invalid/no voltage files at specified path");
	}
	// Copy filenames vector to char* array to create VoltageContext
	const char* filenamesArray[numFiles];
	for (unsigned i = 0; i < numFiles; i++) {
		filenamesArray[i] = voltageFiles.at(i).c_str();
	}
	// Create VoltageContext for use in MetafitsMetadata creation
	std::string metafitsFilename = appConfig.inputDirectoryPath +
	                               std::to_string(appConfig.observationID) + ".metafits";
	const char* errorMessage;
	const char* metafits = metafitsFilename.c_str();
	if (mwalib_voltage_context_new(metafits, filenamesArray, numFiles, &voltageContext,
	                               errorMessage, ERROR_MESSAGE_LENGTH) != EXIT_SUCCESS) {
		// Error creating VoltageContext
		throw MetadataException();
	}
	// Create MetafitsMetadata from VoltageContext
	if (mwalib_metafits_metadata_get(nullptr, nullptr, voltageContext, &metafitsMetadata,
	                                 errorMessage, ERROR_MESSAGE_LENGTH) != EXIT_SUCCESS) {
		// Error loading metadata
		throw MetadataException();
    }
}
// Finds all of the observation signal files within the specified directory
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
			    std::regex_match(path.substr(target.length()), channelAndFileType)) {
				// Check if voltage file is valid
				if (fs::file_size(file) == VOLTAGE_FILE_SIZE) {
			        voltageFilenames.push_back(path);
				}
		    }
		}
    }
	return voltageFilenames;
}
// Checks if metafits file exists and is not empty
bool MetadataFileReader::validMetafits(AppConfig const appConfig) {
    for (auto const& file : fs::directory_iterator(appConfig.inputDirectoryPath)) {
		auto path = std::string(file.path());
		if (path.compare(appConfig.inputDirectoryPath +
		                 std::to_string(appConfig.observationID) + ".metafits") == 0) {
			if (!fs::is_empty(file)) {
				return true;
			}
		}
	}
	return false;
}


// Gathers antenna configuration from the metadata
AntennaConfig MetadataFileReader::getAntennaConfig() {
    AntennaConfig config;
	// Storing the logical to physical antenna mappings
    config.antennaInputs = getPhysicalAntennaInputs();
	// Storing the frequency channels recorded in the observation
	config.frequencyChannels = getFrequencyChannelsUsed();
	return config;
}

std::vector<AntennaInputPhysID> MetadataFileReader::getPhysicalAntennaInputs() {
	std::vector<AntennaInputPhysID> antennaInputs;
	// Convert each Rfinput to AntennaInputPhysID and add to the vector
	for (unsigned i = 0; i < metafitsMetadata->num_rf_inputs; i++) {
		antennaInputs.push_back({metafitsMetadata->rf_inputs[i].tile_id, *(metafitsMetadata->rf_inputs[i].pol)});
	}
	return antennaInputs;
}

std::set<unsigned> MetadataFileReader::getFrequencyChannelsUsed() {
	std::set<unsigned> frequencyChannels;
	// Add each frequency channel number recorded in the observation
    for (unsigned i = 0; i < metafitsMetadata->num_metafits_coarse_chans; i++) {
		frequencyChannels.insert(metafitsMetadata->metafits_coarse_chans[i].rec_chan_number);
	}
	return frequencyChannels;
}