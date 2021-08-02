#include "signal_processing.hpp"

std::vector<int> processSignal(std::vector<std::complex<float>> signalData,
                               std::vector<std::complex<float>> coefficiantPFB,
                               ChannelRemapping remappingData) {
}

void remapChannels(std::vector<std::complex<float>> signalData,
                   std::map<unsigned, unsigned> const mapping) {

}

void performPFB(std::vector<std::complex<float>> signalData,
                std::vector<std::complex<float>> coefficantPFB,
                std::map<unsigned, unsigned> const mapping) {

}

std::vector<int> performDFT(std::vector<std::complex<float>> signalData) {

}

void downsampleData(std::vector<int> signalData, unsigned samplingRate) {

}

std::vector<short> doPostProcessing(std::vector<int> signalData) {

}

