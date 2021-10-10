#pragma once

#include "Common.hpp"

#include <string>

// Throws std::invalid_argument
AppConfig createAppConfig(int argc, char* argv[]);

// Command line validation functions throw std::invalid_argument
std::string validateInputDirectoryPath(std::string const inputDirectoryPath);
unsigned long long validateObservationID(std::string const observationID);
unsigned long long validateSignalStartTime(std::string const observationID, std::string signalStartTime);
std::string validateInvPolyphaseFilterPath(std::string const invPolyphaseFilterPath);
std::string validateOutputDirectoryPath(std::string const outputDirectoryPath);
bool validateIgnoreErrors(std::string const ignoreErrors);