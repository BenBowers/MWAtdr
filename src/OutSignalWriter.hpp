#pragma once

#include <cstdint>
#include <vector>
#include <string>

struct AppConfig;
struct AntennaInputPhysID;


void outSignalWriter(const std::vector<std::int16_t> &inputData, const AppConfig &observation, const AntennaInputPhysID &physID);