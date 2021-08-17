#include "InternodeCommunication.hpp"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <mpi.h>

#include "ChannelRemapping.hpp"
#include "Common.hpp"
#include "NodeAntennaInputAssigner.hpp"

// TODO: remove
#include <iostream>


// Raises an exception if the MPI error code does not indicate success.
static void assertMPISuccess(int mpiErrorCode) {
    if (mpiErrorCode != MPI_SUCCESS) {
        throw InternodeCommunicationError{"MPI call failed with code " + std::to_string(mpiErrorCode)};
    }
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
    std::size_t part1Buffer[5] = {
        appConfig.observationID,
        appConfig.signalStartTime,
        inputDirectoryPath.size(),
        invPolyphaseFilterPath.size(),
        outputDirectoryPath.size()
    };
    assertMPISuccess(MPI_Bcast(part1Buffer, 5, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD));

    // Next we will put all the variable-size strings together and send them.
    std::vector<char> part2Buffer(inputDirectoryPath.size() + invPolyphaseFilterPath.size()
        + outputDirectoryPath.size());
    auto it = std::copy(inputDirectoryPath.cbegin(), inputDirectoryPath.cend(), part2Buffer.begin());
    it = std::copy(invPolyphaseFilterPath.cbegin(), invPolyphaseFilterPath.cend(), it);
    std::copy(outputDirectoryPath.cbegin(), outputDirectoryPath.cend(), it);
    assertMPISuccess(MPI_Bcast(part2Buffer.data(), part2Buffer.size(), MPI_CHAR, 0, MPI_COMM_WORLD));
}

void PrimaryNodeCommunicator::sendAntennaConfig(AntennaConfig const& antennaConfig) const {
    // TODO: real implementation
    std::cout << "Node 0: sending antenna config to all secondary nodes" << std::endl;
}

void PrimaryNodeCommunicator::sendChannelRemapping(ChannelRemapping const& channelRemapping) const {
    // TODO: real implementation
    std::cout << "Node 0: sending channel remapping to all secondary nodes" << std::endl;
}

void PrimaryNodeCommunicator::sendAntennaInputAssignment(unsigned node,
        std::optional<AntennaInputRange> const& antennaInputAssignment) const {
    if (node == 0) {
        throw std::invalid_argument{"node must not be the primary node."};
    }
    
    // Represent the assignment as one array so we can send it all with one MPI call.
    unsigned buffer[3];
    if (antennaInputAssignment) {
        buffer[0] = true;
        buffer[1] = antennaInputAssignment->begin;
        buffer[2] = antennaInputAssignment->end;
    }
    else {
        buffer[0] = false;
        buffer[1] = 0;
        buffer[2] = 0;
    }
    assertMPISuccess(MPI_Send(buffer, 3, MPI_UNSIGNED, node, 0, MPI_COMM_WORLD));
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
    std::size_t part1Buffer[5] = {0};
    assertMPISuccess(MPI_Bcast(part1Buffer, 5, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD));
    unsigned const observationID = part1Buffer[0];
    unsigned const signalStartTime = part1Buffer[1];
    auto const inputDirectoryPathSize = part1Buffer[2];
    auto const invPolyphaseFilterPathSize = part1Buffer[3];
    auto const outputDirectoryPathSize = part1Buffer[4];

    // Allocate a buffer for the variable-size data.
    std::vector<char> part2Buffer(inputDirectoryPathSize + invPolyphaseFilterPathSize + outputDirectoryPathSize);
    assertMPISuccess(MPI_Bcast(part2Buffer.data(), part2Buffer.size(), MPI_CHAR, 0, MPI_COMM_WORLD));
    std::string inputDirectoryPath{part2Buffer.data(), inputDirectoryPathSize};
    std::string invPolyphaseFilterPath{part2Buffer.data() + inputDirectoryPathSize, invPolyphaseFilterPathSize};
    std::string outputDirectoryPath{part2Buffer.data() + inputDirectoryPathSize + invPolyphaseFilterPathSize,
        outputDirectoryPathSize};

    return {
        std::move(inputDirectoryPath),
        observationID, signalStartTime,
        std::move(invPolyphaseFilterPath),
        std::move(outputDirectoryPath)
    };
}

AntennaConfig SecondaryNodeCommunicator::receiveAntennaConfig() const {
    // TODO: real implementation
    std::cout << "Node " << _internodeCommunicator->getNodeID() << ": receiving antenna config from primary node" << std::endl;
    return {
        {{0, 'X'}, {0, 'Y'}, {1, 'X'}, {1, 'Y'}, {2, 'X'}, {2, 'Y'}, {3, 'X'}, {3, 'Y'}, {4, 'X'}, {4, 'Y'}, {5, 'X'}, {5, 'Y'}},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
    };
}

ChannelRemapping SecondaryNodeCommunicator::receiveChannelRemapping() const {
    // TODO: real implementation
    std::cout << "Node " << _internodeCommunicator->getNodeID() << ": receiving channel remapping from primary node" << std::endl;
    return {
        512,
        {{0, {0, false}}, {1, {1, false}}, {2, {2, false}}, {3, {3, false}}, {4, {4, false}}, {5, {5, false}},
            {6, {6, false}}, {7, {7, false}}, {8, {8, false}}, {9, {9, false}}}
    };
}

std::optional<AntennaInputRange> SecondaryNodeCommunicator::receiveAntennaInputAssignment() const {
    unsigned buffer[3] = {0};
    assertMPISuccess(MPI_Recv(&buffer, 3, MPI_UNSIGNED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE));
    if (buffer[0]) {
        return AntennaInputRange{buffer[1], buffer[2]};
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
