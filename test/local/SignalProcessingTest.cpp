#include "SignalProcessingTest.hpp"

#include<map>
#include<vector>
#include<cstdint>
#include<iostream>

#include "../../src/SignalProcessing.hpp"
#include "../TestHelper.hpp"
#include "../../src/ChannelRemapping.hpp"

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
            {-421, 67, INT16_MAX, 43, -34, INT16_MAX, 1, INT16_MIN, 23, -6343, 476, INT16_MIN, 0}};

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }},

    {"doPostProcessing() extreme edge case clamping input", []() {
        std::vector<std::complex<float>> const inData {
            {INT16_MAX + 1, 0.0f},
            {INT16_MIN - 1, 0.0f},
            {INT16_MAX - 1, 0.0f},
            {INT16_MIN + 1, 0.0f},
            {INT16_MAX, 0.0f},
            {INT16_MIN, 0.0f}};

        std::vector<std::int16_t> expected {
            {INT16_MAX, INT16_MIN, INT16_MAX - 1, INT16_MIN + 1, INT16_MAX, INT16_MIN}};

        std::vector<std::int16_t> actual {};

        doPostProcessing(inData, actual);

        testAssert(actual == expected);
    }}
}} {}
