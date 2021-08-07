#include "Common.hpp"
#include "MetadataFileReader.hpp"

class MetadataFileReader {
	public:
		// TODO: Add constructor to build VoltageContext

		AntennaConfig getAntennaConfig() {
		    AntennaConfig config;

		    // TODO: getPhysicalAntennaMappings()
		    AntennaInputPhysID arr[] = {{0, 'X'}, {0, 'Y'}, {1, 'X'}, {1, 'Y'},
                                        {6, 'X'}, {6, 'Y'}, {7, 'X'}, {7, 'Y'}};
		    config.antennaInputs.assign(arr, arr + 8);
		    // TODO: getFrequencyChannelsUsed()
		    config.frequencyChannels.insert({124, 125, 127, 128, 129});

		    return config;
	    }
};