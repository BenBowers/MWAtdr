#pragma once

#include "Common.hpp"
#include "ChannelRemapping.hpp"

void writeLogFile(AppConfig observation, ChannelRemapping mapping,
				  ObservationProcessingResults results, AntennaConfig physical);