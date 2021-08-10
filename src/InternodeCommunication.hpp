#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <optional>


struct AppConfig;
struct AntennaConfig;
struct AntennaInputRange;
struct ChannelRemapping;
struct ObservationProcessingResults;
class PrimaryNodeCommunicator;
class SecondaryNodeCommunicator;


// Represents and manages the communication channel between nodes (processes).
class InternodeCommunicator : public std::enable_shared_from_this<InternodeCommunicator> {
public:
    InternodeCommunicator(InternodeCommunicator const&) = delete;
    InternodeCommunicator(InternodeCommunicator&&) = delete;

    // Gets the ID of this node. ID 0 represents the primary node.
    unsigned getNodeID() const;

    // Gets the total number of nodes (including primary and secondary).
    unsigned getNodeCount() const;

    // Gets a PrimaryNodeCommunicator that may be used by the primary node to communicate with the secondary nodes.
    // The returned object shares ownership of this InternodeCommunicator via shared_ptr.
    PrimaryNodeCommunicator getPrimaryNodeCommunicator() const;

    // Gets a SecondaryNodeCommunicator that may be used by the secondary nodes to communicate with the primary node.
    // The returned object shares ownership of this InternodeCommunicator via shared_ptr.
    SecondaryNodeCommunicator getSecondaryNodeCommunicator() const;

    InternodeCommunicator& operator=(InternodeCommunicator const&) = delete;
    InternodeCommunicator& operator=(InternodeCommunicator&&) = delete;

    // Initialises the internode communication. Cannot be called more than once.
    // When all owning shared_ptr instances get destructed, the internode communication is terminated.
    static std::shared_ptr<InternodeCommunicator> init();

private:
    InternodeCommunicator();
    ~InternodeCommunicator();

    // Indicates if internode communication has been initialised before.
    static std::atomic_flag _initialised;

    friend struct std::default_delete<InternodeCommunicator>;
};


// Provides the primary node's side of the internode communication.
// Instances of this class may be acquired from the InternodeCommunicator class.
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
    PrimaryNodeCommunicator(std::shared_ptr<InternodeCommunicator const> internodeCommunicator);

    std::shared_ptr<InternodeCommunicator const> _internodeCommunicator;

    friend class InternodeCommunicator;
};


// Provides the secondary nodes' side of the internode communication.
// Instances of this class may be acquired from the InternodeCommunicator class.
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
    SecondaryNodeCommunicator(std::shared_ptr<InternodeCommunicator const> internodeCommunicator);

    std::shared_ptr<InternodeCommunicator const> _internodeCommunicator;

    friend class InternodeCommunicator;
};
