#include "Common.hpp"


bool operator==(AppConfig const& lhs, AppConfig const& rhs) {
    return lhs.observationID == rhs.observationID
        && lhs.signalStartTime == rhs.signalStartTime
        && lhs.inputDirectoryPath == rhs.inputDirectoryPath
        && lhs.invPolyphaseFilterPath == rhs.invPolyphaseFilterPath
        && lhs.outputDirectoryPath == rhs.outputDirectoryPath;
}

bool operator==(AntennaInputPhysID const& lhs, AntennaInputPhysID const& rhs) {
    return lhs.tile == rhs.tile && lhs.signalChain == rhs.signalChain;
}

bool operator==(AntennaConfig const& lhs, AntennaConfig const& rhs) {
    return lhs.antennaInputs == rhs.antennaInputs && lhs.frequencyChannels == rhs.frequencyChannels;
}

bool operator==(AntennaInputProcessingResults const& lhs, AntennaInputProcessingResults const& rhs) {
    return lhs.success == rhs.success && lhs.usedChannels == rhs.usedChannels;
}

bool operator==(ObservationProcessingResults const& lhs, ObservationProcessingResults const& rhs) {
    return lhs.results == rhs.results;
}
