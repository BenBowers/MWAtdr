#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>

struct AppConfig;
struct AntennaInputPhysID;

//this fuction throws the following exception throw std::ios::failure


void outSignalWriter(const std::vector<std::int16_t> &inputData, const AppConfig &observation, const AntennaInputPhysID &physID);

std::filesystem::path generateFilePath(const AppConfig &observation, const AntennaInputPhysID &physID);