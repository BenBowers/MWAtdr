#include "CommandLineArguments.hpp"

#include <filesystem>
#include <stdexcept>


AppConfig createAppConfig(int argc, char* argv[]) {
    // Validate command line arguments
	if (argc != 7) {
		throw std::invalid_argument{"Invalid number of command line arguments"};
	}

    // Read in app configuration from command line arguments
	AppConfig appConfig;
	appConfig.inputDirectoryPath = validateInputDirectoryPath(argv[1]);
	appConfig.observationID = validateObservationID(argv[2]);
	appConfig.signalStartTime = validateSignalStartTime(argv[2], argv[3]);
	appConfig.invPolyphaseFilterPath = validateInvPolyphaseFilterPath(argv[4]);
	appConfig.outputDirectoryPath = validateOutputDirectoryPath(argv[5]);
	appConfig.ignoreErrors = validateIgnoreErrors(argv[6]);
	return appConfig;
}


std::string validateInputDirectoryPath(std::string const inputDirectoryPath) {
	std::filesystem::path directory (inputDirectoryPath);

	if (!std::filesystem::exists(directory)) {
		throw std::invalid_argument {"Invalid input directory path, does not exist"};
	}
	else if (!std::filesystem::is_directory(directory)) {
		throw std::invalid_argument {"Invalid input directory path, not a directory"};
	}
	else if (std::filesystem::is_empty(directory)) {
		throw std::invalid_argument {"Invalid input directory path, is empty"};
	}
	return (std::string) directory;
}


unsigned long long validateObservationID(std::string const observationID) {
	auto id = std::stoull(observationID);

	if (id % 8 != 0) {
		throw std::invalid_argument {"Invalid observation ID, must be divisible by 8"};
	}
	return id;
}


unsigned long long validateSignalStartTime(std::string const observationID, std::string signalStartTime) {
	auto id = std::stoull(observationID);
	auto time = std::stoull(signalStartTime);

	if (time % 8 != 0) {
		throw std::invalid_argument {"Invalid signal start time, must be divisible by 8"};
	}
	else if (time < id) {
		throw std::invalid_argument {"Invalid signal start time, must be greater than or equal to observation ID"};
	}
	return time;
}


std::string validateInvPolyphaseFilterPath(std::string const invPolyphaseFilterPath) {
	std::filesystem::path file (invPolyphaseFilterPath);

	if (!std::filesystem::exists(file)) {
		throw std::invalid_argument {"Invalid inverse polyphase filter path, does not exist"};
	}
	else if (!std::filesystem::is_regular_file(file)) {
		throw std::invalid_argument {"Invalid inverse polyphase filter path, is not a regular file"};
	}
	else if (std::filesystem::is_empty(file)) {
		throw std::invalid_argument {"Invalid inverse polyphase filter path, file is empty"};
	}
	return (std::string) file;
}


std::string validateOutputDirectoryPath(std::string const outputDirectoryPath) {
	std::filesystem::path directory (outputDirectoryPath);

	if (!std::filesystem::exists(directory)) {
		throw std::invalid_argument {"Invalid output directory path, does not exist"};
	}
	if (!std::filesystem::is_directory(directory)) {
		throw std::invalid_argument {"Invalid output directory path, not a directory"};
	}
	return (std::string) directory;
}


bool validateIgnoreErrors(std::string const ignoreErrors) {
	bool ignore = false;

	if (ignoreErrors.compare("true") == 0) {
		ignore = true;
	}
    else if (ignoreErrors.compare("false") != 0) {
		throw std::invalid_argument {"Ignore errors argument must be 'true' or 'false'"};
	}
	return ignore;
}