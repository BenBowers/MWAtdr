#pragma once

#include "InternodeCommunication.hpp"

struct AntennaInputRange;
struct ChannelRemapping;

// (Primary) send and (secondary) receive app startup status
void communicateAppStartupStatus(PrimaryNodeCommunicator const& primary);
void communicateAppStartupStatus(SecondaryNodeCommunicator const& secondary);

// (Primary) send and (secondary) receive appConfig
void communicateAppConfig(PrimaryNodeCommunicator const& primary, AppConfig& appConfig);
void communicateAppConfig(SecondaryNodeCommunicator const& primary, AppConfig& appConfig);

// (Primary) receive and (secondary) send node setup status
void communicateNodeSetupStatus(PrimaryNodeCommunicator const& primary);
void communicateNodeSetupStatus(SecondaryNodeCommunicator const& secondary);

// (Primary) send and (secondary) receive antennaConfig
void communicateAntennaConfig(PrimaryNodeCommunicator const& primary, AntennaConfig& antennaConfig);
void communicateAntennaConfig(SecondaryNodeCommunicator const& secondary, AntennaConfig& antennaConfig);

// (Primary) send and (secondary) receive channelRemapping
void communicateChannelRemapping(PrimaryNodeCommunicator const& primary, ChannelRemapping& channelRemapping);
void communicateChannelRemapping(SecondaryNodeCommunicator const& secondary, ChannelRemapping& channelRemapping);

// (Primary) send and (secondary) receive antennaInputRange
void communicateAntennaInputRanges(PrimaryNodeCommunicator const& primary, AntennaConfig const& antennaConfig,
                                   std::optional<AntennaInputRange>& antennaInputRange);
void communicateAntennaInputRanges(SecondaryNodeCommunicator const& secondary, AntennaConfig const& antennaConfig,
                                   std::optional<AntennaInputRange>& antennaInputRange);

// (Primary) receive and (secondary) send processingResults
void communicateProcessingResults(PrimaryNodeCommunicator const& primary,
                                  ObservationProcessingResults& processingResults);
void communicateProcessingResults(SecondaryNodeCommunicator const& secondary,
                                  ObservationProcessingResults& processingResults);