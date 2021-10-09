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

AntennaConfig createAntennaConfig(AppConfig const& appConfig, bool& success);
std::vector<std::complex<float>> createFilterCoefficients(std::string const filterPath, bool& success);
std::optional<AntennaInputRange> communicateNodeAntennaInputAssignment(PrimaryNodeCommunicator const& primary,
                                                                       std::map<unsigned, bool> const& secondaryNodeStatus,
                                                                       unsigned const numAntennaInputs);
unsigned getActiveNodeCount(std::map<unsigned, bool> const& secondaryNodeStatus);

ObservationProcessingResults processAssignedAntennaInputs(AppConfig const& appConfig, AntennaConfig const& antennaConfig,
                                                          std::optional<AntennaInputRange> const& antennaInputRange,
                                                          std::vector<std::complex<float>> const& coefficients,
                                                          ChannelRemapping const& channelRemapping);
void readRawSignalFiles(AppConfig const& appConfig, AntennaConfig const& antennaConfig,
                        std::vector<std::vector<std::complex<float>>>& antennaInputSignals, std::set<unsigned>& usedChannels);

void mergeSecondaryProcessingResults(PrimaryNodeCommunicator const& primary, ObservationProcessingResults& processingResults);


int main(int argc, char* argv[]) {
    // Initialise InternodeCommunicator singleton
    auto const communicatorContext = InternodeCommunicationContext::initialise();
    auto const communicator = communicatorContext->getCommunicator();

	std::visit([argc, argv](auto const& node) {
		try {
	        runNode(node, argc, argv);
		}
		catch (NodeException const& e) {
			std::cout << e.what() << std::endl;
		}
	}, communicator);
}


// Run by the primary node
void runNode(PrimaryNodeCommunicator const& primary, int argc, char* argv[]) {
    bool startupStatus = true;

    AppConfig appConfig;
    AntennaConfig antennaConfig;
    std::vector<std::complex<float>> coefficients;

    try {
        // Create AppConfig from command line arguments
        appConfig = createAppConfig(argc, argv);
        
        // Read in metadata
        antennaConfig = createAntennaConfig(appConfig, startupStatus);

        // Read in filter coefficients
        coefficients = createFilterCoefficients(appConfig.invPolyphaseFilterPath, startupStatus);
    }
    catch (std::invalid_argument const& e) {
        startupStatus = false;
        std::cout << "Node 0 (Primary): " << e.what() << std::endl;
    }

    // Wait for primary to start up
    primary.synchronise();

    // Send startup status to secondary nodes
    primary.sendAppStartupStatus(startupStatus);

    // Terminate node on startup failure
    if (!startupStatus) {
        throw NodeException("Node 0 (Primary): Primary node startup failure, terminating node");
    }

    std::cout << "Node 0 (Primary): Successful startup" << std::endl;

    // Send app configuration to secondary nodes
    std::cout << "Node 0 (Primary): Sending app configuration to secondary nodes" << std::endl;
    primary.sendAppConfig(appConfig);

    // Wait for secondary nodes to set up
    primary.synchronise();

    // Receive setup status from all secondary nodes
    std::cout << "Node 0 (Primary): Receiving setup status from secondary nodes" << std::endl;
    auto const secondaryNodeStatus = primary.receiveNodeSetupStatus();

    // Send antenna configuration to secondary nodes
    std::cout << "Node 0 (Primary): Sending antenna configuration to secondary nodes" << std::endl;
    primary.sendAntennaConfig(antennaConfig);

    // Compute frequency channel remapping
    const unsigned ORIGINAL_SAMPLING_FREQUENCY = 512;
    auto const channelRemapping = computeChannelRemapping(ORIGINAL_SAMPLING_FREQUENCY, antennaConfig.frequencyChannels);

    // Send channel remapping to secondary nodes
    std::cout << "Node 0 (Primary): Sending channel remapping to secondary nodes" << std::endl;
    primary.sendChannelRemapping(channelRemapping);

    // Send antenna input assignments to secondary nodes
    std::cout << "Node 0 (Primary): Sending antenna input assignments to secondary nodes" << std::endl;
    auto const antennaInputRange = communicateNodeAntennaInputAssignment(primary, secondaryNodeStatus, antennaConfig.antennaInputs.size());

    // Wait for all nodes to be ready for signal processing
    primary.synchronise();

    // Read in raw signal, process, and write new signal to file for each assigned antenna input
    std::cout << "Node 0 (Primary): Starting signal processing" << std::endl;
    auto processingResults = processAssignedAntennaInputs(appConfig, antennaConfig, antennaInputRange,
                                                          coefficients, channelRemapping);
    std::cout << "Node 0 (Primary): Finished signal processing" << std::endl;

    // Wait for all nodes to finish processing
    primary.synchronise();

    // Gather processing results from secondary nodes and merge into processingResults
    mergeSecondaryProcessingResults(primary, processingResults);
    std::cout << "Node 0 (Primary): Received processing results from secondary nodes" << std::endl;

    // Write output log file
    std::cout << "Node 0 (Primary): Writing output log file" << std::endl;
    writeLogFile(appConfig, channelRemapping, processingResults, antennaConfig);

    std::cout << "Node 0 (Primary): Terminated succesfully" << std::endl;
}


// Run by all secondary nodes
void runNode(SecondaryNodeCommunicator const& secondary, int argc, char* argv[]) {
    bool setupStatus = true;

    // Wait for primary to start up
    secondary.synchronise();

    // Receive primary node startup status
    if (!secondary.receiveAppStartupStatus()) {
        // Terminate node on primary startup failure
        throw NodeException("Node " + std::to_string(secondary.getNodeID()) +
                            ": Primary node startup failure, terminating node");
    }

    // Receive app configuration from primary node
    std::cout << "Node " + std::to_string(secondary.getNodeID()) +
                 ": Receiving app configuration" << std::endl;
    auto const appConfig = secondary.receiveAppConfig();

    // Read in filter coefficients
    std::cout << "Node " + std::to_string(secondary.getNodeID()) +
                 ": Reading in filter coefficients" << std::endl;
    auto const coefficients = createFilterCoefficients(appConfig.invPolyphaseFilterPath, setupStatus);

    // Wait for secondary nodes to set up
    secondary.synchronise();

    // Send setup status to primary node
    secondary.sendNodeSetupStatus(setupStatus);

    // Terminate node on startup failure
    if (!setupStatus) {
        throw NodeException("Node " + std::to_string(secondary.getNodeID()) +
                            ": startup failure, terminating node");
    }

    std::cout << "Node " + std::to_string(secondary.getNodeID()) +
                 ": Successful startup" << std::endl;

    // Receive antenna configuration from primary node
	auto const antennaConfig = secondary.receiveAntennaConfig();
    std::cout << "Node " + std::to_string(secondary.getNodeID()) +
                 ": Received antenna configuration" << std::endl;

    // Receive channel remapping from primary node
	auto const channelRemapping = secondary.receiveChannelRemapping();
    std::cout << "Node " + std::to_string(secondary.getNodeID()) +
                 ": Received channel remapping" << std::endl;

    // Receive antenna input assignment from primary node
    auto const antennaInputRange = secondary.receiveAntennaInputAssignment();
    std::cout << "Node " + std::to_string(secondary.getNodeID()) +
                 ": Received antenna input assignment" << std::endl;

    // Wait for all nodes to be ready for signal processing
    secondary.synchronise();

    // Read in raw signal, process, and write new signal to file for each assigned antenna input
    std::cout << "Node " + std::to_string(secondary.getNodeID()) +
                 ": Starting signal processing" << std::endl;
    auto const processingResults = processAssignedAntennaInputs(appConfig, antennaConfig, antennaInputRange,
                                                                coefficients, channelRemapping);
    std::cout << "Node " + std::to_string(secondary.getNodeID()) +
                 ": Finished signal processing" << std::endl;

    // Wait for all nodes to finish processing
    secondary.synchronise();

	// Send processing results to primary node
    std::cout << "Node " + std::to_string(secondary.getNodeID()) +
                 ": Sending processing results" << std::endl;
	secondary.sendProcessingResults(processingResults);
    
    std::cout << "Node " + std::to_string(secondary.getNodeID()) +
                 ": Terminated successfully" << std::endl;
}


ObservationProcessingResults processAssignedAntennaInputs(AppConfig const& appConfig, AntennaConfig const& antennaConfig,
                                                          std::optional<AntennaInputRange> const& antennaInputRange,
                                                          std::vector<std::complex<float>> const& coefficients,
                                                          ChannelRemapping const& channelRemapping) {
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

            // Used to store antenna input being processed
            auto const antenna = antennaConfig.antennaInputs.at(index);

            if (!antenna.flagged) {
                // Read in raw signal files from all channels recorded by one antenna input
                readRawSignalFiles(appConfig, antennaConfig, antennaInputSignals, usedChannels);

                if (!antennaInputSignals.empty()) {
                    // Converting set of channels used to vector for use in processSignal()
                    std::vector<unsigned> channelIndexMapping(usedChannels.begin(), usedChannels.end());

                    // Process signal
                    std::cout << "Processing tile " << antenna.tile << antenna.signalChain << std::endl;
                    processSignal(antennaInputSignals, channelIndexMapping, processedSignal, coefficients, channelRemapping);

                    // Write processed antenna input signal to file
                    try {
                        outSignalWriter(processedSignal, appConfig, antenna);
                        processingResults.results.insert({index, {true, usedChannels}});
                        std::cout << "Tile " << antenna.tile << antenna.signalChain << " written to file successfully" << std::endl;
                    }
                    catch (OutSignalException const& e) {
                        processingResults.results.insert({index, {false, usedChannels}});
                        std::cout << "Tile " << antenna.tile << antenna.signalChain << " writing failed" << std::endl;
                    }
                }
                else {
                    // Indicate antenna input skipped due to no readable data
                    processingResults.results.insert({index, {false, usedChannels}});
                    std::cout << "Tile " << antenna.tile << antenna.signalChain << " not processed (no readable data)" << std::endl;
                }
            }
            else {
                // Skip processing for flagged antenna inputs
                processingResults.results.insert({index, {false, usedChannels}});
                std::cout << "Skipping flagged tile " << antenna.tile << antenna.signalChain << std::endl;
            }
		}
	}
	return processingResults;
}

void readRawSignalFiles(AppConfig const& appConfig, AntennaConfig const& antennaConfig,
                        std::vector<std::vector<std::complex<float>>>& antennaInputSignals, std::set<unsigned>& usedChannels) {
    for (auto channel : antennaConfig.frequencyChannels) {
        std::filesystem::path dir (appConfig.inputDirectoryPath);
        std::filesystem::path filename = std::to_string(appConfig.observationID) + "_" +
                                            std::to_string(appConfig.signalStartTime) + "_" +
                                            std::to_string(channel) + ".sub";
        std::filesystem::path voltageFile = dir / filename;

        try {
            antennaInputSignals.push_back(readInputDataFile(voltageFile, index, antennaConfig.antennaInputs.size()));
            usedChannels.insert(channel);
        }
        catch (ReadInputDataException const& e) {
            // Skip channel
        }
    }
}


void mergeSecondaryProcessingResults(PrimaryNodeCommunicator const& primary, ObservationProcessingResults& processingResults) {
    // Gather secondary node processing results
    auto secondaryProcessingResults = primary.receiveProcessingResults();
	// Merge secondary node processing results into primary processing results
	for (unsigned i = 1; i < primary.getNodeCount(); i++) {
		processingResults.results.merge(secondaryProcessingResults.at(i).results);
	}
}


// Use metadata file reader to read in the antenna configuration, update success reference on failure (assuming true by default)
AntennaConfig createAntennaConfig(AppConfig const& appConfig, bool& success) {
    AntennaConfig antennaConfig;
    try {
		MetadataFileReader mfr(appConfig);
		antennaConfig = mfr.getAntennaConfig(appConfig);
	}
	catch (MetadataException const& e) {
        success = false;
		std::cout << "Error creating antenna configuration: " << e.what() << std::endl;
	}
    return antennaConfig;
}


// Read in filter coefficients from file, update success reference on failure (assuming true by default)
std::vector<std::complex<float>> createFilterCoefficients(std::string const filterPath, bool& success) {
    std::vector<std::complex<float>> coefficients;
    try {
        coefficients = readCoeData(filterPath);
    }
    catch (ReadCoeDataException const& e) {
        success = false;
        std::cout << "Error creating filter coefficients: " << e.what() << std::endl;
    }
    return coefficients;
}


std::optional<AntennaInputRange> communicateNodeAntennaInputAssignment(PrimaryNodeCommunicator const& primary,
                                                                       std::map<unsigned, bool> const& secondaryNodeStatus,
                                                                       unsigned const numAntennaInputs) {
    // Calculate range of antenna inputs for each node to process
    auto antennaInputAssignments = assignNodeAntennaInputs(getActiveNodeCount(secondaryNodeStatus), numAntennaInputs);

	// Send antenna input assignments to active secondary nodes
    for (auto const& [nodeID, active] : secondaryNodeStatus) {
        if (active) {
            std::cout << "Assigning node " << nodeID << " | " << antennaInputAssignments.back().value().begin << " - " << antennaInputAssignments.back().value().end << std::endl;
            primary.sendAntennaInputAssignment(nodeID, antennaInputAssignments.back());
            antennaInputAssignments.pop_back();
        }
    }

    std::cout << "Assigning primary node | " << antennaInputAssignments.back().value().begin << " - " << antennaInputAssignments.back().value().end << std::endl;

    // Return last remaining antenna input assignment to primary node
	return antennaInputAssignments.back();
}

// Return the total number of active nodes (nodes that didn't fail on startup)
unsigned getActiveNodeCount(std::map<unsigned, bool> const& secondaryNodeStatus) {
    unsigned numActiveNodes = 1;

    for (auto const& [nodeID, active] : secondaryNodeStatus) {
        if (active) {
            numActiveNodes++;
        }
    }
    return numActiveNodes;
}