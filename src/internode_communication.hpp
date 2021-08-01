#pragma once

#include <map>
#include <memory>
#include <optional>


class PrimaryNodeCommunicator;
class SecondaryNodeCommunicator;


// Represents and manages the communication channel between nodes (processes).
class InternodeCommunicator {
public:
    InternodeCommunicator(InternodeCommunicator const&) = delete;
    InternodeCommunicator(InternodeCommunicator&&) = delete;

    ~InternodeCommunicator();

    // Gets the ID of this node. ID 0 represents the primary node.
    unsigned getNodeID() const;

    // Gets the total number of nodes (including primary and secondary).
    unsigned getNodeCount() const;

    // Gets a PrimaryNodeCommunicator that may be used by the primary node to communicate with the secondary nodes.
    PrimaryNodeCommunicator getPrimaryNodeCommunicator() const;

    // Gets a SecondaryNodeCommunicator that may be used by the secondary nodes to communicate with the primary node.
    SecondaryNodeCommunicator getSecondaryNodeCommunicator() const;

    InternodeCommunicator& operator=(InternodeCommunicator const&) = delete;
    InternodeCommunicator& operator=(InternodeCommunicator&&) = delete;

    // Initialises the internode communication, and constructs the InternodeCommunicator singleton instance.
    // Must be called before any other communication functionality. Cannot be called more than once.
    static void initInstance();

    // Gets the InternodeCommunicator singleton instance.
    static InternodeCommunicator const& getInstance();

    // Destroys the InternodeCommunicator singleton instance and terminates internode communication.
    // Should be called once you're done with internode communication (i.e. at the end of the program).
    static void destroyInstance();

    // Raises an exception if the internode communication has not yet been initialised, or if it has been terminated.
    static void assertInstance();

private:
    InternodeCommunicator();

    // The singleton instance (even though this class doesn't have any per-instance state).
    static std::unique_ptr<InternodeCommunicator> _instance;

    // Indicates if internode communication has been initialised before.
    static bool _initialised;
};


// Provides the primary node's side of the internode communication.
// Before using this class, internode communication must be initialised by the InternodeCommunicator class.
// All communication methods are blocking. If the receiver/sender on the other side doesn't coorperate as expected, a
// deadlock may occur.
class PrimaryNodeCommunicator {
public:
    PrimaryNodeCommunicator(PrimaryNodeCommunicator const&) = default;
    PrimaryNodeCommunicator(PrimaryNodeCommunicator&&) = default;

    // Informs all the secondary nodes if the application start up was ok (i.e. valid application configuration).
    // Corresponding receive method is SecondaryNodeCommunicator::receiveAppStartupStatus().
    void sendAppStartupStatus(bool status);

    // Receives the status of the per-node setup from all the secondary nodes.
    // Corresponding send method is SecondaryNodeCommunicator::sendNodeSetupStatus().
    std::map<unsigned, bool> receiveNodeSetupStatus();

    // Sends the application configuration to all the secondary nodes.
    // Corresponding receive method is SecondaryNodeCommunicator::receiveAppConfig().
    void sendAppConfig(AppConfig const& appConfig);

    // Sends the antenna input configuration to all the secondary nodes.
    // Corresponding receive method is SecondaryNodeCommunicator::receiveAntennaConfig().
    void sendAntennaConfig(AntennaConfig const& antennaConfig);

    // Sends the frequency channel remapping to all the secondary nodes.
    // Corresponding receive method is SecondaryNodeCommunicator::receiveChannelRemapping().
    void sendChannelRemapping(ChannelRemapping const& channelRemapping);

    // Sends a secondary node's antenna input assignment to that node.
    // Corresponding receive method is SecondaryNodeCommunicator::receiveAntennaInputAssignment().
    void sendAntennaInputAssignment(unsigned node, std::optional<AntennaInputRange> const& antennaInputAssignment);

    // Receives the observation data processing results from all the secondary nodes.
    // Corresponding send method is SecondaryNodeCommunicator::sendProcessingResults().
    std::map<unsigned, ObservationProcessingResults> receiveProcessingResults();

    PrimaryNodeCommunicator& operator=(PrimaryNodeCommunicator const&) = default;
    PrimaryNodeCommunicator& operator=(PrimaryNodeCommunicator&&) = default;

private:
    PrimaryNodeCommunicator() = default;

    friend class InternodeCommunicator;
};


// Provides the secondary nodes' side of the internode communication.
// Before using this class, internode communication must be initialised by the InternodeCommunicator class.
// All communication methods are blocking. If the receiver/sender on the other side doesn't coorperate as expected, a
// deadlock may occur.
class SecondaryNodeCommunicator {
public:
    SecondaryNodeCommunicator(SecondaryNodeCommunicator const&) = default;
    SecondaryNodeCommunicator(SecondaryNodeCommunicator&&) = default;

    // Receives the status that says if the application startup was ok (i.e. valid application configuration).
    // Corresponding send method is PrimaryNodeCommunicator::sendAppStartupStatus().
    bool receiveAppStartupStatus();

    // Sends the per-node setup status for this node.
    // Corresponding receive methods is PrimaryNodeCommunicator::receiveNodeSetupStatus().
    void sendNodeSetupStatus(bool status);

    // Receives the application configuration.
    // Corresponding send method is PrimaryNodeCommunicator::sendAppConfig().
    AppConfig receiveAppConfig();

    // Receives the antenna input configuration.
    // Corresponding send method is PrimaryNodeCommunicator::sendAntennaConfig().
    AntennaConfig receiveAntennaConfig();

    // Receives the frequency channel remapping.
    // Corresponding send method is PrimaryNodeCommunicator::sendChannelRemapping().
    ChannelRemapping receiveChannelRemapping();

    // Receives the antenna input assignment for this node.
    // Corresponding send method is PrimaryNodeCommunicator::sendAntennaInputAssignment().
    std::optional<AntennaInputRange> receiveAntennaInputAssignment();

    // Sends the observation processing results for this node.
    // Corresponding receive method is PrimaryNodeCommunicator::receiveProcessingResults().
    void sendProcessingResults(ObservationProcessingResults const& results);

    SecondaryNodeCommunicator& operator=(SecondaryNodeCommunicator const&) = default;
    SecondaryNodeCommunicator& operator=(SecondaryNodeCommunicator&&) = default;

private:
    SecondaryNodeCommunicator() = default;

    friend class InternodeCommunicator;
};
