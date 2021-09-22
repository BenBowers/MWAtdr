#include "ChannelRemapping.hpp"
#include "CommandLineArguments.hpp"
#include "Common.hpp"
#include "InternodeCommunication.hpp"
#include "MetadataFileReader.hpp"
#include "NodeAntennaInputAssigner.hpp"
#include "OutputLogFileWriter.hpp"
#include "OutSignalWriter.hpp"
#include "ReadCoeData.hpp"
#include "ReadInputFile.hpp"
#include "SignalProcessing.hpp"

#include <complex>
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>


class NodeException : public std::runtime_error {
	public:
	    NodeException(const std::string& message) : std::runtime_error(message) {}
};


// Visitor functions for the primary or secondary nodes
void runNode(PrimaryNodeCommunicator const& primary, int argc, char* argv[]);
void runNode(SecondaryNodeCommunicator const& secondary, int argc, char* argv[]);

ObservationProcessingResults processAssignedAntennaInputs(AppConfig const& appConfig, AntennaConfig const& antennaConfig,
                                                          std::optional<AntennaInputRange> const& antennaInputRange);
void mergeSecondaryProcessingResults(PrimaryNodeCommunicator const& primary, ObservationProcessingResults& processingResults);

std::optional<AntennaInputRange> communicateNodeAntennaInputAssignment(PrimaryNodeCommunicator const& primary,
                                                                       unsigned const numAntennaInputs);


int main(int argc, char* argv[]) {
    // Initialise InternodeCommunicator singleton
    auto const communicatorContext = InternodeCommunicationContext::initialise();
    auto const communicator = communicatorContext->getCommunicator();

	std::visit([argc, argv](auto const& node) {
		try {
	        runNode(node, argc, argv);
		}
		catch (NodeException const& e) {
			std::cout << e.what();
		}
	}, communicator);
}


// Run by the primary node
void runNode(PrimaryNodeCommunicator const& primary, int argc, char* argv[]) {
    bool startupStatus = false;

    // Create AppConfig from command line arguments
    auto const appConfig = createAppConfig(argc, argv);

	// Read in metadata
	AntennaConfig antennaConfig;
	try {
		MetadataFileReader mfr(appConfig);
		antennaConfig = mfr.getAntennaConfig();
	}
	catch (MetadataException const& e) {
		std::cout << e.what();
	}

    // Read in filter coefficients
    std::vector<std::complex<float>> coefficients;
    try {
        coefficients = readCoeData(appConfig.invPolyphaseFilterPath);
        startupStatus = true;
    }
    catch (ReadCoeDataException const& e) {
        std::cout << e.what();
    }

    // Send startup status to secondary nodes
    primary.sendAppStartupStatus(startupStatus);

    // Terminate node on startup failure
    if (!startupStatus) {
        throw NodeException("Primary node startup failure, terminating node");
    }

    // Receive setup status from all secondary nodes
    auto const nodeStatus = primary.receiveNodeSetupStatus();

    // Send app configuration to secondary nodes
    primary.sendAppConfig(appConfig);

    // Send antenna configuration to secondary nodes
    primary.sendAntennaConfig(antennaConfig);

    // Compute frequency channel remapping
    const unsigned ORIGINAL_SAMPLING_FREQUENCY = 512;
    auto const channelRemapping = computeChannelRemapping(ORIGINAL_SAMPLING_FREQUENCY, antennaConfig.frequencyChannels);

    // Send channel remapping to secondary nodes
    primary.sendChannelRemapping(channelRemapping);

    // Send antenna input assignments to secondary nodes
    auto const antennaInputRange = communicateNodeAntennaInputAssignment(primary, antennaConfig.antennaInputs.size());

    // Read in raw signal, process, and write new signal to file for each assigned antenna input
    auto processingResults = processAssignedAntennaInputs(appConfig, antennaConfig, antennaInputRange);

    // Gather processing results from secondary nodes and merge into processingResults
    mergeSecondaryProcessingResults(primary, processingResults);

    // Write output log file
    writeLogFile(appConfig, channelRemapping, processingResults, antennaConfig);
}


// Run by all secondary nodes
void runNode(SecondaryNodeCommunicator const& secondary, int argc, char* argv[]) {
    bool setupStatus = false;

    // Receive primary node startup status
	if (!secondary.receiveAppStartupStatus()) {
		// Terminate node on primary startup failure
        throw NodeException("Node " + std::to_string(secondary.getNodeID()) +
                            ": Primary node startup failure, terminating node");
	}

    // Receive app configuration from primary node
    auto const appConfig = secondary.receiveAppConfig();

    // Read in filter coefficients
    std::vector<std::complex<float>> coefficients;
    try {
        coefficients = readCoeData(appConfig.invPolyphaseFilterPath);
        setupStatus = true;
    }
    catch (ReadCoeDataException const& e) {
        std::cout << e.what();
    }

    // Send setup status to primary node
    secondary.sendNodeSetupStatus(setupStatus);

    // Receive antenna configuration from primary node
	auto const antennaConfig = secondary.receiveAntennaConfig();

    // Receive channel remapping from primary node
	auto const channelRemapping = secondary.receiveChannelRemapping();

    // Receive antenna input assignment from primary node
    auto const antennaInputRange = secondary.receiveAntennaInputAssignment();

    // Read in raw signal, process, and write new signal to file for each assigned antenna input
    auto const processingResults = processAssignedAntennaInputs(appConfig, antennaConfig, antennaInputRange);

	// Send processing results to primary node
	secondary.sendProcessingResults(processingResults);
}


ObservationProcessingResults processAssignedAntennaInputs(AppConfig const& appConfig, AntennaConfig const& antennaConfig,
                                                          std::optional<AntennaInputRange> const& antennaInputRange) {
    ObservationProcessingResults processingResults;

    // Process all assigned antenna inputs (if any)
    if (antennaInputRange.has_value()) {
		for (unsigned index = antennaInputRange.value().begin; index <= antennaInputRange.value().end; index++) {
			// Used to store raw signal data from all channels recorded by one antenna input
	        std::vector<std::vector<std::complex<float>>> antennaInputSignals;
			// Used to store processed signal for one antenna input
			std::vector<std::int16_t> processedSignal;
			// Used to store which channels are used in the processed signal
			std::set<unsigned> usedChannels;

			// Read in raw signal files from all channels recorded by one antenna input
			for (auto channel : antennaConfig.frequencyChannels) {
				std::string filename = std::to_string(appConfig.observationID) + "_" +
				                       std::to_string(appConfig.signalStartTime) + "_" +
									   std::to_string(channel) + ".sub";

				if (validateInputData(filename)) {
					antennaInputSignals.push_back(readInputDataFile(filename, index));
				}
			}

			// Process signal
			//processSignal();

			// Write processed antenna input signal to file
			try {
				outSignalWriter(processedSignal, appConfig, antennaConfig.antennaInputs.at(index));
				processingResults.results.insert({index, {true, usedChannels}});
				// print antenna input processed successfully
			}
			catch (OutSignalException const& e) {
				processingResults.results.insert({index, {false, usedChannels}});
				// print antenna input processing failed
			}
		}
	}
	return processingResults;
}


void mergeSecondaryProcessingResults(PrimaryNodeCommunicator const& primary, ObservationProcessingResults& processingResults) {
    // Gather secondary node processing results
    auto secondaryProcessingResults = primary.receiveProcessingResults();
	// Merge secondary node processing results into primary processing results
	for (unsigned i = 1; i < primary.getNodeCount(); i++) {
		processingResults.results.merge(secondaryProcessingResults.at(i).results);
	}
}


std::optional<AntennaInputRange> communicateNodeAntennaInputAssignment(PrimaryNodeCommunicator const& primary,
                                                                       unsigned const numAntennaInputs) {
    auto antennaInputAssignments = assignNodeAntennaInputs(primary.getNodeCount(), numAntennaInputs);

	// Send antenna input assignment to secondary nodes
	for (unsigned i = 1; i < primary.getNodeCount(); i++) {
		primary.sendAntennaInputAssignment(i, antennaInputAssignments.at(i));
	}

	return antennaInputAssignments.at(0);
}