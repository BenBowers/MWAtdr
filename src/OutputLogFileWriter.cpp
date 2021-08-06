#include "OutputLogFileWriter.hpp"

#include <fstream>
#include <iostream>

void writeLogFile(AppConfig observation, ChannelRemapping mapping,
				  ObservationProcessingResults results, AntennaConfig physical) {
    std::string filename = std::to_string(observation.observationID) + "_" +
	                       std::to_string(observation.signalStartTime) + "_outputlog.txt";

	std::cout << filename << std::endl;

	/*ofstream outfile;
	outfile.open("")*/
}