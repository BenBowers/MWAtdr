#pragma once

#include <stdexcept>
#include <string>

struct AntennaConfig;
struct AppConfig;
struct ChannelRemapping;
struct ObservationProcessingResults;

// Throws LogWriterException
void writeLogFile(AppConfig observation, ChannelRemapping mapping,
				  ObservationProcessingResults results, AntennaConfig physical);

class LogWriterException : public std::runtime_error {
public:
    LogWriterException(const std::string& message) : std::runtime_error(message) {}
};