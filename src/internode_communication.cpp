#include "internode_communication.hpp"

#include <map>
#include <stdexcept>
#include <string>

#include <mpi.h>

// TODO: include required modules not yet implemented.


// Raises an exception if the MPI error code does not indicate success.
static void assertMPISuccess(int mpiErrorCode) {
    if (mpiErrorCode != MPI_SUCCESS) {
        throw std::runtime_error{"MPI call failed with code " + std::to_string(mpiErrorCode)};
    }
}


InternodeCommunicator::~InternodeCommunicator() {
    MPI_Finalize();
}

unsigned InternodeCommunicator::getNodeID() const {
    assertInstance();

    int id;
    assertMPISuccess(MPI_Comm_rank(MPI_COMM_WORLD, &id));
    return id;
}

unsigned InternodeCommunicator::getNodeCount() const {
    assertInstance();

    int count;
    assertMPISuccess(MPI_Comm_size(MPI_COMM_WORLD, &count));
    return count;
}

PrimaryNodeCommunicator InternodeCommunicator::getPrimaryNodeCommunicator() const {
    assertInstance();
    return PrimaryNodeCommunicator{};
}

SecondaryNodeCommunicator InternodeCommunicator::getSecondaryNodeCommunicator() const {
    assertInstance();
    return SecondaryNodeCommunicator{};
}

void InternodeCommunicator::initInstance() {
    _instance.reset(new InternodeCommunicator{});
}

InternodeCommunicator const& InternodeCommunicator::getInstance() {
    assertInstance();
    return *_instance;
}

void InternodeCommunicator::destroyInstance() {
    _instance.reset();
}

void InternodeCommunicator::assertInstance() {
    if (!_initialised) {
        throw std::logic_error{"Internode communication cannot be used before calling initInstance()"};
    }
    else if (!_instance) {
        throw std::logic_error{"Internode communication cannot be used after calling destroyInstance()"};
    }
}

InternodeCommunicator::InternodeCommunicator() {
    // MPI may only be initialised once.
    if (_initialised) {
        throw std::logic_error{"Internode communication may only be initialised once."};
    }
    else {
        assertMPISuccess(MPI_Init(nullptr, nullptr));
        _initialised = true;
    }
}

std::unique_ptr<InternodeCommunicator> InternodeCommunicator::_instance{};
bool InternodeCommunicator::_initialised = false;


void PrimaryNodeCommunicator::sendAppStartupStatus(bool status) {
    InternodeCommunicator::assertInstance();
    // TODO: real implementation
}

std::map<unsigned, bool> PrimaryNodeCommunicator::receiveNodeSetupStatus() {
    auto const nodeCount = InternodeCommunicator::getInstance().getNodeCount();
    std::map<unsigned, bool> status;
    // TODO: real implementation
    for (unsigned i = 1; i < nodeCount; ++i) {
        status[i] = true;
    }
    return status;
}

void PrimaryNodeCommunicator::sendAppConfig(AppConfig const& appConfig) {
    InternodeCommunicator::assertInstance();
    // TODO: real implementation
}

void PrimaryNodeCommunicator::sendAntennaConfig(AntennaConfig const& antennaConfig) {
    InternodeCommunicator::assertInstance();
    // TODO: real implementation
}

void PrimaryNodeCommunicator::sendChannelRemapping(ChannelRemapping const& channelRemapping) {
    InternodeCommunicator::assertInstance();
    // TODO: real implementation
}

void PrimaryNodeCommunicator::sendAntennaInputAssignment(unsigned node,
        std::optional<AntennaInputRange> const& antennaInputAssignment) {
    InternodeCommunicator::assertInstance();
    // TODO: real implementation
}

std::map<unsigned, ObservationProcessingResults> PrimaryNodeCommunicator::receiveProcessingResults() {
    auto const nodeCount = InternodeCommunicator::getInstance().getNodeCount();
    std::map<unsigned, ObservationProcessingResults> results;
    // TODO: real implementation
    for (unsigned i = 1; i < nodeCount; ++i) {
        results[i] = {};
    }
    return results;
}


bool SecondaryNodeCommunicator::receiveAppStartupStatus() {
    // TODO: real implementation
    return true;
}

void SecondaryNodeCommunicator::sendNodeSetupStatus(bool status) {
    InternodeCommunicator::assertInstance();
    // TODO: real implementation
}

AppConfig SecondaryNodeCommunicator::receiveAppConfig() {
    InternodeCommunicator::assertInstance();
    // TODO: real implementation
    return {
        1000000, 1000008, "/group/mwavcs/myobservation", "/group/mwavcs/reconstructed_observation",
        "/group/mwavcs/inverse_polyphase_filter.bin"
    };
}

AntennaConfig SecondaryNodeCommunicator::receiveAntennaConfig() {
    InternodeCommunicator::assertInstance();
    // TODO: real implementation
    return {
        {{0, 'X'}, {0, 'Y'}, {1, 'X'}, {1, 'Y'}, {2, 'X'}, {2, 'Y'}, {3, 'X'}, {3, 'Y'}, {4, 'X'}, {4, 'Y'}, {5, 'X'}, {5, 'Y'}},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
    };
}

ChannelRemapping SecondaryNodeCommunicator::receiveChannelRemapping() {
    InternodeCommunicator::assertInstance();
    // TODO: real implementation
    return {
        512,
        {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}, {8, 8}, {9, 9}}
    };
}

std::optional<AntennaInputRange> SecondaryNodeCommunicator::receiveAntennaInputAssignment() {
    // TODO: real implementation
    auto const nodeID = InternodeCommunicator::getInstance().getNodeID();
    return AntennaInputRange{nodeID, nodeID};
}

void SecondaryNodeCommunicator::sendProcessingResults(ObservationProcessingResults const& results) {
    InternodeCommunicator::assertInstance();
    // TODO: real implementation
}
