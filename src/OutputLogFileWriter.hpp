#pragma once

#include <stdexcept>
#include <string>

struct AntennaConfig;
struct AppConfig;
struct ChannelRemapping;
struct ObservationProcessingResults;

// Throws LogWriterException
void writeLogFile(AppConfig const& appConfig, ChannelRemapping const& channelRemapping,
				  ObservationProcessingResults const& results, AntennaConfig const& antennaConfig);

class LogWriterException : public std::runtime_error {
public:
    LogWriterException(const std::string& message) : std::runtime_error(message) {}
};