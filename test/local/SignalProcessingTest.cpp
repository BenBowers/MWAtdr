#include "SignalProcessingTest.hpp"

#include<map>
#include<vector>
#include<cstdint>
#include<iostream>
#include<limits>

#include "../../src/SignalProcessing.hpp"
#include "../TestHelper.hpp"
#include "../../src/ChannelRemapping.hpp"


// Use std::numeric_limits as it is more modern the INT16_MAX macros
constexpr std::int16_t MAX_INT16 = std::numeric_limits<std::int16_t>::max();
constexpr std::int16_t MIN_INT16 = std::numeric_limits<std::int16_t>::min();

// These Function declarations as I don't want them to be publically avalible
// as they are internal, I've made them non static so I can unit test  them

void remapChannels(std::vector<std::complex<float>> const& signalData,
                   std::vector<std::complex<float>>& signalOut,
                   std::map<unsigned, ChannelRemapping::RemappedChannel> const& mapping,
                   unsigned const numberOfSamples,
                   unsigned const newNumChannels);

void performPFB(std::vector<std::complex<float>> const& signalData,
                std::vector<std::complex<float>>& signalOut,
                std::vector<std::complex<float>> const& coefficantPFB,
                std::map<unsigned, ChannelRemapping::RemappedChannel> const& mapping,
                unsigned const numberOfSamples,
                unsigned const numOfChannels);

void performDFT(std::vector<std::complex<float>>& signalData);

void doPostProcessing(std::vector<std::complex<float>> const& signalData,
                      std::vector<std::int16_t>& signalOut);

SignalProcessingTest::SignalProcessingTest() : TestModule{"Signal Processing unit test", {
    // This test should do nothing to the data as the mapping is exactly the same
    {"remapChannels() contiguous input ( No Conjagation )", []() {
        std::vector<std::complex<float>> const signalData{
            { 0.0f, 0.0f }, { 0.1f, 0.1f }, { 0.2f, 0.2f }, { 0.3f, 0.3f }, { 0.4f, 0.4f },
            { 1.0f, 1.0f }, { 1.1f, 1.1f }, { 1.2f, 1.2f }, { 1.3f, 1.3f }, { 1.4f, 1.4f },
            { 2.0f, 2.0f }, { 2.1f, 2.1f }, { 2.2f, 2.2f }, { 2.3f, 2.3f }, { 2.4f, 2.4f },
            { 3.0f, 3.0f }, { 3.1f, 3.1f }, { 3.2f, 3.2f }, { 3.3f, 3.3f }, { 3.4f, 3.4f },
            { 4.0f, 4.0f }, { 4.1f, 4.1f }, { 4.2f, 4.2f }, { 4.3f, 4.3f }, { 4.4f, 4.4f },
            { 5.0f, 5.0f }, { 5.1f, 5.1f }, { 5.2f, 5.2f }, { 5.3f, 5.3f }, { 5.4f, 5.4f },
            { 6.0f, 6.0f }, { 6.1f, 6.1f }, { 6.2f, 6.2f }, { 6.3f, 6.3f }, { 6.4f, 6.4f },
            { 7.0f, 7.0f }, { 7.1f, 7.1f }, { 7.2f, 7.2f }, { 7.3f, 7.3f }, { 7.4f, 7.4f } };

        std::map<unsigned, ChannelRemapping::RemappedChannel> const mapping{
                {0, {0, false}},
                {1, {1, false}},
                {2, {2, false}},
                {3, {3, false}},
                {4, {4, false}}
        };

        std::vector<std::complex<float>> actual{};

        remapChannels(signalData, actual, mapping, 8, 5);

        testAssert(signalData == actual);
    }},
    {"remapChannels() contiguous input ( All Conjagation )", []() {
        std::vector<std::complex<float>> const signalData{
            { 0.0f, 0.0f }, { 0.1f, 0.1f }, { 0.2f, 0.2f }, { 0.3f, 0.3f }, { 0.4f, 0.4f },
            { 1.0f, 1.0f }, { 1.1f, 1.1f }, { 1.2f, 1.2f }, { 1.3f, 1.3f }, { 1.4f, 1.4f },
            { 2.0f, 2.0f }, { 2.1f, 2.1f }, { 2.2f, 2.2f }, { 2.3f, 2.3f }, { 2.4f, 2.4f },
            { 3.0f, 3.0f }, { 3.1f, 3.1f }, { 3.2f, 3.2f }, { 3.3f, 3.3f }, { 3.4f, 3.4f },
            { 4.0f, 4.0f }, { 4.1f, 4.1f }, { 4.2f, 4.2f }, { 4.3f, 4.3f }, { 4.4f, 4.4f },
            { 5.0f, 5.0f }, { 5.1f, 5.1f }, { 5.2f, 5.2f }, { 5.3f, 5.3f }, { 5.4f, 5.4f },
            { 6.0f, 6.0f }, { 6.1f, 6.1f }, { 6.2f, 6.2f }, { 6.3f, 6.3f }, { 6.4f, 6.4f },
            { 7.0f, 7.0f }, { 7.1f, 7.1f }, { 7.2f, 7.2f }, { 7.3f, 7.3f }, { 7.4f, 7.4f } };

        std::map<unsigned, ChannelRemapping::RemappedChannel> const mapping{
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

        remapChannels(signalData, actual, mapping, 8, 5);

        testAssert(actual == expected);
    }},

    {"remapChannels() contiguous input ( Some Conjagation )", []() {
        std::vector<std::complex<float>> const signalData{
            { 0.0f, 0.0f }, { 0.1f, 0.1f }, { 0.2f, 0.2f }, { 0.3f, 0.3f }, { 0.4f, 0.4f },
            { 1.0f, 1.0f }, { 1.1f, 1.1f }, { 1.2f, 1.2f }, { 1.3f, 1.3f }, { 1.4f, 1.4f },
            { 2.0f, 2.0f }, { 2.1f, 2.1f }, { 2.2f, 2.2f }, { 2.3f, 2.3f }, { 2.4f, 2.4f },
            { 3.0f, 3.0f }, { 3.1f, 3.1f }, { 3.2f, 3.2f }, { 3.3f, 3.3f }, { 3.4f, 3.4f },
            { 4.0f, 4.0f }, { 4.1f, 4.1f }, { 4.2f, 4.2f }, { 4.3f, 4.3f }, { 4.4f, 4.4f },
            { 5.0f, 5.0f }, { 5.1f, 5.1f }, { 5.2f, 5.2f }, { 5.3f, 5.3f }, { 5.4f, 5.4f },
            { 6.0f, 6.0f }, { 6.1f, 6.1f }, { 6.2f, 6.2f }, { 6.3f, 6.3f }, { 6.4f, 6.4f },
            { 7.0f, 7.0f }, { 7.1f, 7.1f }, { 7.2f, 7.2f }, { 7.3f, 7.3f }, { 7.4f, 7.4f } };

        std::map<unsigned, ChannelRemapping::RemappedChannel> const mapping{
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

        remapChannels(signalData, actual, mapping, 8, 5);

        testAssert(actual == expected);
    }},

    {"doPostProcessing() regular positive input", []() {
        std::vector<std::complex<float>> const inData {
            {1.0f, 0.0f},
            {2.65f, 0.0f},
            {432.453f, 0.0f},
            {23.0f, 0.0f},
            {6363.45f, 0.0f}};

        std::vector<std::int16_t> const expected {
            1, 2, 432, 23, 6363 };

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }},

    {"doPostProcessing() regular negative input", []() {
        std::vector<std::complex<float>> const inData {
            {-32.4f, 0.0f},
            {-3734.0f, 0.0f},
            {-1.1f, 0.0f},
            {-543.2f, 0.0f},
            {-2.0f, 0.0f}};

        std::vector<std::int16_t> const expected {
            {-32, -3734, -1, -543, -2}};

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }},

    {"doPostProcessing() regular mixed input", []() {
        std::vector<std::complex<float>> const inData {
            {-421.3f, 0.0f},
            {213.1f, 0.0f},
            {1.343f, 0.0f},
            {0.0f, 0.0f},
            {-42.23f, 0.0f},
            {467.3f, 0.0f},
            {23.0f, 0.0f}};

        std::vector<std::int16_t> const expected {
            {-421, 213, 1, 0, -42, 467, 23}};

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }},

    {"doPostProcessing() mixed with clamping input", []() {

        std::vector<std::complex<float>> const inData {
            {-421.3f, 0.0f},
            {67.5f, 0.0f},
            {34267.0f, 0.0f},
            {43.2f, 0.0f},
            {-34.0f, 0.0f},
            {50000.0f, 0.0f},
            {1.54f, 0.0f},
            {-64235.2f, 0.0f},
            {23.5f, 0.0f},
            {-6343.3f, 0.0f},
            {476.3f, 0.0f},
            {-643398.3f, 0.0f},
            {0.0f, 0.0f}};

        std::vector<std::int16_t> const expected {
            {-421, 67, MAX_INT16, 43, -34, MAX_INT16, 1, MIN_INT16, 23, -6343, 476, MIN_INT16, 0}};

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }},

    {"doPostProcessing() extreme edge case clamping input", []() {
        std::vector<std::complex<float>> const inData {
            {MAX_INT16 + 1, 0.0f},
            {MIN_INT16 - 1, 0.0f},
            {MAX_INT16 - 1, 0.0f},
            {MIN_INT16 + 1, 0.0f},
            {MAX_INT16, 0.0f},
            {MIN_INT16, 0.0f}};

        std::vector<std::int16_t> expected {
            {MAX_INT16, MIN_INT16, MAX_INT16 - 1, MIN_INT16 + 1, MAX_INT16, MIN_INT16}};

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }}
}} {}

