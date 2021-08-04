#include "signal_processing.hpp"
#include<iostream>

std::vector<short> processSignal(std::vector<std::complex<float>> signalData,
                               std::vector<std::complex<float>> coefficiantPFB,
                               ChannelRemapping remappingData) {
    std::cout << "ProcessSignal Called" << std::endl;
    remapChannels(signalData, remappingData.channelMap);
    performPFB(signalData, coefficiantPFB, remappingData.channelMap);
    std::vector<int> postDFTSignalData = performDFT(signalData);
    downsampleData(postDFTSignalData, remappingData.newSamplingFreq);
    std::vector<short> outData = doPostProcessing(postDFTSignalData);
    return outData;
}

void remapChannels(std::vector<std::complex<float>> signalData,
                   std::map<unsigned, unsigned> const mapping) {
    std::cout << "remapChannels() called" << std::endl;
}

void performPFB(std::vector<std::complex<float>> signalData,
                std::vector<std::complex<float>> coefficantPFB,
                std::map<unsigned, unsigned> const mapping) {
    std::cout << "performPFB() called" << std::endl;
}

std::vector<int> performDFT(std::vector<std::complex<float>> signalData) {
    std::cout << "performDFT() called" << std::endl;
    return std::vector<int>();
}

void downsampleData(std::vector<int> signalData, unsigned samplingRate) {
    std::cout << "downsampleData() called" << std::endl;
}

std::vector<short> doPostProcessing(std::vector<int> signalData) {
    std::cout << "doPostProcessing() called" << std::endl;
    return std::vector<short>();
}

