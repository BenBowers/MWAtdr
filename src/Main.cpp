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

#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>


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
	    runNode(node, argc, argv);
	}, communicator);
}


// Run by the primary node
void runNode(PrimaryNodeCommunicator const& primary, int argc, char* argv[]) {
	bool startupStatus = false;

    // Create AppConfig from command line arguments
    auto appConfig = createAppConfig(argc, argv);

	// Read in metadata
	AntennaConfig antennaConfig;
	try {
		MetadataFileReader mfr(appConfig);
		antennaConfig = mfr.getAntennaConfig();
		startupStatus = true;
	}
	catch (MetadataException const& e) {
		std::cout << e.what();
	}

    // Send startup status to secondary nodes
	primary.sendAppStartupStatus(startupStatus);

	// Send app configuration to secondary nodes
    primary.sendAppConfig(appConfig);

	// Receive setup status from all secondary nodes
	auto const nodeStatus = primary.receiveNodeSetupStatus();


	// If any nodes fail, do we continue with the working nodes?


	// Compute frequency channel remapping
	const unsigned ORIGINAL_SAMPLING_FREQUENCY = 512;
	auto const channelRemapping = computeChannelRemapping(ORIGINAL_SAMPLING_FREQUENCY, antennaConfig.frequencyChannels);

	// Send antenna configuration to secondary nodes
	primary.sendAntennaConfig(antennaConfig);

	// Send channel remapping to secondary nodes
	primary.sendChannelRemapping(channelRemapping);

    // Send antenna input assignments to secondary nodes
	auto antennaInputRange = communicateNodeAntennaInputAssignment(primary, antennaConfig.antennaInputs.size());

	// Read in filter coefficients
	try {
	    auto const coefficients = readCoeData(appConfig.invPolyphaseFilterPath);
	}
	catch (ReadCoeDataException const& e) {
		std::cout << e.what();
	}

    // Read in raw signal, process, and write new signal to file for each assigned antenna input
    auto processingResults = processAssignedAntennaInputs(appConfig, antennaConfig, antennaInputRange);

	// Gather processing results from secondary nodes and merge into processingResults
    mergeSecondaryProcessingResults(primary, processingResults);

	// Write output log file
	writeLogFile(appConfig, channelRemapping, processingResults, antennaConfig);
}


// Run by all secondary nodes
void runNode(SecondaryNodeCommunicator const& secondary, int argc, char* argv[]) {
}


ObservationProcessingResults processAssignedAntennaInputs(AppConfig const& appConfig, AntennaConfig const& antennaConfig,
                                                          std::optional<AntennaInputRange> const& antennaInputRange) {
    ObservationProcessingResults processingResults;

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

			// Write antenna input processed signal to file
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