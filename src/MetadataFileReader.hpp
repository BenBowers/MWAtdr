#pragma once

#include "Common.hpp"
#include "mwalib.h"

#include <stdexcept>
#include <string>

class MetadataFileReader {
	private:
	    VoltageContext* voltageContext;
		MetafitsMetadata* metafitsMetadata;

        bool validMetafits(AppConfig const appConfig);
        std::vector<std::string> findVoltageFiles(AppConfig const appConfig);
		std::vector<AntennaInputPhysID> getPhysicalAntennaInputs();
        std::set<unsigned> getFrequencyChannelsUsed();
	public:
	    MetadataFileReader(AppConfig const appConfig);
        AntennaConfig getAntennaConfig();
};

class MetadataException : public std::runtime_error {
    public:
	    MetadataException() : std::runtime_error("MetadataException") {}
};