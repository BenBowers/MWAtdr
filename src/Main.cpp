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

int main() {
    // Initialise InternodeCommunicator singleton
	auto communicator = InternodeCommunicator::init();
	
    if (communicator->getNodeID() == 0) {
	    AppConfig appConfig = {"/app/signals/", 1294797712, 1294797712,
	                           "./inverse_polyphase_filter.bin", "./reconstructed_observation"};

   	    // Read in metadata
        MetadataFileReader mfr = MetadataFileReader(appConfig);
	    AntennaConfig antennaConfig = mfr.getAntennaConfig();

        // Create primary node communicator
		auto primary = communicator->getPrimaryNodeCommunicator();

        // Node startup and error checking
		primary.sendAppStartupStatus(true);
		primary.sendAppConfig(appConfig);
		primary.receiveNodeSetupStatus();

		// Compute frequency channel remapping
		auto channelRemapping = computeChannelRemapping(2, antennaConfig.frequencyChannels);

        // Send observation and signal processing details to secondary nodes
		primary.sendAntennaConfig(antennaConfig);
		primary.sendChannelRemapping(channelRemapping);

		// Assign and communicate the range of antenna inputs each node will process
		auto antennaInputRanges = assignNodeAntennaInputs(communicator->getNodeCount(), 4);

		for (int i = 1; i < communicator->getNodeCount(); i++) {
			primary.sendAntennaInputAssignment(i, antennaInputRanges.at(i));
		}

		// Read in filter coefficients
		auto coefficients = readCoeData("./inverse_polyphase_filter.bin");

        // Read in signal files
		std::vector<std::vector<std::complex<float>>> rawSignals;
		for (int i = antennaInputRanges.at(0).value().begin;
		         i <= antennaInputRanges.at(0).value().end; i++) {
			if (validateInputData(std::to_string(i))) {
		        rawSignals.push_back(readInputDataFile(std::to_string(i), i));
			}
		}

		// Process all signals from assigned antenna inputs
		std::vector<std::vector<std::int16_t>> processedSignals;
		for (auto signal : rawSignals) {
            processedSignals.push_back(processSignal(signal, coefficients, channelRemapping));
		}

		// Write all processed signals to file
		for (int i = 0; i < processedSignals.size(); i++) {
            outSignalWriter(processedSignals.at(i), appConfig,
			                antennaConfig.antennaInputs.at(i + antennaInputRanges.at(0).value().begin));
		}

        auto processingResults = primary.receiveProcessingResults();
        //writeLogFile(appConfig, channelRemapping, processingResults, antennaConfig);
	}
	else {
		ObservationProcessingResults processingResults;

        // Create secondary node communicator
		auto secondary = communicator->getSecondaryNodeCommunicator();

        // Node startup and error checking
        secondary.receiveAppStartupStatus();
		auto appConfig = secondary.receiveAppConfig();
		secondary.sendNodeSetupStatus(true);

        // Receive observation and signal processing details from primary node
		auto antennaConfig = secondary.receiveAntennaConfig();
		auto channelRemapping = secondary.receiveChannelRemapping();
		auto antennaInputRange = secondary.receiveAntennaInputAssignment();

        // Read in filter coefficients
		auto coefficients = readCoeData("./inverse_polyphase_filter.bin");

        // Read in signal files
		std::vector<std::vector<std::complex<float>>> rawSignals;
		for (int i = antennaInputRange.value().begin;
		         i <= antennaInputRange.value().end; i++) {
			if (validateInputData(std::to_string(i))) {
		        rawSignals.push_back(readInputDataFile(std::to_string(i), i));
			}
		}

        // Process all signals from assigned antenna inputs
		std::vector<std::vector<std::int16_t>> processedSignals;
		for (auto signal : rawSignals) {
            processedSignals.push_back(processSignal(signal, coefficients, channelRemapping));
		}
		
        // Write all processed signals to file
		for (int i = 0; i < processedSignals.size(); i++) {
            outSignalWriter(processedSignals.at(i), appConfig,
			                antennaConfig.antennaInputs.at(i + antennaInputRange.value().begin));
		}

        secondary.sendProcessingResults(processingResults);
	}
}