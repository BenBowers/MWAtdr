#pragma once

struct AntennaConfig;
struct AppConfig;
struct ChannelRemapping;
struct ObservationProcessingResults;

void writeLogFile(AppConfig observation, ChannelRemapping mapping,
				  ObservationProcessingResults results, AntennaConfig physical);