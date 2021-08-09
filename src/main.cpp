#include "ChannelRemapping.hpp"
#include "Common.hpp"
#include "InternodeCommunication.hpp"
#include "MetadataFileReader.hpp"
#include "NodeAntennaInputAssigner.hpp"
#include "SignalProcessing.hpp"

// TODO: remove
#include <iostream>

int main() {
    // Initialise InternodeCommunicator singleton
	auto communicator = InternodeCommunicator::init();
	
    if (communicator->getNodeID() == 0) {
	    AppConfig appConfig = {"./myobservation", 1000000, 1000008,
	                           "./inverse_polyphase_filter.bin", "./reconstructed_observation"};

   	    // Read in metadata
        MetadataFileReader mfr;
	    AntennaConfig antennaConfig = mfr.getAntennaConfig();

        // Create primary node communicator
		auto primary = communicator->getPrimaryNodeCommunicator();

		primary.sendAppStartupStatus(true);
		primary.receiveNodeSetupStatus();

		primary.sendAppConfig(appConfig);
		primary.sendAntennaConfig(antennaConfig);
		//primary.sendChannelRemapping();

		// Assign and communicate the range of antenna inputs each node will process
		auto antennaInputRanges = assignNodeAntennaInputs(communicator->getNodeCount(), 4);

		for (int i = 1; i < communicator->getNodeCount(); i++) {
			primary.sendAntennaInputAssignment(i, antennaInputRanges.at(i));
		}
	}
	else {
        // Create secondary node communicator
		auto secondary = communicator->getSecondaryNodeCommunicator();

        secondary.receiveAppStartupStatus();
		secondary.sendNodeSetupStatus(true);

		auto appConfig = secondary.receiveAppConfig();
		auto antennaConfig = secondary.receiveAntennaConfig();
		auto channelRemapping = secondary.receiveChannelRemapping();
		auto antennaInputRange = secondary.receiveAntennaInputAssignment();
	}

	// Processing success/failure (secondary send)
	// Termination (primary and secondary)
}