#pragma once

#include "common.hpp"

struct MetadataFileReader {
	// TODO: Add constructor to build VoltageContext

    AntennaConfig getAntennaConfig() {
		AntennaConfig config;

		// TODO: getPhysicalAntennaMappings()
		// TODO: getFrequencyChannelsUsed()

		return config;
	}
};