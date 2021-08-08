#include<iostream>
#include<map>
#include"SignalProcessing.hpp"
#include"ChannelRemapping.hpp"

// Remaps the channels based on the mapping given to it, this mapping is to be generated
// by computeChannelRemapping() in ChannelRemapping.hpp
static void remapChannels(std::vector<std::complex<float>> const& signalData,
                   ChannelRemapping const& remappingData);

// Performs an inverse polyphase filter bank (PFB) on the signal data, the mapping is required
// for this function so the convolution only goes over the appropriate channels. This mapping
// is provided by computeChannelRemapping() in ChannelRempping.hpp
static void performPFB(std::vector<std::complex<float>> const& signalData,
                std::vector<std::complex<float>> const& coefficantPFB,
                std::map<unsigned, ChannelRemapping::RemappedChannel> const& mapping);

// Performs an inverse discrete fourier transorm on the signal data, changing the frequency
// data into a time domain signal
static void performDFT(std::vector<std::complex<float>> const& signalData);

// Converts the downsampled time domain array into a 16bit signed int with clamping
static std::vector<std::int16_t> doPostProcessing(std::vector<std::complex<float>> const& signalData);


std::vector<std::int16_t> processSignal(std::vector<std::complex<float>> const& signalData,
                               std::vector<std::complex<float>> const& coefficiantPFB,
                               ChannelRemapping const& remappingData) {
    std::cout << "ProcessSignal Called" << std::endl;
    remapChannels(signalData, remappingData);
    performPFB(signalData, coefficiantPFB, remappingData.channelMap);
    performDFT(signalData);
    std::vector<std::int16_t> outData = doPostProcessing(signalData);
    return outData;
}

static void remapChannels(std::vector<std::complex<float>> const& signalData,
                   ChannelRemapping const& remappingData) {
    std::cout << "remapChannels() called" << std::endl;
}

static void performPFB(std::vector<std::complex<float>> const& signalData,
                std::vector<std::complex<float>> const& coefficantPFB,
                std::map<unsigned, ChannelRemapping::RemappedChannel> const& mapping) {
    std::cout << "performPFB() called" << std::endl;
}

static void performDFT(std::vector<std::complex<float>> const& signalData) {
    std::cout << "performDFT() called" << std::endl;
}

static std::vector<std::int16_t> doPostProcessing(std::vector<std::complex<float>> const& signalData) {
    std::cout << "doPostProcessing() called" << std::endl;
    return std::vector<std::int16_t>();
}

