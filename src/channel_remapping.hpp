#pragma once

#include <map>
#include <set>


// Represents a remapping of frequency channels.
struct ChannelRemapping {
    // The sampling frequency of the time domain signal after appling the channel remapping, represented as a multiple
    // of the channel bandwidth.
    unsigned newSamplingFreq;
    // Mapping of old channel to new channel. Channels are represented as multiples of the channel bandwidth.
    std::map<unsigned, unsigned> channelMap;
};


// Computes a remapping of frequency channels using Nyquist zone aliasing such that after applying an IFT, the resulting
// time domain signal may be optimally downsampled.
// Parameters:
//  - samplingFreq: the original sampling frequency, as a multiple of the channel bandwidth. Must be an even number.
//  - channels: the original set of frequency channels. Each channel is represented as a multiple of the channel
//      bandwidth. Each channel must be < samplingFreq/2.
ChannelRemapping computeChannelRemapping(unsigned samplingFreq, std::set<unsigned> const& channels);
