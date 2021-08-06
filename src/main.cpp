#include "common.hpp"
#include "metadata_file_reader.hpp"

#include <iostream>

int main() {
	AntennaConfig antennas;
	MetadataFileReader metareader;

	antennas = metareader.getAntennaConfig();

	for (AntennaInputPhysID i : antennas.antennaInputs)
	{
		std::cout << "Antenna " << i.tile << " " << i.signalChain << std::endl;
	}

	for (unsigned i : antennas.frequencyChannels)
	{
		std::cout << i << ", ";
	}

	std::cout << std::endl;
}
