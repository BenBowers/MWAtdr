#pragma once

#include "Common.hpp"
#include "mwalib.h"

#include <string>

class MetadataFileReader {
	private:
	    VoltageContext* voltageContext;
		MetafitsMetadata* metafitsMetadata;

        std::vector<std::string> findVoltageFiles(AppConfig const appConfig);
		std::vector<AntennaInputPhysID> getPhysicalAntennaInputs();
        std::set<unsigned> getFrequencyChannelsUsed();
	public:
	    MetadataFileReader(AppConfig const appConfig);
        AntennaConfig getAntennaConfig();
};