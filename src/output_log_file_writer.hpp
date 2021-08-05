#pragma once

#include "common.hpp"
#include "channel_remapping.hpp"

#include <fstream>
#include <iostream>

void writeLogFile(AppConfig observation, ChannelRemapping mapping,
				  ObservationProcessingReults results, AntennaConfig physical);