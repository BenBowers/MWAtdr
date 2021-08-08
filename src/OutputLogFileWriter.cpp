#include "OutputLogFileWriter.hpp"

#include <fstream>
#include <iostream>

void writeLogFile(AppConfig observation, ChannelRemapping mapping,
				  ObservationProcessingResults results, AntennaConfig physical) {
    std::string filename = std::to_string(observation.observationID) + "_" +
	                       std::to_string(observation.signalStartTime) + "_outputlog.txt";

	std::ofstream outfile;
	outfile.open(filename);
	
	outfile << "Observation ID: " << std::to_string(observation.observationID) << std::endl;
	outfile << "Signal start time: " << std::to_string(observation.signalStartTime) << std::endl;
	outfile << "Signal stop time:  " << std::to_string(observation.signalStartTime + 8) << std::endl;
	outfile << "Output sample rate: " << std::to_string(mapping.newSamplingFreq) << std::endl;
	outfile << "Sampling period: " << "very fast" << std::endl;
	outfile << "Frequency channel mapping: " << std::endl;
	outfile << "Antenna inputs successfully processed: " << std::endl;
	outfile << "Logical-physical tile mapping: " << std::endl;
	outfile << "Frequency channels used: " << std::endl;

	outfile.close();
}