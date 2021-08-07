#pragma once
#include<complex>
#include<vector>
#include<cstdint>

// Forward declaration of ChannelRemapping struct "ChannelRemapping.hpp"
struct ChannelRemapping;

// Function responsible for all the transoformations, filters and downsampling
// the signal data.
std::vector<std::int16_t> processSignal(std::vector<std::complex<float>> const& signalData,
                               std::vector<std::complex<float>> const& coefficiantPFB,
                               ChannelRemapping const& remappingData);

