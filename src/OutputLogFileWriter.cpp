#include "OutputLogFileWriter.hpp"

#include "ChannelRemapping.hpp"
#include "Common.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>


void writeObservationDetails(std::ofstream& log, AppConfig const& appConfig);
void writeProcessingDetails(std::ofstream& log, ChannelRemapping const& channelRemapping);
void writeChannelRemappingDetails(std::ofstream& log, ChannelRemapping const& channelRemapping);
void writeProcessingResults(std::ofstream& log, ObservationProcessingResults const& results, AntennaConfig const& antennaConfig);
std::filesystem::path generateOutputLogFilepath(AppConfig const& appConfig);
float roundTwoDecimalPlace(float num);


void writeLogFile(AppConfig appConfig, ChannelRemapping channelRemapping,
				  ObservationProcessingResults results, AntennaConfig antennaConfig) {
    // Generate output log filepath and open file for writing
	auto filepath = generateOutputLogFilepath(appConfig);
	std::ofstream log;
	log.open(filepath);

	if (log.is_open()) {
		writeObservationDetails(log, appConfig);
		writeProcessingDetails(log, channelRemapping);
		writeChannelRemappingDetails(log, channelRemapping);
		writeProcessingResults(log, results, antennaConfig);
	    log.close();
	}
	else {
		// throw exception
	}
}


void writeObservationDetails(std::ofstream& log, AppConfig const& appConfig) {
    log << "OBSERVATION DETAILS" << std::endl;
    log << "Observation ID: " << appConfig.observationID << std::endl;
    log << "Signal start time: " << appConfig.signalStartTime << std::endl;
    log << "Signal stop time: " << appConfig.signalStartTime + 8 << std::endl << std::endl;
}

void writeProcessingDetails(std::ofstream& log, ChannelRemapping const& channelRemapping) {
	//const float CHANNEL_BANDWIDTH_MHZ = 1.28;
	// CHANNEL_BANDWIDTH_MHZ * (float) channelRemapping.newSamplingFreq
	const float ORIGINAL_SAMPLE_RATE_MHZ = 655.36;
	const float ORIGINAL_SAMPLING_PERIOD_NS = 781.25;

	float sampleRate = ORIGINAL_SAMPLE_RATE_MHZ / (float) channelRemapping.newSamplingFreq;
	float samplingPeriod = ORIGINAL_SAMPLING_PERIOD_NS * (float) channelRemapping.newSamplingFreq;

    log << "PROCESSING DETAILS" << std::endl;
    log << "Output sample rate: " << roundTwoDecimalPlace(sampleRate) << "MHz" << std::endl;
    log << "Sampling period (time between samples): " << roundTwoDecimalPlace(samplingPeriod) << "ns\n" << std::endl;
}

void writeChannelRemappingDetails(std::ofstream& log, ChannelRemapping const& channelRemapping) {
    log << "Frequency channels used: " << std::endl;
    for (const auto& [old, remapped] : channelRemapping.channelMap) {
        log << "Channel " << old << " mapped to " << remapped.newChannel;
        if (remapped.flipped) {
            log << " (conjugate)";
        }
        log << std::endl;
    }
    log << std::endl;
}

void writeProcessingResults(std::ofstream& log, ObservationProcessingResults const& results, AntennaConfig const& antennaConfig) {
    log << "Processing results: " << std::endl;
    for (const auto& [index, outcome] : results.results) {
        auto antenna = antennaConfig.antennaInputs.at(index);
        log << "#" << index << " Tile " << antenna.tile << antenna.signalChain << ": " << outcome.success << std::endl;
		log << "/tUsed channels: ";
		for (auto const& i : outcome.usedChannels) {
			log << i << " | ";
		}
		log << std::endl;
    }
}


std::filesystem::path generateOutputLogFilepath(AppConfig const& appConfig) {
    std::filesystem::path dir (appConfig.outputDirectoryPath);
    std::filesystem::path filename (std::to_string(appConfig.observationID) + "_" +
                                    std::to_string(appConfig.signalStartTime) + "_outputlog.txt");
    return dir / filename;
}


float roundTwoDecimalPlace(float num) {
	return std::round(num * 100.0) / 100.0;
}