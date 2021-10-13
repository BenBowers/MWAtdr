#pragma once
#include<complex>
#include<vector>
#include<cstdint>
#include<map>

// Forward declaration of ChannelRemapping struct "ChannelRemapping.hpp"
struct ChannelRemapping;

class SignalProcessingMKLError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

// Function responsible for all the transoformations, filters and downsampling
// the signal data.
void processSignal(std::vector<std::vector<std::complex<float>>> const& signalDataIn,
                               std::vector<unsigned> const& signalDataInMapping,
                               std::vector<std::int16_t>& signalDataOut,
                               std::vector<std::complex<float>> const& coefficiantPFB,
                               ChannelRemapping const& remappingData);
