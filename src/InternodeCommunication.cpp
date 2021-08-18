#include "InternodeCommunication.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <mpi.h>

#include "ChannelRemapping.hpp"
#include "Common.hpp"
#include "NodeAntennaInputAssigner.hpp"

// TODO: remove
#include <iostream>


// Raises an exception if the MPI error code does not indicate success.
// Note that we don't expect this to ever happen, unless there is a logic error in our code.
#define assertMPISuccess(mpiErrorCode) \
    if ((mpiErrorCode) != MPI_SUCCESS) { \
        throw InternodeCommunicationError{ \
            "MPI call failed with code " + std::to_string(mpiErrorCode) + " (" __FILE__ " : " + std::to_string(__LINE__) + ")"}; \
    }


unsigned InternodeCommunicator::getNodeID() const {
    int id = 0;
    assertMPISuccess(MPI_Comm_rank(MPI_COMM_WORLD, &id));
    return id;
}

unsigned InternodeCommunicator::getNodeCount() const {
    int count = 0;
    assertMPISuccess(MPI_Comm_size(MPI_COMM_WORLD, &count));
    return count;
}

PrimaryNodeCommunicator InternodeCommunicator::getPrimaryNodeCommunicator() const {
    return PrimaryNodeCommunicator{shared_from_this()};
}

SecondaryNodeCommunicator InternodeCommunicator::getSecondaryNodeCommunicator() const {
    return SecondaryNodeCommunicator{shared_from_this()};
}

std::shared_ptr<InternodeCommunicator> InternodeCommunicator::init() {
    return {new InternodeCommunicator{}, std::default_delete<InternodeCommunicator>{}};
}

void InternodeCommunicator::sync() const {
    assertMPISuccess(MPI_Barrier(MPI_COMM_WORLD));
}

InternodeCommunicator::InternodeCommunicator() {
    // MPI may only be initialised once.
    if (_initialised.test_and_set()) {
        throw std::logic_error{"Internode communication may only be initialised once."};
    }
    else {
        assertMPISuccess(MPI_Init(nullptr, nullptr));
    }
}

InternodeCommunicator::~InternodeCommunicator() {
    MPI_Finalize();
}

std::atomic_flag InternodeCommunicator::_initialised = ATOMIC_FLAG_INIT;


// For the following communication functions, I will note that I am not an MPI expert.
// There are likely better ways to do some of this communication, for example using custom MPI datatypes for our
// structs. However, my approach has been to stick to the simplest MPI functionality for speed and ease of development.
// Apologies in advance.


std::shared_ptr<InternodeCommunicator const> const& PrimaryNodeCommunicator::getBaseCommunicator() const {
    return _internodeCommunicator;
}

void PrimaryNodeCommunicator::sendAppStartupStatus(bool status) const {
    assertMPISuccess(MPI_Bcast(&status, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD));
}

std::map<unsigned, bool> PrimaryNodeCommunicator::receiveNodeSetupStatus() const {
    auto const nodeCount = _internodeCommunicator->getNodeCount();
    // Can't use std::vector<bool> due to specialisation.
    auto const statusBuffer = std::make_unique<bool[]>(nodeCount);
    // Note that the root node must send data, but actually we don't need anything from root, so we will discard it.
    bool const dummyRootValue = true;
    assertMPISuccess(
        MPI_Gather(&dummyRootValue, 1, MPI_CXX_BOOL, statusBuffer.get(), 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD));
    
    std::map<unsigned, bool> result;
    for (std::size_t i = 1; i < nodeCount; ++i) {
        result.emplace(i, statusBuffer[i]);
    }
    return result;
}

void PrimaryNodeCommunicator::sendAppConfig(AppConfig const& appConfig) const {
    auto const& inputDirectoryPath = appConfig.inputDirectoryPath;
    auto const& invPolyphaseFilterPath = appConfig.invPolyphaseFilterPath;
    auto const& outputDirectoryPath = appConfig.outputDirectoryPath;

    // First we will send the fixed-size data, including sizes of the variable-size data (strings).
    std::array<unsigned long long, 5> part1Buffer{
        appConfig.observationID,
        appConfig.signalStartTime,
        inputDirectoryPath.size(),
        invPolyphaseFilterPath.size(),
        outputDirectoryPath.size()
    };
    assertMPISuccess(MPI_Bcast(part1Buffer.data(), part1Buffer.size(), MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD));

    // Next we will put all the variable-size strings together and send them.
    std::vector<char> part2Buffer(inputDirectoryPath.size() + invPolyphaseFilterPath.size()
        + outputDirectoryPath.size());
    auto it = std::copy(inputDirectoryPath.cbegin(), inputDirectoryPath.cend(), part2Buffer.begin());
    it = std::copy(invPolyphaseFilterPath.cbegin(), invPolyphaseFilterPath.cend(), it);
    std::copy(outputDirectoryPath.cbegin(), outputDirectoryPath.cend(), it);
    assertMPISuccess(MPI_Bcast(part2Buffer.data(), part2Buffer.size(), MPI_CHAR, 0, MPI_COMM_WORLD));
}

void PrimaryNodeCommunicator::sendAntennaConfig(AntennaConfig const& antennaConfig) const {
    // First we send the sizes of the antenna input and frequency channel arrays.
    std::array<unsigned long long, 2> part1Buffer{
        antennaConfig.antennaInputs.size(),
        antennaConfig.frequencyChannels.size()
    };
    assertMPISuccess(MPI_Bcast(part1Buffer.data(), part1Buffer.size(), MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD));

    // Next we send the antenna inputs and frequency channels.
    // We can represent it all as an array of unsigned.
    std::vector<unsigned> part2Buffer;
    part2Buffer.reserve((2 * antennaConfig.antennaInputs.size() + antennaConfig.frequencyChannels.size()));
    for (auto const& antennaInputID : antennaConfig.antennaInputs) {
        part2Buffer.push_back(antennaInputID.tile);
        part2Buffer.push_back(static_cast<char>(antennaInputID.signalChain));
    }
    for (auto const channel : antennaConfig.frequencyChannels) {
        part2Buffer.push_back(channel);
    }
    assertMPISuccess(MPI_Bcast(part2Buffer.data(), part2Buffer.size(), MPI_UNSIGNED, 0, MPI_COMM_WORLD));
}

void PrimaryNodeCommunicator::sendChannelRemapping(ChannelRemapping const& channelRemapping) const {
    // First we will send the fixed-size data, include the size of the channel map.
    std::array<unsigned, 2> part1Buffer{
        channelRemapping.newSamplingFreq,
        static_cast<unsigned>(channelRemapping.channelMap.size())
    };
    assertMPISuccess(MPI_Bcast(part1Buffer.data(), part1Buffer.size(), MPI_UNSIGNED, 0, MPI_COMM_WORLD));

    // Next we will send the variable-size channel map.
    // We can represent single remapped channel as 3 unsigned ints.
    std::vector<unsigned> part2Buffer;
    part2Buffer.reserve(channelRemapping.channelMap.size() * 3);
    for (auto const& [oldChannel, remappedChannel] : channelRemapping.channelMap) {
        part2Buffer.push_back(oldChannel);
        part2Buffer.push_back(remappedChannel.newChannel);
        part2Buffer.push_back(remappedChannel.flipped);
    }
    assertMPISuccess(MPI_Bcast(part2Buffer.data(), part2Buffer.size(), MPI_UNSIGNED, 0, MPI_COMM_WORLD));
}

void PrimaryNodeCommunicator::sendAntennaInputAssignment(unsigned node,
        std::optional<AntennaInputRange> const& antennaInputAssignment) const {
    if (node == 0) {
        throw std::invalid_argument{"node must not be the primary node."};
    }
    
    // Represent the assignment as one array so we can send it all with one MPI call.
    std::array<unsigned, 3> buffer;
    if (antennaInputAssignment) {
        buffer = {true, antennaInputAssignment.value().begin, antennaInputAssignment.value().end};
    }
    else {
        buffer = {false, 0, 0};
    }
    assertMPISuccess(MPI_Send(buffer.data(), buffer.size(), MPI_UNSIGNED, node, 0, MPI_COMM_WORLD));
}

std::map<unsigned, ObservationProcessingResults> PrimaryNodeCommunicator::receiveProcessingResults() const {
    auto const nodeCount = _internodeCommunicator->getNodeCount();
    std::map<unsigned, ObservationProcessingResults> results;
    // TODO: real implementation
    for (unsigned i = 1; i < nodeCount; ++i) {
        std::cout << "Node 0: Receiving processing results from secondary node " << i << std::endl;
        results[i] = {};
    }
    return results;
}

PrimaryNodeCommunicator::PrimaryNodeCommunicator(std::shared_ptr<InternodeCommunicator const> internodeCommunicator) :
    _internodeCommunicator{internodeCommunicator}
{
    if (_internodeCommunicator->getNodeID() != 0) {
        throw std::logic_error{"PrimaryNodeCommunicator must only be used with node 0."};
    }
}


std::shared_ptr<InternodeCommunicator const> const& SecondaryNodeCommunicator::getBaseCommunicator() const {
    return _internodeCommunicator;
}

bool SecondaryNodeCommunicator::receiveAppStartupStatus() const {
    bool status = false;
    assertMPISuccess(MPI_Bcast(&status, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD));
    return status;
}

void SecondaryNodeCommunicator::sendNodeSetupStatus(bool status) const {
    assertMPISuccess(MPI_Gather(&status, 1, MPI_CXX_BOOL, nullptr, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD));
}

AppConfig SecondaryNodeCommunicator::receiveAppConfig() const {
    // Receive the fixed-size data.
    std::array<unsigned long long, 5> part1Buffer{};
    assertMPISuccess(MPI_Bcast(part1Buffer.data(), part1Buffer.size(), MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD));
    auto const [
        observationID,
        signalStartTime,
        inputDirectoryPathSize,
        invPolyphaseFilterPathSize,
        outputDirectoryPathSize
    ] = part1Buffer;

    // Allocate a buffer for the variable-size data.
    std::vector<char> part2Buffer(inputDirectoryPathSize + invPolyphaseFilterPathSize + outputDirectoryPathSize);
    assertMPISuccess(MPI_Bcast(part2Buffer.data(), part2Buffer.size(), MPI_CHAR, 0, MPI_COMM_WORLD));

    return {
        {part2Buffer.data(), inputDirectoryPathSize},
        static_cast<unsigned>(observationID),
        static_cast<unsigned>(signalStartTime),
        {part2Buffer.data() + inputDirectoryPathSize, invPolyphaseFilterPathSize},
        {part2Buffer.data() + inputDirectoryPathSize + invPolyphaseFilterPathSize, outputDirectoryPathSize}
    };
}

AntennaConfig SecondaryNodeCommunicator::receiveAntennaConfig() const {
    // First we receive the sizes of the antenna input and frequency channel arrays.
    std::array<unsigned long long, 2> part1Buffer{};
    assertMPISuccess(MPI_Bcast(part1Buffer.data(), part1Buffer.size(), MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD));
    auto const [antennaInputCount, channelCount] = part1Buffer;

    // Then we receive the antenna input and frequency channel data.
    std::vector<unsigned> part2Buffer(2 * antennaInputCount + channelCount);
    assertMPISuccess(MPI_Bcast(part2Buffer.data(), part2Buffer.size(), MPI_UNSIGNED, 0, MPI_COMM_WORLD));

    AntennaConfig result{};
    result.antennaInputs.reserve(antennaInputCount);
    for (std::size_t i = 0; i < antennaInputCount; ++i) {
        auto const tile = part2Buffer.at(2 * i);
        auto const signalChain = part2Buffer.at(2 * i + 1);
        result.antennaInputs.push_back({tile, static_cast<char>(signalChain)});
    }
    for (std::size_t i = 0; i < channelCount; ++i) {
        auto const channel = part2Buffer.at(2 * antennaInputCount + i);
        result.frequencyChannels.insert(channel);
    }
    return result;
}

ChannelRemapping SecondaryNodeCommunicator::receiveChannelRemapping() const {
    // Receive the fixed-size data.
    std::array<unsigned, 2> part1Buffer{};
    assertMPISuccess(MPI_Bcast(part1Buffer.data(), part1Buffer.size(), MPI_UNSIGNED, 0, MPI_COMM_WORLD));
    auto const [newSamplingFreq, channelMapSize] = part1Buffer;

    // Receive the variable-size channel map.
    std::vector<unsigned> part2Buffer(channelMapSize * 3);
    assertMPISuccess(MPI_Bcast(part2Buffer.data(), part2Buffer.size(), MPI_UNSIGNED, 0, MPI_COMM_WORLD));

    ChannelRemapping result{newSamplingFreq, {}};
    for (std::size_t i = 0; i < channelMapSize; ++i) {
        auto const oldChannel = part2Buffer.at(3 * i);
        auto const newChannel = part2Buffer.at(3 * i + 1);
        auto const flipped = part2Buffer.at(3 * i + 2);
        result.channelMap.emplace(oldChannel, ChannelRemapping::RemappedChannel{newChannel, flipped});
    }
    return result;
}

std::optional<AntennaInputRange> SecondaryNodeCommunicator::receiveAntennaInputAssignment() const {
    std::array<unsigned, 3> buffer{};
    assertMPISuccess(
        MPI_Recv(buffer.data(), buffer.size(), MPI_UNSIGNED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE));
    auto const [hasValue, begin, end] = buffer;
    if (hasValue) {
        return AntennaInputRange{begin, end};
    }
    else {
        return std::nullopt;
    }
}

void SecondaryNodeCommunicator::sendProcessingResults(ObservationProcessingResults const& results) const {
    // TODO: real implementation
    std::cout << "Node " << _internodeCommunicator->getNodeID() << ": sending processing results to primary node" << std::endl;
}

SecondaryNodeCommunicator::SecondaryNodeCommunicator(std::shared_ptr<InternodeCommunicator const> internodeCommunicator) :
    _internodeCommunicator{internodeCommunicator}
{
    if (_internodeCommunicator->getNodeID() == 0) {
        throw std::logic_error{"SecondaryNodeCommunicator cannot be used with node 0."};
    }
}
