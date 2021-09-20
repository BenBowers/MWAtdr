#include "SignalProcessingTest.hpp"

#include<map>
#include<vector>
#include<cstdint>
#include<iostream>
#include<limits>
#include<memory>

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

void performPFB(std::vector<std::complex<float>> const& signalData,
                       std::vector<std::complex<float>>& signalDataOut,
                       std::vector<std::complex<float>> const& coefficantPFB,
                       std::map<unsigned, ChannelRemapping::RemappedChannel> const& mapping,
                       unsigned const numOfBlocks,
                       unsigned const numOfChannels);

void performDFT(std::vector<std::complex<float>>& signalData,
                std::vector<float>& outData,
                unsigned const samplingFreq,
                unsigned const numOfBlocks,
                unsigned const numOfChannels);

void doPostProcessing(std::vector<float> const& signalData,
                      std::vector<std::int16_t>& signalDataOut);

class SignalProcessingTest : public StatelessTestModuleImpl {
    public:
        SignalProcessingTest();
};

SignalProcessingTest::SignalProcessingTest() : StatelessTestModuleImpl{{
    {"processingSignals() empty signals", []() {
        std::vector<std::vector<std::complex<float>>> const signalDataIn{};
        std::map<unsigned, unsigned> const signalDataMap{};
        std::vector<int16_t> signalDataOut{};
        ChannelRemapping remappingData{};
        std::vector<std::complex<float>> coeData{};

        try {
            processSignal(signalDataIn, signalDataMap, signalDataOut, coeData, remappingData);
            failTest();
        } catch (std::invalid_argument e) {
        }
    }},
    // This test should do nothing to the data as the mapping is exactly the same
    {"remapChannels() contiguous input ( No Conjagation )", []() {
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
            { 0.0f, 0.0f }, { 0.1f, 0.1f }, { 0.2f, 0.2f }, { 0.3f, 0.3f }, { 0.4f, 0.4f },
            { 1.0f, 1.0f }, { 1.1f, 1.1f }, { 1.2f, 1.2f }, { 1.3f, 1.3f }, { 1.4f, 1.4f },
            { 2.0f, 2.0f }, { 2.1f, 2.1f }, { 2.2f, 2.2f }, { 2.3f, 2.3f }, { 2.4f, 2.4f },
            { 3.0f, 3.0f }, { 3.1f, 3.1f }, { 3.2f, 3.2f }, { 3.3f, 3.3f }, { 3.4f, 3.4f },
            { 4.0f, 4.0f }, { 4.1f, 4.1f }, { 4.2f, 4.2f }, { 4.3f, 4.3f }, { 4.4f, 4.4f },
            { 5.0f, 5.0f }, { 5.1f, 5.1f }, { 5.2f, 5.2f }, { 5.3f, 5.3f }, { 5.4f, 5.4f },
            { 6.0f, 6.0f }, { 6.1f, 6.1f }, { 6.2f, 6.2f }, { 6.3f, 6.3f }, { 6.4f, 6.4f },
            { 7.0f, 7.0f }, { 7.1f, 7.1f }, { 7.2f, 7.2f }, { 7.3f, 7.3f }, { 7.4f, 7.4f } };

        std::vector<std::complex<float>> actual{};

        remapChannels(signalDataIn, signalDataMap, actual, channelRemapping, 5);

        testAssert(actual == expected);
    }},
    {"remapChannels() contiguous input ( All Conjagation )", []() {
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
            { 0.0f, -0.0f }, { 0.1f, -0.1f }, { 0.2f, -0.2f }, { 0.3f, -0.3f }, { 0.4f, -0.4f },
            { 1.0f, -1.0f }, { 1.1f, -1.1f }, { 1.2f, -1.2f }, { 1.3f, -1.3f }, { 1.4f, -1.4f },
            { 2.0f, -2.0f }, { 2.1f, -2.1f }, { 2.2f, -2.2f }, { 2.3f, -2.3f }, { 2.4f, -2.4f },
            { 3.0f, -3.0f }, { 3.1f, -3.1f }, { 3.2f, -3.2f }, { 3.3f, -3.3f }, { 3.4f, -3.4f },
            { 4.0f, -4.0f }, { 4.1f, -4.1f }, { 4.2f, -4.2f }, { 4.3f, -4.3f }, { 4.4f, -4.4f },
            { 5.0f, -5.0f }, { 5.1f, -5.1f }, { 5.2f, -5.2f }, { 5.3f, -5.3f }, { 5.4f, -5.4f },
            { 6.0f, -6.0f }, { 6.1f, -6.1f }, { 6.2f, -6.2f }, { 6.3f, -6.3f }, { 6.4f, -6.4f },
            { 7.0f, -7.0f }, { 7.1f, -7.1f }, { 7.2f, -7.2f }, { 7.3f, -7.3f }, { 7.4f, -7.4f } };

        std::vector<std::complex<float>> actual{};

        remapChannels(signalDataIn, signalDataMap, actual, channelRemapping, 5);

        testAssert(actual == expected);
    }},

    {"remapChannels() contiguous input ( Some Conjagation )", []() {
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
            { 0.0f, -0.0f }, { 0.1f, 0.1f }, { 0.2f, -0.2f }, { 0.3f, 0.3f }, { 0.4f, -0.4f },
            { 1.0f, -1.0f }, { 1.1f, 1.1f }, { 1.2f, -1.2f }, { 1.3f, 1.3f }, { 1.4f, -1.4f },
            { 2.0f, -2.0f }, { 2.1f, 2.1f }, { 2.2f, -2.2f }, { 2.3f, 2.3f }, { 2.4f, -2.4f },
            { 3.0f, -3.0f }, { 3.1f, 3.1f }, { 3.2f, -3.2f }, { 3.3f, 3.3f }, { 3.4f, -3.4f },
            { 4.0f, -4.0f }, { 4.1f, 4.1f }, { 4.2f, -4.2f }, { 4.3f, 4.3f }, { 4.4f, -4.4f },
            { 5.0f, -5.0f }, { 5.1f, 5.1f }, { 5.2f, -5.2f }, { 5.3f, 5.3f }, { 5.4f, -5.4f },
            { 6.0f, -6.0f }, { 6.1f, 6.1f }, { 6.2f, -6.2f }, { 6.3f, 6.3f }, { 6.4f, -6.4f },
            { 7.0f, -7.0f }, { 7.1f, 7.1f }, { 7.2f, -7.2f }, { 7.3f, 7.3f }, { 7.4f, -7.4f } };

        std::vector<std::complex<float>> actual{};

        remapChannels(signalDataIn, signalDataMap, actual, channelRemapping, 5);

        testAssert(actual == expected);
    }},

    {"remapChannels() contiguous input ( Block above zero channels )", []() {
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
            { 0.3f, 0.3f }, { 0.4f, 0.4f }, { 0.5f, 0.5f }, { 0.6f, 0.6f }, { 0.7f, 0.7f },
            { 1.3f, 1.3f }, { 1.4f, 1.4f }, { 1.5f, 1.5f }, { 1.6f, 1.6f }, { 1.7f, 1.7f },
            { 2.3f, 2.3f }, { 2.4f, 2.4f }, { 2.5f, 2.5f }, { 2.6f, 2.6f }, { 2.7f, 2.7f },
            { 3.3f, 3.3f }, { 3.4f, 3.4f }, { 3.5f, 3.5f }, { 3.6f, 3.6f }, { 3.7f, 3.7f },
            { 4.3f, 4.3f }, { 4.4f, 4.4f }, { 4.5f, 4.5f }, { 4.6f, 4.6f }, { 4.7f, 4.7f },
            { 5.3f, 5.3f }, { 5.4f, 5.4f }, { 5.5f, 5.5f }, { 5.6f, 5.6f }, { 5.7f, 5.7f },
            { 6.3f, 6.3f }, { 6.4f, 6.4f }, { 6.5f, 6.5f }, { 6.6f, 6.6f }, { 6.7f, 6.7f },
            { 7.3f, 7.3f }, { 7.4f, 7.4f }, { 7.5f, 7.5f }, { 7.6f, 7.6f }, { 7.7f, 7.7f },
            { 8.3f, 8.3f }, { 8.4f, 8.4f }, { 8.5f, 8.5f }, { 8.6f, 8.6f }, { 8.7f, 8.7f },
            { 9.3f, 9.3f }, { 9.4f, 9.4f }, { 9.5f, 9.5f }, { 9.6f, 9.6f }, { 9.7f, 9.7f },
            { 10.3f, 10.3f }, { 10.4f, 10.4f }, { 10.5f, 10.5f }, { 10.6f, 10.6f }, { 10.7f, 10.7f },
            { 11.3f, 11.3f }, { 11.4f, 11.4f }, { 11.5f, 11.5f }, { 11.6f, 11.6f }, { 11.7f, 11.7f },
            { 12.3f, 12.3f }, { 12.4f, 12.4f }, { 12.5f, 12.5f }, { 12.6f, 12.6f }, { 12.7f, 12.7f },
            { 13.3f, 13.3f }, { 13.4f, 13.4f }, { 13.5f, 13.5f }, { 13.6f, 13.6f }, { 13.7f, 13.7f }};

        std::vector<std::complex<float>> actual{};

        remapChannels(signalDataIn, signalDataMap, actual, channelRemapping, 5);

        testAssert(actual == expected);
    }},

    {"remapChannels() contiguous mixed input ( Block above zero channels )", []() {
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
            { 0.0f, 0.0f }, { 0.9f, -0.9f }, { 0.8f, -0.8f }, { 0.7f, -0.7f }, { 0.6f, -0.6f }, { 0.5f, 0.5f },
            { 0.0f, 0.0f }, { 1.9f, -1.9f }, { 1.8f, -1.8f }, { 1.7f, -1.7f }, { 1.6f, -1.6f }, { 1.5f, 1.5f },
            { 0.0f, 0.0f }, { 2.9f, -2.9f }, { 2.8f, -2.8f }, { 2.7f, -2.7f }, { 2.6f, -2.6f }, { 2.5f, 2.5f },
            { 0.0f, 0.0f }, { 3.9f, -3.9f }, { 3.8f, -3.8f }, { 3.7f, -3.7f }, { 3.6f, -3.6f }, { 3.5f, 3.5f },
            { 0.0f, 0.0f }, { 4.9f, -4.9f }, { 4.8f, -4.8f }, { 4.7f, -4.7f }, { 4.6f, -4.6f }, { 4.5f, 4.5f },
            { 0.0f, 0.0f }, { 5.9f, -5.9f }, { 5.8f, -5.8f }, { 5.7f, -5.7f }, { 5.6f, -5.6f }, { 5.5f, 5.5f },
            { 0.0f, 0.0f }, { 6.9f, -6.9f }, { 6.8f, -6.8f }, { 6.7f, -6.7f }, { 6.6f, -6.6f }, { 6.5f, 6.5f },
            { 0.0f, 0.0f }, { 7.9f, -7.9f }, { 7.8f, -7.8f }, { 7.7f, -7.7f }, { 7.6f, -7.6f }, { 7.5f, 7.5f },
            { 0.0f, 0.0f }, { 8.9f, -8.9f }, { 8.8f, -8.8f }, { 8.7f, -8.7f }, { 8.6f, -8.6f }, { 8.5f, 8.5f },
            { 0.0f, 0.0f }, { 9.9f, -9.9f }, { 9.8f, -9.8f }, { 9.7f, -9.7f }, { 9.6f, -9.6f }, { 9.5f, 9.5f },
            { 0.0f, 0.0f }, { 10.9f, -10.9f }, { 10.8f, -10.8f }, { 10.7f, -10.7f }, { 10.6f, -10.6f }, { 10.5f, 10.5f },
            { 0.0f, 0.0f }, { 11.9f, -11.9f }, { 11.8f, -11.8f }, { 11.7f, -11.7f }, { 11.6f, -11.6f }, { 11.5f, 11.5f },
            { 0.0f, 0.0f }, { 12.9f, -12.9f }, { 12.8f, -12.8f }, { 12.7f, -12.7f }, { 12.6f, -12.6f }, { 12.5f, 12.5f },
            { 0.0f, 0.0f }, { 13.9f, -13.9f }, { 13.8f, -13.8f }, { 13.7f, -13.7f }, { 13.6f, -13.6f }, { 13.5f, 13.5f },
            { 0.0f, 0.0f }, { 14.9f, -14.9f }, { 14.8f, -14.8f }, { 14.7f, -14.7f }, { 14.6f, -14.6f }, { 14.5f, 14.5f },
            { 0.0f, 0.0f }, { 15.9f, -15.9f }, { 15.8f, -15.8f }, { 15.7f, -15.7f }, { 15.6f, -15.6f }, { 15.5f, 15.5f },
            { 0.0f, 0.0f }, { 16.9f, -16.9f }, { 16.8f, -16.8f }, { 16.7f, -16.7f }, { 16.6f, -16.6f }, { 16.5f, 16.5f },
            { 0.0f, 0.0f }, { 17.9f, -17.9f }, { 17.8f, -17.8f }, { 17.7f, -17.7f }, { 17.6f, -17.6f }, { 17.5f, 17.5f }};

        std::vector<std::complex<float>> actual{};

        remapChannels(signalDataIn, signalDataMap, actual, channelRemapping, 6);

        testAssert(actual == expected);
    }},

    {"performPFB()", []() {
        std::vector<std::complex<float>> const signalDataIn {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f },
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 4.0f, 0.0f }, { 0.0f, 0.0f }};

        std::vector<std::complex<float>> const coefficantData(256 * 5, { 0.0f, 0.0f });

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

    }},

    {"performDFT() [0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0]", []() {
        std::vector<std::complex<float>> signalDataIn {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }};
        std::vector<float> actual{};
        performDFT(signalDataIn, actual, 20, 1, 11);
        std::cout << std::endl;
        for(auto num : actual) {
            std::cout << num << ", ";
        }
        std::cout << std::endl;
    }},

    {"performDFT() [0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0]", []() {
        std::vector<std::complex<float>> signalDataIn {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } };
        std::vector<float> actual{};
        performDFT(signalDataIn, actual, 20, 1, 11);
        std::cout << std::endl;
        for(auto num : actual) {
            std::cout << num << ", ";
        }
        std::cout << std::endl;
    }},
    {"performDFT() [1,0,0]", []() {
        std::vector<std::complex<float>> signalDataIn {
            { 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } };

        std::vector<float> actual{};
        performDFT(signalDataIn, actual, 4, 1, 3);
        std::cout << std::endl;
        for(auto num : actual) {
            std::cout << num << ", ";
        }
        std::cout << std::endl;
    }},
    {"performDFT() [0, 0, 0, 0, 0, 1]", []() {
        std::vector<std::complex<float>> signalDataIn {
            { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }};
        std::vector<float> actual {};
        performDFT(signalDataIn, actual, 10, 1, 6);
        std::cout << std::endl;
        for(auto num : actual) {
            std::cout << num << ", ";
        }
        std::cout << std::endl;
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
