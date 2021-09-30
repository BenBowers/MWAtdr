#include "SignalProcessingTest.hpp"

#include<map>
#include<vector>
#include<cstdint>
#include<iostream>
#include<limits>
#include<memory>
#include<algorithm>

#include "../../src/SignalProcessing.hpp"
#include "../TestHelper.hpp"
#include "../../src/ChannelRemapping.hpp"
#include "../../src/Common.hpp"

// Use std::numeric_limits as it is more modern the INT16_MAX macros
constexpr std::int16_t MAX_INT16 = std::numeric_limits<std::int16_t>::max();
constexpr std::int16_t MIN_INT16 = std::numeric_limits<std::int16_t>::min();

// These Function declarations as I don't want them to be publically avalible
// as they are internal, I've made them non static so I can unit test  them
void remapChannels(std::vector<std::vector<std::complex<float>>> const& signalDataIn,
                   std::map<unsigned, unsigned> const& signalDataInMapping,
                   std::vector<std::complex<float>>& signalDataOut,
                   std::map<unsigned, ChannelRemapping::RemappedChannel> const& channelRemapping,
                   unsigned const outNumChannels);

void performPFB(std::vector<std::complex<float>>& signalData,
                       std::vector<std::complex<float>> const& coefficantPFB,
                       std::map<unsigned, ChannelRemapping::RemappedChannel> const& mapping,
                       unsigned const numOfBlocks,
                       unsigned const numOfChannels);

void performDFT(std::vector<std::complex<float>>& signalData,
                std::vector<float>& outData,
                unsigned const samplingFreq,
                unsigned const numOfBlocks,
                unsigned const numOfChannels,
                unsigned const originalSamplingFreq);

void doPostProcessing(std::vector<float> const& signalData,
                      std::vector<std::int16_t>& signalDataOut);

// Helper function to generate a coefficantArray with the number of channels in Common.hpp
// Will take a vector containing each channels blocks and a map to tell what channel is what
std::vector<std::complex<float>> makeCoeArr(std::vector<std::vector<std::complex<float>>> const& channels,
                                            std::map<unsigned, unsigned> const& mapping) {
    // Get number of blocks from the width of channels
    unsigned const NUM_OF_BLOCKS = channels[0].size();
    // Get the number of channels from common.hpp
    unsigned const NUM_OF_CHANNELS = filterSize;

    // Allocate the array with zeros
    std::vector<std::complex<float>> coefficantData(NUM_OF_BLOCKS * NUM_OF_CHANNELS, { 0.0f, 0.0f });

    // Iterate over each channel
    for( unsigned ii = 0; ii < mapping.size(); ++ii ) {
        auto const channelData = channels[ii];
        unsigned const channelNum = mapping.at(ii);

        // Copy that channel to coefficantData
        for ( unsigned jj = 0; jj < NUM_OF_BLOCKS; ++jj ) {
            coefficantData[channelNum + jj * NUM_OF_CHANNELS] = channelData[jj];
        }
    }

    return coefficantData;
}

class SignalProcessingTest : public StatelessTestModuleImpl {
    public:
        SignalProcessingTest();
};

SignalProcessingTest::SignalProcessingTest() : StatelessTestModuleImpl{{
    {"processSignals() Empty Channel Remapping", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn{};
        std::map<unsigned, unsigned> const signalDataMap{};
        std::vector<int16_t> signalDataOut{};
        ChannelRemapping remappingData{};
        std::vector<std::complex<float>> coeData{};

        try {
            processSignal(signalDataIn, signalDataMap, signalDataOut, coeData, remappingData);
            failTest();
        } catch (std::invalid_argument& e) {
            // Test passed
        }

    }},
    {"processSignals() Different Number of Channels in mappings", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn{};
        std::map<unsigned, unsigned> const signalDataMap{{1, 1}};
        std::vector<int16_t> signalDataOut{};
        ChannelRemapping const remappingData{
            2,
            {
                {0, {0, false}},
                {1, {1, false}}
            }
        };
        std::vector<std::complex<float>> coeData{};

        try {
            processSignal(signalDataIn, signalDataMap, signalDataOut, coeData, remappingData);
            failTest();
        } catch (std::invalid_argument& e) {
            // Test passed
        }

    }},
    {"processSignals() Different Number of Channels between signal and mapping", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn{};
        std::map<unsigned, unsigned> const signalDataMap{{0, 0}, {1, 1}};
        std::vector<int16_t> signalDataOut{};
        ChannelRemapping const remappingData{
            2,
            {
                {0, {0, false}},
                {1, {1, false}}
            }
        };
        std::vector<std::complex<float>> coeData{};

        try {
            processSignal(signalDataIn, signalDataMap, signalDataOut, coeData, remappingData);
            failTest();
        } catch (std::invalid_argument& e) {
            // Test passed
        }

    }},
    {"processSignals() Different number of blocks in the input data (Different at the start)", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn{
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }, {0.0f, 0.0f }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }}
        };
        std::map<unsigned, unsigned> const signalDataMap{
            {0, 0},
            {1, 1},
            {2, 2},
            {3, 3}};

        std::vector<int16_t> signalDataOut{};
        ChannelRemapping const remappingData{
            4,
            {
                {0, {0, false}},
                {1, {1, false}},
                {2, {2, false}},
                {3, {3, false}}
            }
        };
        std::vector<std::complex<float>> coeData{};

        try {
            processSignal(signalDataIn, signalDataMap, signalDataOut, coeData, remappingData);
            failTest();
        } catch (std::invalid_argument& e) {
        }
    }},
    {"processSignals() Different number of blocks in the input data (Different in the middle)", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn{
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }, { 0.0f, 0.0f }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }}
        };
        std::map<unsigned, unsigned> const signalDataMap{
            {0, 0},
            {1, 1},
            {2, 2},
            {3, 3}};

        std::vector<int16_t> signalDataOut{};
        ChannelRemapping const remappingData{
            4,
            {
                {0, {0, false}},
                {1, {1, false}},
                {2, {2, false}},
                {3, {3, false}}
            }
        };
        std::vector<std::complex<float>> coeData{};

        try {
            processSignal(signalDataIn, signalDataMap, signalDataOut, coeData, remappingData);
            failTest();
        } catch (std::invalid_argument& e) {
        }
    }},
    {"processSignals() Different number of blocks in the input data (Different at the end)", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn{
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }, { 0.0f, 0.0f }}
        };
        std::map<unsigned, unsigned> const signalDataMap{
            {0, 0},
            {1, 1},
            {2, 2},
            {3, 3}};

        std::vector<int16_t> signalDataOut{};
        ChannelRemapping const remappingData{
            4,
            {
                {0, {0, false}},
                {1, {1, false}},
                {2, {2, false}},
                {3, {3, false}}
            }
        };
        std::vector<std::complex<float>> coeData{};

        try {
            processSignal(signalDataIn, signalDataMap, signalDataOut, coeData, remappingData);
            failTest();
        } catch (std::invalid_argument& e) {
        }
    }},
    {"processSignals() New samping frequency invalid", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn{
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }}
        };
        std::map<unsigned, unsigned> const signalDataMap{
            {0, 0},
            {1, 1},
            {2, 2},
            {3, 3}};

        std::vector<int16_t> signalDataOut{};
        ChannelRemapping const remappingData{
            8,
            {
                {0, {0, false}},
                {1, {1, false}},
                {2, {2, false}},
                {3, {3, false}}
            }
        };
        std::vector<std::complex<float>> coeData{};

        try {
            processSignal(signalDataIn, signalDataMap, signalDataOut, coeData, remappingData);
            failTest();
        } catch (std::invalid_argument& e) {
        }
    }},
    {"processSignals() Invalid coefficant data, not a multiple of the number of channels", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn{
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }}
        };
        std::map<unsigned, unsigned> const signalDataMap{
            {0, 0},
            {1, 1},
            {2, 2},
            {3, 3}};

        std::vector<int16_t> signalDataOut{};
        ChannelRemapping const remappingData{
            4,
            {
                {0, {0, false}},
                {1, {1, false}},
                {2, {2, false}},
                {3, {3, false}}
            }
        };
        std::vector<std::complex<float>> coeData{ { 0.0f, 0.0f } };

        try {
            processSignal(signalDataIn, signalDataMap, signalDataOut, coeData, remappingData);
            failTest();
        } catch (std::invalid_argument& e) {
        }
    }},
    {"processSignals() Invalid coefficant data, (More blocks than signal data)", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn{
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }},
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0 }}
        };
        std::map<unsigned, unsigned> const signalDataMap{
            {0, 0},
            {1, 1},
            {2, 2},
            {3, 3}};

        std::vector<int16_t> signalDataOut{};
        ChannelRemapping const remappingData{
            4,
            {
                {0, {0, false}},
                {1, {1, false}},
                {2, {2, false}},
                {3, {3, false}}
            }
        };

        std::vector<std::complex<float>> coeData(filterSize * 5, { 0.0f, 0.0f } );

        try {
            processSignal(signalDataIn, signalDataMap, signalDataOut, coeData, remappingData);
            failTest();
        } catch (std::invalid_argument& e) {
        }
    }},
    // This test should do nothing to the data as the mapping is exactly the same apart from the nyquist scaling
    {"remapChannels() contiguous input ( No Conjagation, No Nyquist channel )", []() {
       std::vector<std::vector<std::complex<float>>> const signalDataIn {
            { { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 2.0f, 2.0f }, { 3.0f, 3.0f }, { 4.0f, 4.0f }, { 5.0f, 5.0f }, { 6.0f, 6.0f }, { 7.0f, 7.0f } },
            { { 0.1f, 0.1f }, { 1.1f, 1.1f }, { 2.1f, 2.1f }, { 3.1f, 3.1f }, { 4.1f, 4.1f }, { 5.1f, 5.1f }, { 6.1f, 6.1f }, { 7.1f, 7.1f } },
            { { 0.2f, 0.2f }, { 1.2f, 1.2f }, { 2.2f, 2.2f }, { 3.2f, 3.2f }, { 4.2f, 4.2f }, { 5.2f, 5.2f }, { 6.2f, 6.2f }, { 7.2f, 7.2f } },
            { { 0.3f, 0.3f }, { 1.3f, 1.3f }, { 2.3f, 2.3f }, { 3.3f, 3.3f }, { 4.3f, 4.3f }, { 5.3f, 5.3f }, { 6.3f, 6.3f }, { 7.3f, 7.3f } },
            { { 0.4f, 0.4f }, { 1.4f, 1.4f }, { 2.4f, 2.4f }, { 3.4f, 3.4f }, { 4.4f, 4.4f }, { 5.4f, 5.4f }, { 6.4f, 6.4f }, { 7.4f, 7.4f } }
        };

        std::map<unsigned, unsigned> const signalDataMap {
            {0, 0},
            {1, 1},
            {2, 2},
            {3, 3},
            {4, 4}
        };

        std::map<unsigned, ChannelRemapping::RemappedChannel> const channelRemapping{
                {0, {0, false}},
                {1, {1, false}},
                {2, {2, false}},
                {3, {3, false}},
                {4, {4, false}}
        };

        std::vector<std::complex<float>> const expected {
            { 0.0f, 0.0f }, { 0.1f, 0.1f }, { 0.2f, 0.2f }, { 0.3f, 0.3f }, { 0.4f, 0.4f }, { 0.0f, 0.0f },
            { 1.0f, 1.0f }, { 1.1f, 1.1f }, { 1.2f, 1.2f }, { 1.3f, 1.3f }, { 1.4f, 1.4f }, { 0.0f, 0.0f },
            { 2.0f, 2.0f }, { 2.1f, 2.1f }, { 2.2f, 2.2f }, { 2.3f, 2.3f }, { 2.4f, 2.4f }, { 0.0f, 0.0f },
            { 3.0f, 3.0f }, { 3.1f, 3.1f }, { 3.2f, 3.2f }, { 3.3f, 3.3f }, { 3.4f, 3.4f }, { 0.0f, 0.0f },
            { 4.0f, 4.0f }, { 4.1f, 4.1f }, { 4.2f, 4.2f }, { 4.3f, 4.3f }, { 4.4f, 4.4f }, { 0.0f, 0.0f },
            { 5.0f, 5.0f }, { 5.1f, 5.1f }, { 5.2f, 5.2f }, { 5.3f, 5.3f }, { 5.4f, 5.4f }, { 0.0f, 0.0f },
            { 6.0f, 6.0f }, { 6.1f, 6.1f }, { 6.2f, 6.2f }, { 6.3f, 6.3f }, { 6.4f, 6.4f }, { 0.0f, 0.0f },
            { 7.0f, 7.0f }, { 7.1f, 7.1f }, { 7.2f, 7.2f }, { 7.3f, 7.3f }, { 7.4f, 7.4f }, { 0.0f, 0.0f }};

        std::vector<std::complex<float>> actual{};

        remapChannels(signalDataIn, signalDataMap, actual, channelRemapping, 6);

        testAssert(actual == expected);
    }},
    {"remapChannels() contiguous input ( All Conjagation, Nyquist Scaling )", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn {
            { { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 2.0f, 2.0f }, { 3.0f, 3.0f }, { 4.0f, 4.0f }, { 5.0f, 5.0f }, { 6.0f, 6.0f }, { 7.0f, 7.0f } },
            { { 0.1f, 0.1f }, { 1.1f, 1.1f }, { 2.1f, 2.1f }, { 3.1f, 3.1f }, { 4.1f, 4.1f }, { 5.1f, 5.1f }, { 6.1f, 6.1f }, { 7.1f, 7.1f } },
            { { 0.2f, 0.2f }, { 1.2f, 1.2f }, { 2.2f, 2.2f }, { 3.2f, 3.2f }, { 4.2f, 4.2f }, { 5.2f, 5.2f }, { 6.2f, 6.2f }, { 7.2f, 7.2f } },
            { { 0.3f, 0.3f }, { 1.3f, 1.3f }, { 2.3f, 2.3f }, { 3.3f, 3.3f }, { 4.3f, 4.3f }, { 5.3f, 5.3f }, { 6.3f, 6.3f }, { 7.3f, 7.3f } },
            { { 0.4f, 0.4f }, { 1.4f, 1.4f }, { 2.4f, 2.4f }, { 3.4f, 3.4f }, { 4.4f, 4.4f }, { 5.4f, 5.4f }, { 6.4f, 6.4f }, { 7.4f, 7.4f } }
        };

        std::map<unsigned, unsigned> const signalDataMap {
            {0, 0},
            {1, 1},
            {2, 2},
            {3, 3},
            {4, 4}
        };

        std::map<unsigned, ChannelRemapping::RemappedChannel> const channelRemapping{
                {0, {0, true}},
                {1, {1, true}},
                {2, {2, true}},
                {3, {3, true}},
                {4, {4, true}}
        };

        std::vector<std::complex<float>> const expected{
            { 0.0f, -0.0f }, { 0.1f, -0.1f }, { 0.2f, -0.2f }, { 0.3f, -0.3f }, { 0.8f, -0.8f },
            { 1.0f, -1.0f }, { 1.1f, -1.1f }, { 1.2f, -1.2f }, { 1.3f, -1.3f }, { 2.8f, -2.8f },
            { 2.0f, -2.0f }, { 2.1f, -2.1f }, { 2.2f, -2.2f }, { 2.3f, -2.3f }, { 4.8f, -4.8f },
            { 3.0f, -3.0f }, { 3.1f, -3.1f }, { 3.2f, -3.2f }, { 3.3f, -3.3f }, { 6.8f, -6.8f },
            { 4.0f, -4.0f }, { 4.1f, -4.1f }, { 4.2f, -4.2f }, { 4.3f, -4.3f }, { 8.8f, -8.8f },
            { 5.0f, -5.0f }, { 5.1f, -5.1f }, { 5.2f, -5.2f }, { 5.3f, -5.3f }, { 10.8f, -10.8f },
            { 6.0f, -6.0f }, { 6.1f, -6.1f }, { 6.2f, -6.2f }, { 6.3f, -6.3f }, { 12.8f, -12.8f },
            { 7.0f, -7.0f }, { 7.1f, -7.1f }, { 7.2f, -7.2f }, { 7.3f, -7.3f }, { 14.8f, -14.8f } };

        std::vector<std::complex<float>> actual{};

        remapChannels(signalDataIn, signalDataMap, actual, channelRemapping, 5);

        testAssert(actual == expected);
    }},

    {"remapChannels() contiguous input ( Some Conjagation, Nyquist Scaling )", []() {
       std::vector<std::vector<std::complex<float>>> const signalDataIn {
            { { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 2.0f, 2.0f }, { 3.0f, 3.0f }, { 4.0f, 4.0f }, { 5.0f, 5.0f }, { 6.0f, 6.0f }, { 7.0f, 7.0f } },
            { { 0.1f, 0.1f }, { 1.1f, 1.1f }, { 2.1f, 2.1f }, { 3.1f, 3.1f }, { 4.1f, 4.1f }, { 5.1f, 5.1f }, { 6.1f, 6.1f }, { 7.1f, 7.1f } },
            { { 0.2f, 0.2f }, { 1.2f, 1.2f }, { 2.2f, 2.2f }, { 3.2f, 3.2f }, { 4.2f, 4.2f }, { 5.2f, 5.2f }, { 6.2f, 6.2f }, { 7.2f, 7.2f } },
            { { 0.3f, 0.3f }, { 1.3f, 1.3f }, { 2.3f, 2.3f }, { 3.3f, 3.3f }, { 4.3f, 4.3f }, { 5.3f, 5.3f }, { 6.3f, 6.3f }, { 7.3f, 7.3f } },
            { { 0.4f, 0.4f }, { 1.4f, 1.4f }, { 2.4f, 2.4f }, { 3.4f, 3.4f }, { 4.4f, 4.4f }, { 5.4f, 5.4f }, { 6.4f, 6.4f }, { 7.4f, 7.4f } }
        };

        std::map<unsigned, unsigned> const signalDataMap {
            {0, 0},
            {1, 1},
            {2, 2},
            {3, 3},
            {4, 4}
        };

        std::map<unsigned, ChannelRemapping::RemappedChannel> const channelRemapping{
                {0, {0, true}},
                {1, {1, false}},
                {2, {2, true}},
                {3, {3, false}},
                {4, {4, true}}
        };

        std::vector<std::complex<float>> const expected{
            { 0.0f, -0.0f }, { 0.1f, 0.1f }, { 0.2f, -0.2f }, { 0.3f, 0.3f }, { 0.8f, -0.8f },
            { 1.0f, -1.0f }, { 1.1f, 1.1f }, { 1.2f, -1.2f }, { 1.3f, 1.3f }, { 2.8f, -2.8f },
            { 2.0f, -2.0f }, { 2.1f, 2.1f }, { 2.2f, -2.2f }, { 2.3f, 2.3f }, { 4.8f, -4.8f },
            { 3.0f, -3.0f }, { 3.1f, 3.1f }, { 3.2f, -3.2f }, { 3.3f, 3.3f }, { 6.8f, -6.8f },
            { 4.0f, -4.0f }, { 4.1f, 4.1f }, { 4.2f, -4.2f }, { 4.3f, 4.3f }, { 8.8f, -8.8f },
            { 5.0f, -5.0f }, { 5.1f, 5.1f }, { 5.2f, -5.2f }, { 5.3f, 5.3f }, { 10.8f, -10.8f },
            { 6.0f, -6.0f }, { 6.1f, 6.1f }, { 6.2f, -6.2f }, { 6.3f, 6.3f }, { 12.8f, -12.8f },
            { 7.0f, -7.0f }, { 7.1f, 7.1f }, { 7.2f, -7.2f }, { 7.3f, 7.3f }, { 14.8f, -14.8f } };

        std::vector<std::complex<float>> actual{};

        remapChannels(signalDataIn, signalDataMap, actual, channelRemapping, 5);

        testAssert(actual == expected);
    }},

    {"remapChannels() contiguous input ( Block above zero channels, Nyquist Scaling, 0 Channel remap scaling )", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn {
            // Channel 3
                { { 0.3f, 0.3f }, { 1.3f, 1.3f }, { 2.3f, 2.3f }, { 3.3f, 3.3f }, { 4.3f, 4.3f }, { 5.3f, 5.3f },
                { 6.3f, 6.3f }, { 7.3f, 7.3f }, { 8.3f, 8.3f }, { 9.3f, 9.3f }, { 10.3f, 10.3f }, { 11.3f, 11.3f },
                { 12.3f, 12.3f }, { 13.3f, 13.3f }
            },
            // Channel 4
                { { 0.4f, 0.4f }, { 1.4f, 1.4f }, { 2.4f, 2.4f }, { 3.4f, 3.4f }, { 4.4f, 4.4f }, { 5.4f, 5.4f },
                { 6.4f, 6.4f }, { 7.4f, 7.4f }, { 8.4f, 8.4f }, { 9.4f, 9.4f }, { 10.4f, 10.4f }, { 11.4f, 11.4f },
                { 12.4f, 12.4f }, { 13.4f, 13.4f }
            },
            // Channel 5
                { { 0.5f, 0.5f }, { 1.5f, 1.5f }, { 2.5f, 2.5f }, { 3.5f, 3.5f }, { 4.5f, 4.5f }, { 5.5f, 5.5f },
                { 6.5f, 6.5f }, { 7.5f, 7.5f }, { 8.5f, 8.5f }, { 9.5f, 9.5f }, { 10.5f, 10.5f }, { 11.5f, 11.5f },
                { 12.5f, 12.5f }, { 13.5f, 13.5f }
            },
            // Channel 6
                { { 0.6f, 0.6f }, { 1.6f, 1.6f }, { 2.6f, 2.6f }, { 3.6f, 3.6f }, { 4.6f, 4.6f }, { 5.6f, 5.6f },
                { 6.6f, 6.6f }, { 7.6f, 7.6f }, { 8.6f, 8.6f }, { 9.6f, 9.6f }, { 10.6f, 10.6f }, { 11.6f, 11.6f },
                { 12.6f, 12.6f }, { 13.6f, 13.6f }
            },
            // Channel 7
                { { 0.7f, 0.7f }, { 1.7f, 1.7f }, { 2.7f, 2.7f }, { 3.7f, 3.7f }, { 4.7f, 4.7f }, { 5.7f, 5.7f },
                { 6.7f, 6.7f }, { 7.7f, 7.7f }, { 8.7f, 8.7f }, { 9.7f, 9.7f }, { 10.7f, 10.7f }, { 11.7f, 11.7f },
                { 12.7f, 12.7f }, { 13.7f, 13.7f }
            }
        };

        std::map<unsigned, unsigned> const signalDataMap {
            { 3, 0 },
            { 4, 1 },
            { 5, 2 },
            { 6, 3 },
            { 7, 4 }
        };

        std::map<unsigned, ChannelRemapping::RemappedChannel> const channelRemapping {
                {3, {0, false}},
                {4, {1, false}},
                {5, {2, false}},
                {6, {3, false}},
                {7, {4, false}}
        };
        std::vector<std::complex<float>> const expected {
            { 0.6f, 0.6f }, { 0.4f, 0.4f }, { 0.5f, 0.5f }, { 0.6f, 0.6f }, { 1.4f, 1.4f },
            { 2.6f, 2.6f }, { 1.4f, 1.4f }, { 1.5f, 1.5f }, { 1.6f, 1.6f }, { 3.4f, 3.4f },
            { 4.6f, 4.6f }, { 2.4f, 2.4f }, { 2.5f, 2.5f }, { 2.6f, 2.6f }, { 5.4f, 5.4f },
            { 6.6f, 6.6f }, { 3.4f, 3.4f }, { 3.5f, 3.5f }, { 3.6f, 3.6f }, { 7.4f, 7.4f },
            { 8.6f, 8.6f }, { 4.4f, 4.4f }, { 4.5f, 4.5f }, { 4.6f, 4.6f }, { 9.4f, 9.4f },
            { 10.6f, 10.6f }, { 5.4f, 5.4f }, { 5.5f, 5.5f }, { 5.6f, 5.6f }, { 11.4f, 11.4f },
            { 12.6f, 12.6f }, { 6.4f, 6.4f }, { 6.5f, 6.5f }, { 6.6f, 6.6f }, { 13.4f, 13.4f },
            { 14.6f, 14.6f }, { 7.4f, 7.4f }, { 7.5f, 7.5f }, { 7.6f, 7.6f }, { 15.4f, 15.4f },
            { 16.6f, 16.6f }, { 8.4f, 8.4f }, { 8.5f, 8.5f }, { 8.6f, 8.6f }, { 17.4f, 17.4f },
            { 18.6f, 18.6f }, { 9.4f, 9.4f }, { 9.5f, 9.5f }, { 9.6f, 9.6f }, { 19.4f, 19.4f },
            { 20.6f, 20.6f }, { 10.4f, 10.4f }, { 10.5f, 10.5f }, { 10.6f, 10.6f }, { 21.4f, 21.4f },
            { 22.6f, 22.6f }, { 11.4f, 11.4f }, { 11.5f, 11.5f }, { 11.6f, 11.6f }, { 23.4f, 23.4f },
            { 24.6f, 24.6f }, { 12.4f, 12.4f }, { 12.5f, 12.5f }, { 12.6f, 12.6f }, { 25.4f, 25.4f },
            { 26.6f, 26.6f }, { 13.4f, 13.4f }, { 13.5f, 13.5f }, { 13.6f, 13.6f }, { 27.4f, 27.4f }};

        std::vector<std::complex<float>> actual{};

        remapChannels(signalDataIn, signalDataMap, actual, channelRemapping, 5);
        testAssert(actual == expected);
    }},

    {"remapChannels() contiguous mixed input ( Block above zero channels, Nyquist Scaling )", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn { {
            // Channel 5
                { 0.5f, 0.5f }, { 1.5f, 1.5f }, { 2.5f, 2.5f }, { 3.5f, 3.5f }, { 4.5f, 4.5f }, { 5.5f, 5.5f },
                { 6.5f, 6.5f }, { 7.5f, 7.5f }, { 8.5f, 8.5f }, { 9.5f, 9.5f }, { 10.5f, 10.5f }, { 11.5f, 11.5f },
                { 12.5f, 12.5f }, { 13.5f, 13.5f }, { 14.5f, 14.5f }, { 15.5f, 15.5f }, { 16.5f, 16.5f }, { 17.5f, 17.5f }
            },
            // Channel 6
                { { 0.6f, 0.6f }, { 1.6f, 1.6f }, { 2.6f, 2.6f }, { 3.6f, 3.6f }, { 4.6f, 4.6f }, { 5.6f, 5.6f },
                { 6.6f, 6.6f }, { 7.6f, 7.6f }, { 8.6f, 8.6f }, { 9.6f, 9.6f }, { 10.6f, 10.6f }, { 11.6f, 11.6f },
                { 12.6f, 12.6f }, { 13.6f, 13.6f }, { 14.6f, 14.6f }, { 15.6f, 15.6f }, { 16.6f, 16.6f }, { 17.6f, 17.6f }
            },
            // Channel 7
                { { 0.7f, 0.7f }, { 1.7f, 1.7f }, { 2.7f, 2.7f }, { 3.7f, 3.7f }, { 4.7f, 4.7f }, { 5.7f, 5.7f },
                { 6.7f, 6.7f }, { 7.7f, 7.7f }, { 8.7f, 8.7f }, { 9.7f, 9.7f }, { 10.7f, 10.7f }, { 11.7f, 11.7f },
                { 12.7f, 12.7f }, { 13.7f, 13.7f }, { 14.7f, 14.7f }, { 15.7f, 15.7f }, { 16.7f, 16.7f }, { 17.7f, 17.7f }
            },
            // Channel 8
                { { 0.8f, 0.8f }, { 1.8f, 1.8f }, { 2.8f, 2.8f }, { 3.8f, 3.8f }, { 4.8f, 4.8f }, { 5.8f, 5.8f },
                { 6.8f, 6.8f }, { 7.8f, 7.8f }, { 8.8f, 8.8f }, { 9.8f, 9.8f }, { 10.8f, 10.8f }, { 11.8f, 11.8f },
                { 12.8f, 12.8f }, { 13.8f, 13.8f }, { 14.8f, 14.8f }, { 15.8f, 15.8f }, { 16.8f, 16.8f }, { 17.8f, 17.8f }
            },
            // Channel 9
                { { 0.9f, 0.9f }, { 1.9f, 1.9f }, { 2.9f, 2.9f }, { 3.9f, 3.9f }, { 4.9f, 4.9f }, { 5.9f, 5.9f },
                { 6.9f, 6.9f }, { 7.9f, 7.9f }, { 8.9f, 8.9f }, { 9.9f, 9.9f }, { 10.9f, 10.9f }, { 11.9f, 11.9f },
                { 12.9f, 12.9f }, { 13.9f, 13.9f }, { 14.9f, 14.9f }, { 15.9f, 15.9f }, { 16.9f, 16.9f }, { 17.9f, 17.9f } }
        };

        std::map<unsigned, unsigned> const signalDataMap {
            { 5, 0 },
            { 6, 1 },
            { 7, 2 },
            { 8, 3 },
            { 9, 4 }
        };


        std::map<unsigned, ChannelRemapping::RemappedChannel> const channelRemapping {
            {5, {5, false}},
            {6, {4, true}},
            {7, {3, true}},
            {8, {2, true}},
            {9, {1, true}},
        };

        std::vector<std::complex<float>> const expected {
            { 0.0f, 0.0f }, { 0.9f, -0.9f }, { 0.8f, -0.8f }, { 0.7f, -0.7f }, { 0.6f, -0.6f }, { 1.0f, 1.0f },
            { 0.0f, 0.0f }, { 1.9f, -1.9f }, { 1.8f, -1.8f }, { 1.7f, -1.7f }, { 1.6f, -1.6f }, { 3.0f, 3.0f },
            { 0.0f, 0.0f }, { 2.9f, -2.9f }, { 2.8f, -2.8f }, { 2.7f, -2.7f }, { 2.6f, -2.6f }, { 5.0f, 5.0f },
            { 0.0f, 0.0f }, { 3.9f, -3.9f }, { 3.8f, -3.8f }, { 3.7f, -3.7f }, { 3.6f, -3.6f }, { 7.0f, 7.0f },
            { 0.0f, 0.0f }, { 4.9f, -4.9f }, { 4.8f, -4.8f }, { 4.7f, -4.7f }, { 4.6f, -4.6f }, { 9.0f, 9.0f },
            { 0.0f, 0.0f }, { 5.9f, -5.9f }, { 5.8f, -5.8f }, { 5.7f, -5.7f }, { 5.6f, -5.6f }, { 11.0f, 11.0f },
            { 0.0f, 0.0f }, { 6.9f, -6.9f }, { 6.8f, -6.8f }, { 6.7f, -6.7f }, { 6.6f, -6.6f }, { 13.0f, 13.0f },
            { 0.0f, 0.0f }, { 7.9f, -7.9f }, { 7.8f, -7.8f }, { 7.7f, -7.7f }, { 7.6f, -7.6f }, { 15.0f, 15.0f },
            { 0.0f, 0.0f }, { 8.9f, -8.9f }, { 8.8f, -8.8f }, { 8.7f, -8.7f }, { 8.6f, -8.6f }, { 17.0f, 17.0f },
            { 0.0f, 0.0f }, { 9.9f, -9.9f }, { 9.8f, -9.8f }, { 9.7f, -9.7f }, { 9.6f, -9.6f }, { 19.0f, 19.0f },
            { 0.0f, 0.0f }, { 10.9f, -10.9f }, { 10.8f, -10.8f }, { 10.7f, -10.7f }, { 10.6f, -10.6f }, { 21.0f, 21.0f },
            { 0.0f, 0.0f }, { 11.9f, -11.9f }, { 11.8f, -11.8f }, { 11.7f, -11.7f }, { 11.6f, -11.6f }, { 23.0f, 23.0f },
            { 0.0f, 0.0f }, { 12.9f, -12.9f }, { 12.8f, -12.8f }, { 12.7f, -12.7f }, { 12.6f, -12.6f }, { 25.0f, 25.0f },
            { 0.0f, 0.0f }, { 13.9f, -13.9f }, { 13.8f, -13.8f }, { 13.7f, -13.7f }, { 13.6f, -13.6f }, { 27.0f, 27.0f },
            { 0.0f, 0.0f }, { 14.9f, -14.9f }, { 14.8f, -14.8f }, { 14.7f, -14.7f }, { 14.6f, -14.6f }, { 29.0f, 29.0f },
            { 0.0f, 0.0f }, { 15.9f, -15.9f }, { 15.8f, -15.8f }, { 15.7f, -15.7f }, { 15.6f, -15.6f }, { 31.0f, 31.0f },
            { 0.0f, 0.0f }, { 16.9f, -16.9f }, { 16.8f, -16.8f }, { 16.7f, -16.7f }, { 16.6f, -16.6f }, { 33.0f, 33.0f },
            { 0.0f, 0.0f }, { 17.9f, -17.9f }, { 17.8f, -17.8f }, { 17.7f, -17.7f }, { 17.6f, -17.6f }, { 35.0f, 35.0f }};

        std::vector<std::complex<float>> actual{};

        remapChannels(signalDataIn, signalDataMap, actual, channelRemapping, 6);

        testAssert(actual == expected);
    }},

    {"performPFB(), same number of blocks", []() {
        unsigned const numOfBlocks = 5;
        unsigned const numOfChannels = 8;
        std::vector<std::complex<float>> signalDataIn {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }};

        std::map<unsigned, ChannelRemapping::RemappedChannel> const channelRemapping {
                {0, {0, false}},
                {1, {1, false}},
                {2, {2, false}},
                {3, {3, false}},
                {4, {4, false}},
                {5, {5, false}},
                {6, {6, false}},
                {7, {7, false}},
        };

        // Generate the coefficantData
        std::vector<std::vector<std::complex<float>>> const coefficantData{
            { { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f } }
        };

        std::map<unsigned, unsigned> const coefficantDataMap{ { 0, 2 }, { 1, 6 } };

        std::vector<std::complex<float>> const coefficantArray = makeCoeArr(coefficantData, coefficantDataMap);

        std::vector<std::complex<float>> expected {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f }};

        performPFB(signalDataIn, coefficantArray, channelRemapping, numOfBlocks, numOfChannels);
        testAssert( signalDataIn == expected );
    }},
    {"performPFB(), different number of blocks", []() {
        unsigned const numOfBlocks = 15;
        unsigned const numOfChannels = 8;
        std::vector<std::complex<float>> signalDataIn {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }};

        std::map<unsigned, ChannelRemapping::RemappedChannel> const channelRemapping {
                {0, {0, false}},
                {1, {1, false}},
                {2, {2, false}},
                {3, {3, false}},
                {4, {4, false}},
                {5, {5, false}},
                {6, {6, false}},
                {7, {7, false}},
        };

        // Generate the coefficantData
        std::vector<std::vector<std::complex<float>>> const coefficantData{
            { { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f } }
        };

        std::map<unsigned, unsigned> const coefficantDataMap{ { 0, 2 }, { 1, 6 } };

        std::vector<std::complex<float>> const coefficantArray = makeCoeArr(coefficantData, coefficantDataMap);

        std::vector<std::complex<float>> expected {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f }};

        performPFB(signalDataIn, coefficantArray, channelRemapping, numOfBlocks, numOfChannels);

        testAssert( signalDataIn == expected );
    }},
    {"performPFB(), remapped channels", []() {
        unsigned const numOfBlocks = 15;
        unsigned const numOfChannels = 8;
        std::vector<std::complex<float>> signalDataIn {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }};

        std::map<unsigned, ChannelRemapping::RemappedChannel> const channelRemapping {
                {5, {2, false}},
                {7, {6, false}},
        };

        // Generate the coefficantData
        std::vector<std::vector<std::complex<float>>> const coefficantData{
            { { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f } }
        };

        std::map<unsigned, unsigned> const coefficantDataMap{ { 0, 5 }, { 1, 7 } };

        std::vector<std::complex<float>> const coefficantArray = makeCoeArr(coefficantData, coefficantDataMap);

        std::vector<std::complex<float>> expected {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 16.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 12.0f, 0.0f }, { 0.0f, 0.0f }};

        performPFB(signalDataIn, coefficantArray, channelRemapping, numOfBlocks, numOfChannels);

        testAssert( signalDataIn == expected );
    }},
    {"performPFB(), checking if it only works on the specified channels", []() {
        unsigned const numOfBlocks = 15;
        unsigned const numOfChannels = 8;
        std::vector<std::complex<float>> signalDataIn {
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 4.0f, 0.0f }, { 1.0f, 0.0f }};

        std::map<unsigned, ChannelRemapping::RemappedChannel> const channelRemapping {
                {5, {2, false}},
                {7, {6, false}},
        };

        // Generate the coefficantData
        std::vector<std::vector<std::complex<float>>> const coefficantData{
            { { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f } }
        };

        std::map<unsigned, unsigned> const coefficantDataMap{ { 0, 5 }, { 1, 7 } };

        std::vector<std::complex<float>> const coefficantArray = makeCoeArr(coefficantData, coefficantDataMap);

        std::vector<std::complex<float>> expected {
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 12.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 12.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 16.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 16.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 20.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 16.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 16.0f, 0.0f }, { 1.0f, 0.0f },
            { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 12.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f }, { 12.0f, 0.0f }, { 1.0f, 0.0f }};

        performPFB(signalDataIn, coefficantArray, channelRemapping, numOfBlocks, numOfChannels);

        testAssert( signalDataIn == expected );
    }},

    {"performPFB(), more complex filter", []() {
        unsigned const numOfBlocks = 15;
        unsigned const numOfChannels = 8;
        std::vector<std::complex<float>> signalDataIn {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }};

        std::map<unsigned, ChannelRemapping::RemappedChannel> const channelRemapping {
                {2, {2, false}},
                {6, {6, false}},
        };

        // Generate the coefficantData
        std::vector<std::vector<std::complex<float>>> const coefficantData{
            { { 1.0f, 0.0f }, { 2.0f, 0.0f }, { 2.0f, 0.0f }, { 2.0f, 0.0f }, { 1.0f, 0.0f } },
            { { 1.0f, 0.0f }, { 2.5f, 0.0f }, { 2.5f, 0.0f }, { 2.5f, 0.0f }, { 1.0f, 0.0f } }
        };

        std::map<unsigned, unsigned> const coefficantDataMap{ { 0, 2 }, { 1, 6 } };

        std::vector<std::complex<float>> const coefficantArray = makeCoeArr(coefficantData, coefficantDataMap);

        std::vector<std::complex<float>> expected {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 24.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 28.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 34.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 32.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 38.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 32.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 38.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 32.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 38.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 32.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 38.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 32.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 38.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 32.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 38.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 32.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 38.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 32.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 38.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 32.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 38.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 32.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 38.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 32.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 38.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 28.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 34.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 24.0f, 0.0f }, { 0.0f, 0.0f }};
        performPFB(signalDataIn, coefficantArray, channelRemapping, numOfBlocks, numOfChannels);

        testAssert( signalDataIn == expected );
    }},

    {"performPFB(), more complex filter and signal", []() {
        unsigned const numOfBlocks = 15;
        unsigned const numOfChannels = 6;
        std::vector<std::complex<float>> signalDataIn {
            { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 3.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 2.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 3.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 2.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 3.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 2.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 3.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 2.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }};

        std::map<unsigned, ChannelRemapping::RemappedChannel> const channelRemapping {
                {0, {0, false}},
                {4, {4, false}},
        };

        // Generate the coefficantData
        std::vector<std::vector<std::complex<float>>> const coefficantData{
            { { 1.0f, 0.0f }, { -2.0f, 0.0f }, { 2.0f, 0.0f }, { 4.0f, 0.0f } },
            { { 1.1f, 0.0f }, { 2.5f, 0.0f }, { 2.5f, 0.0f }, { -2.5f, 0.0f }, }
        };

        std::map<unsigned, unsigned> const coefficantDataMap{ { 0, 0 }, { 1, 4 } };

        std::vector<std::complex<float>> const coefficantArray = makeCoeArr(coefficantData, coefficantDataMap);

        std::vector<std::complex<float>> expected {
            { 4.0f, 0.0f }, { 0.0f, 0.0f },  { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 24.4f, 0.0f }, { 0.0f, 0.0f },
            { 19.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 18.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 5.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 8.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 19.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 18.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 5.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 8.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 19.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 18.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 5.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 8.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 14.4f, 0.0f }, { 0.0f, 0.0f },
            { 18.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 10.0f, 0.0f }, { 0.0f, 0.0f },
            { 16.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }};
        performPFB(signalDataIn, coefficantArray, channelRemapping, numOfBlocks, numOfChannels);

        testAssert( signalDataIn == expected );
    }},
    {"performDFT() Three blocks of cosine waves", []() {

        // Signal in (time domain)
        // 1, 0, -1, 0, 1, 0, -1, 0

        // Signal in (freq domain)
        // 0, 0, 4, 0, 0

        // Remapped data ( New sampling freq of 2 )
        // 8, 0

        // Aliased signal
        // 1, 1
        std::vector<std::complex<float>> inData {
            { 8.0f, 0.0f }, { 0.0f, 0.0f },
            { 8.0f, 0.0f }, { 0.0f, 0.0f },
            { 8.0f, 0.0f }, { 0.0f, 0.0f }
        };

        std::vector<float> expected {
            1.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 1.0f,
        };

        std::vector<float> actual {};

        performDFT(inData, actual, 2, 3, 2, 8);

        testAssert(actual == expected);
    }},
    {"performDFT() Three blocks of sine waves", []() {

        // Signal in (time domain)
        // 0, 1, 0, -1, 0, 1, 0, -1

        // Signal in (freq domain)
        // 0, 0, 0-4j, 0, 0

        // Remapped data ( New sampling freq of 2 )
        // 0-8j, 0

        // Aliased signal
        // 0, 0
        std::vector<std::complex<float>> inData {
            { 0.0f, -8.0f }, { 0.0f, 0.0f },
            { 0.0f, -8.0f }, { 0.0f, 0.0f },
            { 0.0f, -8.0f }, { 0.0f, 0.0f }
        };

        std::vector<float> expected {
            0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
        };

        std::vector<float> actual {};

        performDFT(inData, actual, 2, 3, 2, 8);

        testAssert(actual == expected);
    }},
    {"performDFT() Conjagated remap", []() {
	    std::vector<std::complex<float>> inData {
		    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f },
		    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f },
		    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }
	};

	    std::vector<float> expected {
		    0.4f, 0.0f, 0.0f, 0.0f, -0.4f, 0.0f, 0.0f, 0.0f,
		    0.4f, 0.0f, 0.0f, 0.0f, -0.4f, 0.0f, 0.0f, 0.0f,
		    0.4f, 0.0f, 0.0f, 0.0f, -0.4f, 0.0f, 0.0f, 0.0f
	    };

	    std::vector<float> actual {};

        performDFT(inData, actual, 8, 3, 6, 10);

        testAssert(actual == expected);
    }},
    {"doPostProcessing() regular positive input", []() {
        std::vector<float> const inData {
            1.0f,
            2.65f,
            432.453f,
            23.0f,
            6363.45f};

        std::vector<std::int16_t> const expected {
            1, 2, 432, 23, 6363 };

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }},
    {"doPostProcessing() regular negative input", []() {
        std::vector<float> const inData {
            -32.4f,
            -3734.0f,
            -1.1f,
            -543.2f,
            -2.0f};

        std::vector<std::int16_t> const expected {
            {-32, -3734, -1, -543, -2}};

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }},

    {"doPostProcessing() regular mixed input", []() {
        std::vector<float> const inData {
            -421.3f,
            213.1f,
            1.343f,
            0.0f,
            -42.23f,
            467.3f,
            23.0f};

        std::vector<std::int16_t> const expected {
            {-421, 213, 1, 0, -42, 467, 23}};

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }},

    {"doPostProcessing() mixed with clamping input", []() {

        std::vector<float> const inData {
            -421.3f,
            67.5f,
            34267.0f,
            43.2f,
            -34.0f,
            50000.0f,
            1.54f,
            -64235.2f,
            23.5f,
            -6343.3f,
            476.3f,
            -643398.3f,
            0.0f};

        std::vector<std::int16_t> const expected {
            {-421, 67, MAX_INT16, 43, -34, MAX_INT16, 1, MIN_INT16, 23, -6343, 476, MIN_INT16, 0}};

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }},

    {"doPostProcessing() extreme edge case clamping input", []() {
        std::vector<float> const inData {
            MAX_INT16 + 1,
            MIN_INT16 - 1,
            MAX_INT16 - 1,
            MIN_INT16 + 1,
            MAX_INT16,
            MIN_INT16};

        std::vector<std::int16_t> expected
            {MAX_INT16, MIN_INT16, MAX_INT16 - 1, MIN_INT16 + 1, MAX_INT16, MIN_INT16};

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }}
}} {}

 TestModule signalProcessingTest() {
    return {
        "Signal Processing Test",
        []() { return std::make_unique<SignalProcessingTest>(); }
    };
}
