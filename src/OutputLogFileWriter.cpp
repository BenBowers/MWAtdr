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
double roundThreeDecimalPlace(double num);


void writeLogFile(AppConfig const& appConfig, ChannelRemapping const& channelRemapping,
				  ObservationProcessingResults const& results, AntennaConfig const& antennaConfig) {
    // Generate output log filepath and open file for writing
	auto filepath = generateOutputLogFilepath(appConfig);
	std::ofstream log (filepath);

	if (log.is_open()) {
		writeObservationDetails(log, appConfig);
		writeProcessingDetails(log, channelRemapping);
		writeChannelRemappingDetails(log, channelRemapping);
		writeProcessingResults(log, results, antennaConfig);

        // Check if error occurred while writing
        if (log.fail()) {
            throw LogWriterException("Error occurred writing to log file");
        }

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
    log << "GPS stop time:  " << appConfig.signalStartTime + 8 << std::endl << std::endl;
}

// Write information about the signal sample rate and sampling period to the log file.
void writeProcessingDetails(std::ofstream& log, ChannelRemapping const& channelRemapping) {
    const double CHANNEL_BANDWIDTH_MHZ = 1.28;

    // Calculate output sample rate and sampling period (time between samples) for processed signals
	double sampleRate = CHANNEL_BANDWIDTH_MHZ * (double) channelRemapping.newSamplingFreq;
	double samplingPeriod = (1.0 / sampleRate) * 1000.0;

    log << "SIGNAL PROCESSING DETAILS" << std::endl;
    log << "Output sample rate: " << roundThreeDecimalPlace(sampleRate) << " MHz" << std::endl;
    log << "Output sampling period: " << roundThreeDecimalPlace(samplingPeriod) << " ns\n" << std::endl;
}

// Write which frequency channels were used in the observation,
// and how they were remapped (if at all) to the log file.
void writeChannelRemappingDetails(std::ofstream& log, ChannelRemapping const& channelRemapping) {
    log << "FREQUENCY CHANNELS " << std::endl;
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
// Channel numbers are written as their original value (not the remapped value)
void writeProcessingResults(std::ofstream& log, ObservationProcessingResults const& results, AntennaConfig const& antennaConfig) {
    log << "ANTENNA INPUT PROCESSING RESULTS " << std::endl;
    for (const auto& [index, outcome] : results.results) {
        auto antenna = antennaConfig.antennaInputs.at(index);
        log << "(#" << index << ") " << "Tile " << antenna.tile << antenna.signalChain << ": ";

        if (!antenna.flagged) {
            if (outcome.success) {
                log << "success" << std::endl;
                log << "-Used channels: ";
                for (auto const& i : outcome.usedChannels) {
                    log << i;
                    if (i != *outcome.usedChannels.rbegin()) {
                        log << ", ";
                    }
                }
            }
            else {
                log << "fail" << std::endl;
                log << "-Used channels: N/A";
            }
        }
        else {
            log << "not processed (flagged)" << std::endl;
            log << "-Used channels: N/A";
        }
        log << std::endl << std::endl;
    }
}


// Generates a filepath which can be used to open the log file for writing.
std::filesystem::path generateOutputLogFilepath(AppConfig const& appConfig) {
    std::filesystem::path dir (appConfig.outputDirectoryPath);
    std::filesystem::path filename (std::to_string(appConfig.observationID) + "_" +
                                    std::to_string(appConfig.signalStartTime) + "_outputlog.txt");
    return dir / filename;
}


// Rounds a real (double) number to the nearest three decimal places.
double roundThreeDecimalPlace(double num) {
	return std::round(num * 1000.0) / 1000.0;
}