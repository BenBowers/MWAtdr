#include "channel_remapping.hpp"

#include <stdexcept>


ChannelRemapping computeChannelRemapping(unsigned samplingFreq, std::set<unsigned> const& channels) {
    if (samplingFreq % 2 != 0) {
        throw std::invalid_argument{"samplingFreq must be even"};
    }
    for (auto const channel : channels) {
        if (channel > samplingFreq / 2) {
            throw std::invalid_argument{"Frequency channels must be <= samplingFreq/2"};
        }
    }

    // TODO: real remapping
    ChannelRemapping result{samplingFreq, {}};
    for (auto const channel : channels) {
        result.channelMap.emplace(channel, ChannelRemapping::RemappedChannel{channel, false});
    }
    return result;
}
