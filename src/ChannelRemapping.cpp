#include "ChannelRemapping.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>


// Aliases a channel using a given Nyquist frequency.
static ChannelRemapping::RemappedChannel aliasChannel(unsigned channel, unsigned nyquistFreq) {
    auto const samplingFreq = 2 * nyquistFreq;
    auto const diff = channel % samplingFreq;
    if (diff <= nyquistFreq) {
        return {diff, false};
    }
    else {
        return {samplingFreq - diff, true};
    }
}


ChannelRemapping computeChannelRemapping(unsigned samplingFreq, std::set<unsigned> const& channels) {
    if (samplingFreq <= 0) {
        throw std::invalid_argument{"samplingFreq must be > 0"};
    }
    for (auto const channel : channels) {
        if (channel > samplingFreq / 2) {
            throw std::invalid_argument{"Frequency channels must be <= samplingFreq/2"};
        }
    }

    // Edge cases which cause trouble for the general algorithm.
    if (channels.size() == 0) {
        return {samplingFreq, {}};
    }
    else if (channels.size() == 1) {
        auto const channel = *channels.cbegin();
        // Note: if there is only channel 0, new sampling frequency becomes 1.
        // Otherwise, new sampling frequency becomes channel (not 2x channel).
        auto const newSamplingFreq = std::max<unsigned>(channel, 1);
        return {newSamplingFreq, {{channel, {0, false}}}};
    }
    else {
        // If we have N channels, then we need at very least we need N new channels <= the new Nyquist frequency.
        unsigned const minNyquistFreq = channels.size() - 1;

        unsigned newNyquistFreq;
        std::map<unsigned, ChannelRemapping::RemappedChannel> remapping;
        std::set<unsigned> newChannels;
        for (newNyquistFreq = minNyquistFreq; ; newNyquistFreq += 1) {
            bool valid = true;
            for (auto const channel : channels) {
                auto const alias = aliasChannel(channel, newNyquistFreq);
                if (newChannels.count(alias.newChannel) > 0) {
                    // Alias is already filled by another channel.
                    valid = false;
                    break;
                }
                else {
                    newChannels.insert(alias.newChannel);
                    remapping[channel] = alias;
                }
            }

            // Since we are exploring new sampling frequencies from low to high, the first valid remapping is optimal.
            if (valid) {
                break;
            }

            remapping.clear();
            newChannels.clear();
        }

        auto const newSamplingFreq = 2 * newNyquistFreq;
        return {newSamplingFreq, std::move(remapping)};
    }
}


bool operator==(ChannelRemapping const& lhs, ChannelRemapping const& rhs) {
    return lhs.newSamplingFreq == rhs.newSamplingFreq && lhs.channelMap == rhs.channelMap;
}

bool operator==(ChannelRemapping::RemappedChannel const& lhs, ChannelRemapping::RemappedChannel const& rhs) {
    return lhs.newChannel == rhs.newChannel && lhs.flipped == rhs.flipped;
}
