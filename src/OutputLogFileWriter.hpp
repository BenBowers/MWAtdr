#pragma once

#include "Common.hpp"
#include "ChannelRemapping.hpp"

#include <fstream>
#include <iostream>

void writeLogFile(AppConfig observation, ChannelRemapping mapping,
				  ObservationProcessingReults results, AntennaConfig physical);