#include "ChannelRemappingTest.hpp"

#include <algorithm>
#include <numeric>
#include <set>
#include <stdexcept>
#include <vector>

#include "../../src/ChannelRemapping.hpp"
#include "../TestHelper.hpp"


ChannelRemappingTest::ChannelRemappingTest() : TestModule{"Frequency channel remapper unit test", {
    {"Empty set of channels", []() {
        auto const actual = computeChannelRemapping(512, {});
        ChannelRemapping const expected{
            512,
            {}
        };
        testAssert(actual == expected);
    }},
    {"Just channel 0", []() {
        auto const actual = computeChannelRemapping(512, {0});
        ChannelRemapping const expected{
            2,
            {
                {0, {0, false}}
            }
        };
        testAssert(actual == expected);
    }},
    {"Just Nyquist frequency channel", []() {
        auto const actual = computeChannelRemapping(512, {256});
        ChannelRemapping const expected{
            2 * 256,
            {
                {256, {0, false}}
            }
        };
        testAssert(actual == expected);
    }},
    {"Single even channel", []() {
        auto const actual = computeChannelRemapping(512, {104});
        ChannelRemapping const expected{
            2 * 104,
            {
                {104, {0, false}}
            }
        };
        testAssert(actual == expected);
    }},
    {"Single odd channel", []() {
        auto const actual = computeChannelRemapping(512, {93});
        ChannelRemapping const expected{
            2 * 93,
            {
                {93, {0, false}}
            }
        };
        testAssert(actual == expected);
    }},
    {"Contiguous block starting at channel 0", []() {
        auto const actual = computeChannelRemapping(512, {0, 1, 2, 3, 4});
        ChannelRemapping const expected{
            2 * 4,
            {
                {0, {0, false}},
                {1, {1, false}},
                {2, {2, false}},
                {3, {3, false}},
                {4, {4, false}}
            }
        };
        testAssert(actual == expected);
    }},
    {"Contiguous block starting above 0", []() {
        auto const actual = computeChannelRemapping(512, {5, 6, 7, 8, 9});
        ChannelRemapping const expected{
            2 * 5,
            {
                {5, {5, false}},
                {6, {4, true}},
                {7, {3, true}},
                {8, {2, true}},
                {9, {1, true}},
            }
        };
        testAssert(actual == expected);
    }},
    {"Jumble of channels", []() {
        // Would like to do more tests like this, but calculating the correct result by hand is quite tedious.
        auto const actual = computeChannelRemapping(512, {7, 18, 47, 53, 71});
        ChannelRemapping const expected{
            14,
            {
                {7, {7, false}},
                {18, {4, false}},
                {47, {5, false}},
                {53, {3, true}},
                {71, {1, false}}
            }
        };
        testAssert(actual == expected);
    }},
    {"Invalid sampling freq", []() {
        try {
            computeChannelRemapping(0, {1, 2, 76, 3});
            failTest();
        }
        catch (std::invalid_argument const&) {}
        try {
            computeChannelRemapping(1, {123, 45, 3, 89, 41, 6, 23});
            failTest();
        }
        catch (std::invalid_argument const&) {}
        try {
            computeChannelRemapping(73, {14, 54, 3, 87});
            failTest();
        }
        catch (std::invalid_argument const&) {}
    }},
    {"Invalid channels", []() {
        try {
            computeChannelRemapping(512, {1, 45, 250, 257, 56, 139});
            failTest();
        }
        catch (std::invalid_argument const&) {}
        try {
            computeChannelRemapping(512, {403});
            failTest();
        }
        catch (std::invalid_argument const&) {}
        try {
            computeChannelRemapping(2, {0, 1, 2});
            failTest();
        }
        catch (std::invalid_argument const&) {}
    }},
    {"Bulk random test", []() {
        // This test doesn't ensure that the computed channel remapping is definitely correct (that is hard to do), but
        // it does do some basic consistency checks on a lot of varying data.

        for (unsigned i = 0; i < 250; ++i) {
            // Generate a random valid sampling frequency and random valid channels.
            unsigned const samplingFreq = ((testRandomEngine() % 300) + 1) * 2;
            unsigned const channelCount = testRandomEngine() % (1 + samplingFreq / 2);
            auto const channels = [&]() -> std::set<unsigned> {
                std::vector<unsigned> allChannels(1 + samplingFreq / 2);
                std::iota(allChannels.begin(), allChannels.end(), 0);
                std::shuffle(allChannels.begin(), allChannels.end(), testRandomEngine);
                return {allChannels.cbegin(), allChannels.cbegin() + channelCount};
            }();

            auto const remapping = computeChannelRemapping(samplingFreq, channels);
            auto const newSamplingFreq = remapping.newSamplingFreq;

            // New sampling frequency can't be 0 or worse than original sampling frequency.
            testAssert(newSamplingFreq > 0 && newSamplingFreq <= samplingFreq);
            // New sampling frequency must be even.
            testAssert(newSamplingFreq % 2 == 0);
            // New Nyquist frequency must be high enough to fit all channels.
            testAssert(1 + newSamplingFreq / 2 >= channels.size());
            // Must have remapped all channels.
            testAssert(remapping.channelMap.size() == channels.size());
            std::set<unsigned> newChannels;
            for (auto const [oldChannel, remappedChannel] : remapping.channelMap) {
                auto const newChannel = remappedChannel.newChannel;
                // Must remap each channel.
                testAssert(channels.count(oldChannel) > 0);
                // New channel must be <= new Nyquist frequency.
                testAssert(newChannel <= newSamplingFreq / 2);
                // New channel must not overlap any other channels.
                testAssert(newChannels.count(newChannel) == 0);
                newChannels.insert(newChannel);
            }
        }
    }}
}} {}
