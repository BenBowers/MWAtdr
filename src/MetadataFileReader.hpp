#pragma once

#include "Common.hpp"
#include "mwalib.h"

#include <string>

class MetadataFileReader {
	private:
	    VoltageContext* voltageContext;

        std::vector<std::string> findVoltageFiles(AppConfig const appConfig);
	public:
	    MetadataFileReader(AppConfig const appConfig);
        AntennaConfig getAntennaConfig();
};