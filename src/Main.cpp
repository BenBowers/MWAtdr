#include "ChannelRemapping.hpp"
#include "Common.hpp"
#include "InternodeCommunication.hpp"
#include "MetadataFileReader.hpp"
#include "NodeAntennaInputAssigner.hpp"
#include "OutputLogFileWriter.hpp"
#include "OutSignalWriter.hpp"
#include "ReadCoeData.hpp"
#include "ReadInputFile.hpp"
#include "SignalProcessing.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>


void runNode(PrimaryNodeCommunicator const& primary, int argc, char* argv[]);
void runNode(SecondaryNodeCommunicator const& primary, int argc, char* argv[]);

// Command line validation functions throw std::invalid_argument
std::string validateInputDirectoryPath(std::string const inputDirectoryPath);
unsigned long long validateObservationID(std::string const observationID);
unsigned long long validateSignalStartTime(unsigned long long const observationID, std::string signalStartTime);
std::string validateInvPolyphaseFilterPath(std::string const invPolyphaseFilterPath);
std::string validateOutputDirectoryPath(std::string const outputDirectoryPath);
bool validateIgnoreErrors(std::string const ignoreErrors);


int main(int argc, char* argv[]) {
    // Initialise InternodeCommunicator singleton
    auto const communicatorContext = InternodeCommunicationContext::initialise();
    auto const communicator = communicatorContext->getCommunicator();

	std::visit([argc, argv](auto const& node) {
	    runNode(node, argc, argv);
	}, communicator);
}

void runNode(PrimaryNodeCommunicator const& primary, int argc, char* argv[]) {
	bool startupStatus = true;

	// Validate command line arguments
	if (argc != 7) {
		throw std::invalid_argument{"Invalid number of command line arguments"};
	}

    // Read in app configuration from command line arguments
	AppConfig appConfig;
	appConfig.inputDirectoryPath = validateInputDirectoryPath(argv[1]);
	appConfig.observationID = validateObservationID(argv[2]);
	appConfig.signalStartTime = validateSignalStartTime(appConfig.observationID, argv[3]);
	appConfig.invPolyphaseFilterPath = validateInvPolyphaseFilterPath(argv[4]);
	appConfig.outputDirectoryPath = validateOutputDirectoryPath(argv[5]);
	appConfig.ignoreErrors = validateIgnoreErrors(argv[6]);

	// Read in metadata
	AntennaConfig antennaConfig;
	try {
		MetadataFileReader mfr(appConfig);
		antennaConfig = mfr.getAntennaConfig();
	}
	catch (MetadataException const& e) {
		std::cout << e.what();
		startupStatus = false;
	}

    // Communicate startup status and app configuration
	primary.sendAppStartupStatus(startupStatus);
    primary.sendAppConfig(appConfig);
	auto const nodeStatus = primary.receiveNodeSetupStatus();

	// Compute frequency channel remapping
	const unsigned ORIGINAL_SAMPLING_FREQUENCY = 512;
	auto const channelRemapping = computeChannelRemapping(ORIGINAL_SAMPLING_FREQUENCY, antennaConfig.frequencyChannels);

	// Communicate antenna configuration, channel remapping, and antenna input assignment
	primary.sendAntennaConfig(antennaConfig);
	primary.sendChannelRemapping(channelRemapping);

	std::optional<AntennaInputRange> antennaInputRange;
	auto antennaInputAssignments = assignNodeAntennaInputs(primary.getNodeCount(), antennaConfig.antennaInputs.size());
	antennaInputRange = antennaInputAssignments.at(0);
	for (unsigned i = 1; i < primary.getNodeCount(); i++) {
		primary.sendAntennaInputAssignment(i, antennaInputAssignments.at(i));
	}

	// Read in filter coefficients
	try {
	    auto const coefficients = readCoeData(appConfig.invPolyphaseFilterPath);
	}
	catch (ReadCoeDataException const& e) {
		std::cout << e.what();
	}

    // Read in raw signal, process, and write new signal to file for each assigned antenna input
    // TODO: ObservationProcessingResults processAssignedAntennaInputs();
	ObservationProcessingResults processingResults;

    if (antennaInputRange.has_value()) {
		for (unsigned i = antennaInputRange.value().begin; i <= antennaInputRange.value().end; i++) {
			// Read in raw signal file
			// Process signal
			// Write processed signal to file
		}
	}

    // Gather secondary node processing results
    auto secondaryProcessingResults = primary.receiveProcessingResults();
	for (unsigned i = 1; i < primary.getNodeCount(); i++) {
		processingResults.results.merge(secondaryProcessingResults.at(i).results);
	}

	// Write output log file
	writeLogFile(appConfig, channelRemapping, processingResults, antennaConfig);
}


void runNode(SecondaryNodeCommunicator const& primary, int argc, char* argv[]) {
}


std::string validateInputDirectoryPath(std::string const inputDirectoryPath) {
	std::filesystem::path directory (inputDirectoryPath);

	if (!std::filesystem::exists(directory) ||
        !std::filesystem::is_directory(directory) ||
        std::filesystem::is_empty(directory)) {
		throw std::invalid_argument {"Invalid input directory path"};
	}
	return (std::string) directory;
}

unsigned long long validateObservationID(std::string const observationID) {
	auto id = std::stoull(observationID);

	if (id % 8 != 0) {
		throw std::invalid_argument {"Invalid observation ID, must be divisible by 8"};
	}
	return id;
}

unsigned long long validateSignalStartTime(unsigned long long const observationID, std::string signalStartTime) {
	auto time = std::stoull(signalStartTime);

	if (time % 8 != 0) {
		throw std::invalid_argument {"Invalid signal start time, must be divisible by 8"};
	}
	else if (time < observationID) {
		throw std::invalid_argument {"Invalid signal start time, must be greater than or equal to observation ID"};
	}
	return time;
}

std::string validateInvPolyphaseFilterPath(std::string const invPolyphaseFilterPath) {
	std::filesystem::path file (invPolyphaseFilterPath);

	if (!std::filesystem::exists(file) || !std::filesystem::is_regular_file(file)) {
		throw std::invalid_argument {"Invalid inverse polyphase filter path"};
	}
	return (std::string) file;
}

std::string validateOutputDirectoryPath(std::string const outputDirectoryPath) {
	std::filesystem::path directory (outputDirectoryPath);

	if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
		throw std::invalid_argument {"Invalid output directory path"};
	}
	return (std::string) directory;
}

bool validateIgnoreErrors(std::string const ignoreErrors) {
	bool ignore = false;

	if (ignoreErrors.compare("true") == 0) {
		ignore = true;
	}
    else if (ignoreErrors.compare("false") != 0) {
		throw std::invalid_argument {"Ignore errors argument must be 'true' or 'false'"};
	}
	return ignore;
}