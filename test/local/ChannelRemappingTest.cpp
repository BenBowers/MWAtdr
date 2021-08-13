#include "ChannelRemappingTest.hpp"

#include "../../src/ChannelRemapping.hpp"
#include "../TestHelper.hpp"


ChannelRemappingTest::ChannelRemappingTest() : TestModule{"Frequency channel remapper unit test", {
    defTestCase("Empty set of channels", {
        auto const remapping = computeChannelRemapping(512, {});
        testAssert((remapping == ChannelRemapping{512, {}}));
    }),
    defTestCase("Single channel", {
        auto const remapping = computeChannelRemapping(512, {93});
        testAssert((remapping == ChannelRemapping{93 * 2, {{93, {0, false}}}}));
    }),
    defTestCase("Just channel 0", {
        auto const remapping = computeChannelRemapping(512, {0});
        testAssert((remapping == ChannelRemapping{2, {{0, {0, false}}}}));
    })
}} {}
