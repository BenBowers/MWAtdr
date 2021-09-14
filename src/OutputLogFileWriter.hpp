#pragma once

#include <stdexcept>
#include <string>

struct AntennaConfig;
struct AppConfig;
struct ChannelRemapping;
struct ObservationProcessingResults;

// Throws LogWriterException
void writeLogFile(AppConfig appConfig, ChannelRemapping channelRemapping,
				  ObservationProcessingResults results, AntennaConfig antennaConfig);

class LogWriterException : public std::runtime_error {
public:
    LogWriterException(const std::string& message) : std::runtime_error(message) {}
};