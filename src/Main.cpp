#include "Main.hpp"

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
	auto const communicatorContext = InternodeCommunicationContext::initialise();
	auto const communicator = communicatorContext->getCommunicator();

    std::visit([](auto const& node) {
	    AppConfig appConfig;
		AntennaConfig antennaConfig;
		std::optional<AntennaInputRange> antennaInputRange;

		// (Primary node) validate AppConfig and metadata
		if (node.getNodeID() == 0) {
	        appConfig = {"./myobservation", 1000000, 1000008,
	                     "./inverse_polyphase_filter.bin",
						 "./reconstructed_observation", true};

			// Read in metadata
			MetadataFileReader mfr;
			antennaConfig = mfr.getAntennaConfig();
		}

        // Node startup and error checking
		communicateAppStartupStatus(node);
		communicateAppConfig(node, appConfig);
		communicateNodeSetupStatus(node);

		// (Primary node) compute frequency channel remapping
		ChannelRemapping channelRemapping;
		if (node.getNodeID() == 0) {
		    //channelRemapping = computeChannelRemapping(1, antennaConfig.frequencyChannels);
		}

		// Communicate observation and signal processing details
		communicateAntennaConfig(node, antennaConfig);
		communicateChannelRemapping(node, channelRemapping);
		communicateAntennaInputRanges(node, antennaConfig, antennaInputRange);

		// Read in filter coefficients
		//auto const coefficients = readCoeData(appConfig.invPolyphaseFilterPath);

        // Read in and process signal for each assigned antenna input
		ObservationProcessingResults processingResults;

		if (antennaInputRange.has_value()) {
			for (unsigned i = antennaInputRange.value().begin; i <= antennaInputRange.value().end; i++) {
				// Read in raw signal file
				//if (validateInputData()) {
					//auto const rawSignal = readInputDataFile();
				//}
				// Process signal
				//auto const processedSignal = processSignal();
				// Write processed signal to file
				//outSignalWriter();
			}
		}

        // Communicate signal processing results
		communicateProcessingResults(node, processingResults);

        // (Primary node) write output log file
		if (node.getNodeID() == 0) {
            writeLogFile(appConfig, channelRemapping, processingResults, antennaConfig);
		}
	}, communicator);
}


void communicateProcessingResults(PrimaryNodeCommunicator const& primary,
                                  ObservationProcessingResults& processingResults) {
    auto secondaryProcessingResults = primary.receiveProcessingResults();

	for (unsigned i = 1; i < primary.getNodeCount(); i++) {
		processingResults.results.merge(secondaryProcessingResults.at(i).results);
	}
}
void communicateProcessingResults(SecondaryNodeCommunicator const& secondary,
                                  ObservationProcessingResults& processingResults) {
    secondary.sendProcessingResults(processingResults);
}


void communicateAntennaConfig(PrimaryNodeCommunicator const& primary, AntennaConfig& antennaConfig) {
	primary.sendAntennaConfig(antennaConfig);
}
void communicateAntennaConfig(SecondaryNodeCommunicator const& secondary, AntennaConfig& antennaConfig) {
	antennaConfig = secondary.receiveAntennaConfig();
}

void communicateChannelRemapping(PrimaryNodeCommunicator const& primary, ChannelRemapping& channelRemapping) {
	primary.sendChannelRemapping(channelRemapping);
}
void communicateChannelRemapping(SecondaryNodeCommunicator const& secondary, ChannelRemapping& channelRemapping) {
	channelRemapping = secondary.receiveChannelRemapping();
}

void communicateAntennaInputRanges(PrimaryNodeCommunicator const& primary, AntennaConfig const& antennaConfig,
                                   std::optional<AntennaInputRange>& antennaInputRange) {
	auto antennaInputAssignments = assignNodeAntennaInputs(primary.getNodeCount(),
	                                                       antennaConfig.antennaInputs.size());

	for (unsigned i = 1; i < primary.getNodeCount(); i++) {
		primary.sendAntennaInputAssignment(i, antennaInputAssignments.at(i));
	}

	antennaInputRange = antennaInputAssignments.at(0);
}
void communicateAntennaInputRanges(SecondaryNodeCommunicator const& secondary, AntennaConfig const& antennaConfig,
                                   std::optional<AntennaInputRange>& antennaInputRange) {
    antennaInputRange = secondary.receiveAntennaInputAssignment();
}


void communicateAppStartupStatus(PrimaryNodeCommunicator const& primary) {
	primary.sendAppStartupStatus(true);
}
void communicateAppStartupStatus(SecondaryNodeCommunicator const& secondary) {
	secondary.receiveAppStartupStatus();
}

void communicateAppConfig(PrimaryNodeCommunicator const& primary, AppConfig& appConfig) {
	primary.sendAppConfig(appConfig);
}
void communicateAppConfig(SecondaryNodeCommunicator const& secondary, AppConfig& appConfig) {
	appConfig = secondary.receiveAppConfig();
}

void communicateNodeSetupStatus(PrimaryNodeCommunicator const& primary) {
	primary.receiveNodeSetupStatus();
}
void communicateNodeSetupStatus(SecondaryNodeCommunicator const& secondary) {
	secondary.sendNodeSetupStatus(true);
}