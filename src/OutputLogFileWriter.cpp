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
	std::ofstream log (filepath);

	if (log.is_open()) {
		writeObservationDetails(log, appConfig);
		writeProcessingDetails(log, channelRemapping);
		writeChannelRemappingDetails(log, channelRemapping);
		writeProcessingResults(log, results, antennaConfig);
	    log.close();
	}
	else {
        throw LogWriterException("Error opening log file");
	}
}


// Write general information about the observation to the log file.
void writeObservationDetails(std::ofstream& log, AppConfig const& appConfig) {
    log << "OBSERVATION DETAILS" << std::endl;
    log << "Observation ID: " << appConfig.observationID << std::endl;
    log << "GPS start time: " << appConfig.signalStartTime << std::endl;
    log << "GPS stop time: " << appConfig.signalStartTime + 8 << std::endl << std::endl;
}

// Write information about the signal sample rate and sampling period to the log file.
void writeProcessingDetails(std::ofstream& log, ChannelRemapping const& channelRemapping) {
    const float CHANNEL_BANDWIDTH_MHZ = 1.28;
	const float ORIGINAL_SAMPLING_PERIOD_NS = 781.25;
	const float ORIGINAL_SAMPLE_RATE = 512;

    // Calculate output sample rate and sampling period (time between samples).
	float sampleRate = CHANNEL_BANDWIDTH_MHZ * (float) channelRemapping.newSamplingFreq;
	float samplingPeriod = ORIGINAL_SAMPLING_PERIOD_NS * (ORIGINAL_SAMPLE_RATE / (float) channelRemapping.newSamplingFreq);

    log << "PROCESSING DETAILS" << std::endl;
    log << "Output sample rate: " << roundTwoDecimalPlace(sampleRate) << " MHz" << std::endl;
    log << "Sampling period: " << roundTwoDecimalPlace(samplingPeriod) << " ns\n" << std::endl;
}

// Write which frequency channels were used in the observation,
// and how they were remapped (if at all) to the log file.
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

// Write which antenna inputs were processed successfully or failed,
// and which channels they used to the log file.
void writeProcessingResults(std::ofstream& log, ObservationProcessingResults const& results, AntennaConfig const& antennaConfig) {
    log << "Antenna input processing results: " << std::endl;
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


// Generates a filepath which can be used to open the log file for writing.
std::filesystem::path generateOutputLogFilepath(AppConfig const& appConfig) {
    std::filesystem::path dir (appConfig.outputDirectoryPath);
    std::filesystem::path filename (std::to_string(appConfig.observationID) + "_" +
                                    std::to_string(appConfig.signalStartTime) + "_outputlog.txt");
    return dir / filename;
}


// Rounds a real (float) number to the nearest two decimal places.
float roundTwoDecimalPlace(float num) {
	return std::round(num * 100.0) / 100.0;
}