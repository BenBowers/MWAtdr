#include "InternodeCommunication.hpp"

#include <atomic>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include <mpi.h>

#include "ChannelRemapping.hpp"
#include "Common.hpp"
#include "NodeAntennaInputAssigner.hpp"

// TODO: remove
#include <iostream>


// Raises an exception if the MPI error code does not indicate success.
static void assertMPISuccess(int mpiErrorCode) {
    if (mpiErrorCode != MPI_SUCCESS) {
        throw std::runtime_error{"MPI call failed with code " + std::to_string(mpiErrorCode)};
    }
}


unsigned InternodeCommunicator::getNodeID() const {
    int id;
    assertMPISuccess(MPI_Comm_rank(MPI_COMM_WORLD, &id));
    return id;
}

unsigned InternodeCommunicator::getNodeCount() const {
    int count;
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


void PrimaryNodeCommunicator::sendAppStartupStatus(bool status) {
    // TODO: real implementation
    std::cout << "Node 0: Sending app startup status to all secondary nodes" << std::endl;
}

std::map<unsigned, bool> PrimaryNodeCommunicator::receiveNodeSetupStatus() {
    auto const nodeCount = _internodeCommunicator->getNodeCount();
    std::map<unsigned, bool> status;
    // TODO: real implementation
    for (unsigned i = 1; i < nodeCount; ++i) {
        std::cout << "Node 0: Receiving node setup status from secondary node " << i << std::endl;
        status[i] = true;
    }
    return status;
}

void PrimaryNodeCommunicator::sendAppConfig(AppConfig const& appConfig) {
    // TODO: real implementation
    std::cout << "Node 0: sending app config to all secondary nodes" << std::endl;
}

void PrimaryNodeCommunicator::sendAntennaConfig(AntennaConfig const& antennaConfig) {
    // TODO: real implementation
    std::cout << "Node 0: sending antenna config to all secondary nodes" << std::endl;
}

void PrimaryNodeCommunicator::sendChannelRemapping(ChannelRemapping const& channelRemapping) {
    // TODO: real implementation
    std::cout << "Node 0: sending channel remapping to all secondary nodes" << std::endl;
}

void PrimaryNodeCommunicator::sendAntennaInputAssignment(unsigned node,
        std::optional<AntennaInputRange> const& antennaInputAssignment) {
    // TODO: real implementation
    std::cout << "Node 0: sending antenna input assignment to secondary node " << node << std::endl;
}

std::map<unsigned, ObservationProcessingResults> PrimaryNodeCommunicator::receiveProcessingResults() {
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
{}


bool SecondaryNodeCommunicator::receiveAppStartupStatus() {
    // TODO: real implementation
    std::cout << "Node " << _internodeCommunicator->getNodeID() << ": receiving app startup status from primary node" << std::endl;
    return true;
}

void SecondaryNodeCommunicator::sendNodeSetupStatus(bool status) {
    // TODO: real implementation
    std::cout << "Node " << _internodeCommunicator->getNodeID() << ": sending node setup status to primary node" << std::endl;
}

AppConfig SecondaryNodeCommunicator::receiveAppConfig() {
    // TODO: real implementation
    std::cout << "Node " << _internodeCommunicator->getNodeID() << ": receiving app config from primary node" << std::endl;
    return {
        "./myobservation", 1000000, 1000008, "./inverse_polyphase_filter.bin",
        "./reconstructed_observation"
    };
}

AntennaConfig SecondaryNodeCommunicator::receiveAntennaConfig() {
    // TODO: real implementation
    std::cout << "Node " << _internodeCommunicator->getNodeID() << ": receiving antenna config from primary node" << std::endl;
    return {
        {{0, 'X'}, {0, 'Y'}, {1, 'X'}, {1, 'Y'}, {2, 'X'}, {2, 'Y'}, {3, 'X'}, {3, 'Y'}, {4, 'X'}, {4, 'Y'}, {5, 'X'}, {5, 'Y'}},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
    };
}

ChannelRemapping SecondaryNodeCommunicator::receiveChannelRemapping() {
    // TODO: real implementation
    std::cout << "Node " << _internodeCommunicator->getNodeID() << ": receiving channel remapping from primary node" << std::endl;
    return {
        512,
        {{0, {0, false}}, {1, {1, false}}, {2, {2, false}}, {3, {3, false}}, {4, {4, false}}, {5, {5, false}},
            {6, {6, false}}, {7, {7, false}}, {8, {8, false}}, {9, {9, false}}}
    };
}

std::optional<AntennaInputRange> SecondaryNodeCommunicator::receiveAntennaInputAssignment() {
    // TODO: real implementation
    auto const nodeID = _internodeCommunicator->getNodeID();
    std::cout << "Node " << nodeID << ": receiving antenna input assignment from primary node" << std::endl;
    return AntennaInputRange{nodeID, nodeID};
}

void SecondaryNodeCommunicator::sendProcessingResults(ObservationProcessingResults const& results) {
    // TODO: real implementation
    std::cout << "Node " << _internodeCommunicator->getNodeID() << ": sending processing results to primary node" << std::endl;
}

SecondaryNodeCommunicator::SecondaryNodeCommunicator(std::shared_ptr<InternodeCommunicator const> internodeCommunicator) :
    _internodeCommunicator{internodeCommunicator}
{}
