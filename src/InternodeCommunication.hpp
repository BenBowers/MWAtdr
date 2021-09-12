#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <thread>
#include <variant>

#include <mpi.h>


struct AppConfig;
struct AntennaConfig;
struct AntennaInputRange;
struct ChannelRemapping;
struct ObservationProcessingResults;

class PrimaryNodeCommunicator;
class SecondaryNodeCommunicator;


// Represents the communication channel between nodes (processes).
// Should be initialised and destroyed at the same point for all nodes (otherwise deadlocks and other issues may occur).
class InternodeCommunicationContext : public std::enable_shared_from_this<InternodeCommunicationContext> {
public:
    InternodeCommunicationContext(InternodeCommunicationContext const&) = delete;
    InternodeCommunicationContext(InternodeCommunicationContext&&) = delete;

    // Gets the appropriate InternodeCommunicator subclass for this node (i.e. primary or secondary).
    // The returned object shares ownership of this InternodeCommunicationContext via shared_ptr.
    std::variant<PrimaryNodeCommunicator, SecondaryNodeCommunicator> getCommunicator();

    InternodeCommunicationContext& operator=(InternodeCommunicationContext const&) = delete;
    InternodeCommunicationContext& operator=(InternodeCommunicationContext&&) = delete;

    // Initialises the internode communication. Cannot be called more than once.
    // When all owning shared_ptr instances get destructed, the internode communication is terminated.
    static std::shared_ptr<InternodeCommunicationContext> initialise();

private:
    // Handles initialisation and termination of MPI.
    class MPIContext {
    public:
        MPIContext();
        ~MPIContext();

    private:
        // Indicates if MPI has been initialised before (because we can't initialise more than once.)
        static std::atomic_flag _initialised;
    };

    // Provides communication of error statuses between nodes.
    class ErrorCommunicator {
    public:
        ErrorCommunicator();
        ~ErrorCommunicator();

        // Checks if any node has indicated an error.
        bool getErrorStatus() const;

        // Indicates to all nodes that an error has occurred.
        void indicateError();

    private:
        enum class Message : unsigned {
            ERROR_OCCURRED = 1,
            EXIT_THREAD
        };

        // MPI communicator that is used for error status messages. Needs to be a separate communicator so we don't
        // interfere with the main communication.
        MPI_Comm _communicator;
        // Indicates if an error has occurred on any node. Note that once this becomes true, it stays true.
        std::atomic_bool _errorStatus;
        // Background thread to receive error status messages (async communication on only the main thread is too hard.)
        std::thread _thread;

        // Loop that receives and processes error status messages in the background.
        void _threadFunc();

        // Creates the MPI communicator used for error status messages.
        static MPI_Comm _createCommunicator();
    };

    // The order of these is important, MPI must be initialised first and terminated last.
    MPIContext _mpiContext;
    ErrorCommunicator _errorCommunicator;

    InternodeCommunicationContext() = default;
    ~InternodeCommunicationContext();

    // Friend for access to _errorCommunicator.
    friend class InternodeCommunicator;

    // Friend so shared_ptr can destruct this class (but others can't).
    friend struct std::default_delete<InternodeCommunicationContext>;
};


// Provides methods for communicating between nodes.
class InternodeCommunicator {
public:
    InternodeCommunicator(std::shared_ptr<InternodeCommunicationContext> context);
    InternodeCommunicator(InternodeCommunicator const&) = default;
    InternodeCommunicator(InternodeCommunicator&&) = default;

    virtual ~InternodeCommunicator() = default;

    std::shared_ptr<InternodeCommunicationContext> const& getContext() const;

    // Gets the ID of this node. ID 0 represents the primary node.
    unsigned getNodeID() const;

    // Gets the total number of nodes (including primary and secondary).
    unsigned getNodeCount() const;

    // Waits for all other nodes to call this method.
    void synchronise() const;

    // Checks if any node has indicated an error.
    bool getErrorStatus() const;

    // Indicates to all nodes that an error has occurred.
    void indicateError();

    InternodeCommunicator& operator=(InternodeCommunicator const&) = default;
    InternodeCommunicator& operator=(InternodeCommunicator&&) = default;

private:
    // Share ownership of the InternodeCommunicationContext so it can't terminate while we are doing communication.
    std::shared_ptr<InternodeCommunicationContext> _context;
};


// Provides the primary node's side of the internode communication. Can only be used with node with ID 0.
// Instances of this class may be acquired from the InternodeCommunicationContext class.
// All communication methods are blocking. If the receiver/sender on the other side doesn't coorperate as expected, a
// deadlock may occur.
class PrimaryNodeCommunicator : public InternodeCommunicator {
public:
    PrimaryNodeCommunicator(std::shared_ptr<InternodeCommunicationContext> context);
    PrimaryNodeCommunicator(PrimaryNodeCommunicator const&) = default;
    PrimaryNodeCommunicator(PrimaryNodeCommunicator&&) = default;

    // Informs all the secondary nodes if the application start up was ok (i.e. valid application configuration).
    // Corresponding receive method is SecondaryNodeCommunicator::receiveAppStartupStatus().
    void sendAppStartupStatus(bool status) const;

    // Receives the status of the per-node setup from all the secondary nodes. The return value is a map from secondary
    // node IDs to their status value.
    // Corresponding send method is SecondaryNodeCommunicator::sendNodeSetupStatus().
    std::map<unsigned, bool> receiveNodeSetupStatus() const;

    // Sends the application configuration to all the secondary nodes.
    // Corresponding receive method is SecondaryNodeCommunicator::receiveAppConfig().
    void sendAppConfig(AppConfig const& appConfig) const;

    // Sends the antenna input configuration to all the secondary nodes.
    // Corresponding receive method is SecondaryNodeCommunicator::receiveAntennaConfig().
    void sendAntennaConfig(AntennaConfig const& antennaConfig) const;

    // Sends the frequency channel remapping to all the secondary nodes.
    // Corresponding receive method is SecondaryNodeCommunicator::receiveChannelRemapping().
    void sendChannelRemapping(ChannelRemapping const& channelRemapping) const;

    // Sends a secondary node's antenna input assignment to that node.
    // Corresponding receive method is SecondaryNodeCommunicator::receiveAntennaInputAssignment().
    void sendAntennaInputAssignment(unsigned node, std::optional<AntennaInputRange> const& antennaInputAssignment) const;

    // Receives the observation data processing results from all the secondary nodes. The return value is a map from
    // secondary node IDs to their processing results.
    // Corresponding send method is SecondaryNodeCommunicator::sendProcessingResults().
    std::map<unsigned, ObservationProcessingResults> receiveProcessingResults() const;

    PrimaryNodeCommunicator& operator=(PrimaryNodeCommunicator const&) = default;
    PrimaryNodeCommunicator& operator=(PrimaryNodeCommunicator&&) = default;
};


// Provides the secondary nodes' side of the internode communication. Can only be used with nodes with ID > 0.
// Instances of this class may be acquired from the InternodeCommunicationContext class.
// All communication methods are blocking. If the receiver/sender on the other side doesn't coorperate as expected, a
// deadlock may occur.
class SecondaryNodeCommunicator : public InternodeCommunicator {
public:
    SecondaryNodeCommunicator(std::shared_ptr<InternodeCommunicationContext> context);
    SecondaryNodeCommunicator(SecondaryNodeCommunicator const&) = default;
    SecondaryNodeCommunicator(SecondaryNodeCommunicator&&) = default;

    // Receives the status that says if the application startup was ok (i.e. valid application configuration).
    // Corresponding send method is PrimaryNodeCommunicator::sendAppStartupStatus().
    bool receiveAppStartupStatus() const;

    // Sends the per-node setup status for this node.
    // Corresponding receive methods is PrimaryNodeCommunicator::receiveNodeSetupStatus().
    void sendNodeSetupStatus(bool status) const;

    // Receives the application configuration.
    // Corresponding send method is PrimaryNodeCommunicator::sendAppConfig().
    AppConfig receiveAppConfig() const;

    // Receives the antenna input configuration.
    // Corresponding send method is PrimaryNodeCommunicator::sendAntennaConfig().
    AntennaConfig receiveAntennaConfig() const;

    // Receives the frequency channel remapping.
    // Corresponding send method is PrimaryNodeCommunicator::sendChannelRemapping().
    ChannelRemapping receiveChannelRemapping() const;

    // Receives the antenna input assignment for this node.
    // Corresponding send method is PrimaryNodeCommunicator::sendAntennaInputAssignment().
    std::optional<AntennaInputRange> receiveAntennaInputAssignment() const;

    // Sends the observation processing results for this node.
    // Corresponding receive method is PrimaryNodeCommunicator::receiveProcessingResults().
    void sendProcessingResults(ObservationProcessingResults const& results) const;

    SecondaryNodeCommunicator& operator=(SecondaryNodeCommunicator const&) = default;
    SecondaryNodeCommunicator& operator=(SecondaryNodeCommunicator&&) = default;
};


// Thrown when internode communication fails.
// MPI guarantees error-free communication (otherwise the program will abort), so unless the code is broken, this error
// should never occur. It's probably best to not catch it.
class InternodeCommunicationError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};
