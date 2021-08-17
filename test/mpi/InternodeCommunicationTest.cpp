#include "InternodeCommunicationTest.hpp"

#include <map>
#include <optional>
#include <stdexcept>

#include "../../src/Common.hpp"
#include "../../src/InternodeCommunication.hpp"
#include "../../src/NodeAntennaInputAssigner.hpp"
#include "../TestHelper.hpp"


// Returns a TestModule for the primary node's side of the unit test.
static TestModule primaryNodeTestModule(PrimaryNodeCommunicator const& communicator) {
    return {
        "Internode communication module unit test (primary node)",
        {   // Very important that the order of test cases matches those in secondaryNodeTestModule().
            {"Repeat internode communication initialisation", []() {
                try {
                    InternodeCommunicator::init();
                    failTest();
                }
                catch (std::logic_error const&) {}
            }},
            {"Instantiating wrong communicator class", [communicator]() {
                try {
                    communicator.getBaseCommunicator()->getSecondaryNodeCommunicator();
                    failTest();
                }
                catch (std::logic_error const&) {}
            }},
            {"sendAppStartupStatus(false)", [communicator]() {
                communicator.sendAppStartupStatus(false);
            }},
            {"sendAppStartupStatus(true)", [communicator]() {
                communicator.sendAppStartupStatus(true);
            }},
            {"receiveNodeSetupStatus()", [communicator]() {
                auto const actual = communicator.receiveNodeSetupStatus();
                std::map<unsigned, bool> expected;
                auto const nodeCount = communicator.getBaseCommunicator()->getNodeCount();
                for (unsigned node = 1; node < nodeCount; ++node) {
                    expected.emplace(node, node % 2 == 0);
                }
                testAssert(actual == expected);
            }},
            {"sendAppConfig()", [communicator]() {
                AppConfig const appConfig{
                    "/group/mwavcs/myObservation",
                    1000000000,
                    1000000016,
                    "/group/mwavcs/inversePolyphaseFilter.bin",
                    "/group/mwavcs/myProcessedObservation"
                };
                communicator.sendAppConfig(appConfig);
            }},
            {"sendAntennaInputAssignment()", [communicator]() {
                auto const nodeCount = communicator.getBaseCommunicator()->getNodeCount();
                for (unsigned node = 1; node < nodeCount; ++node) {
                    std::optional<AntennaInputRange> assignment;
                    if (node <= nodeCount / 2) {
                        assignment = {7 * node, 7 * node + 6};
                    }
                    communicator.sendAntennaInputAssignment(node, assignment);
                }
            }},
            {"sendAntennaInputAssignment() to primary node", [communicator]() {
                try {
                    communicator.sendAntennaInputAssignment(0, std::nullopt);
                    failTest();
                }
                catch (std::invalid_argument const&) {}
            }}
        }
    };
}

// Returns a TestModule for the secondary nodes' side of the unit test.
static TestModule secondaryNodeTestModule(SecondaryNodeCommunicator const& communicator) {
    return {
        "Internode communication module unit test (secondary node)",
        {   // Very important that the order of test cases matches those in primaryNodeTestModule().
            {"Repeat internode communication initialisation", []() {
                try {
                    InternodeCommunicator::init();
                    failTest();
                }
                catch (std::logic_error const&) {}
            }},
            {"Instantiating wrong communicator class", [communicator]() {
                try {
                    communicator.getBaseCommunicator()->getPrimaryNodeCommunicator();
                    failTest();
                }
                catch (std::logic_error const&) {}
            }},
            {"receiveAppStartupStatus(false)", [communicator]() {
                auto const actual = communicator.receiveAppStartupStatus();
                auto const expected = false;
                testAssert(actual == expected);
            }},
            {"receiveAppStartupStatus(true)", [communicator]() {
                auto const actual = communicator.receiveAppStartupStatus();
                auto const expected = true;
                testAssert(actual == expected);
            }},
            {"sendNodeSetupStatus()", [communicator]() {
                auto const nodeID = communicator.getBaseCommunicator()->getNodeID();
                communicator.sendNodeSetupStatus(nodeID % 2 == 0);
            }},
            {"receiveAppConfig()", [communicator]() {
                auto const actual = communicator.receiveAppConfig();
                AppConfig const expected{
                    "/group/mwavcs/myObservation",
                    1000000000,
                    1000000016,
                    "/group/mwavcs/inversePolyphaseFilter.bin",
                    "/group/mwavcs/myProcessedObservation"
                };
                testAssert(actual.inputDirectoryPath == expected.inputDirectoryPath);
                testAssert(actual.observationID == expected.observationID);
                testAssert(actual.signalStartTime == expected.signalStartTime);
                testAssert(actual.invPolyphaseFilterPath == expected.invPolyphaseFilterPath);
                testAssert(actual.outputDirectoryPath == expected.outputDirectoryPath);
            }},
            {"receiveAntennaInputAssignment()", [communicator]() {
                auto const nodeID = communicator.getBaseCommunicator()->getNodeID();
                auto const nodeCount = communicator.getBaseCommunicator()->getNodeCount();
                auto const actual = communicator.receiveAntennaInputAssignment();
                std::optional<AntennaInputRange> expected;
                if (nodeID <= nodeCount / 2) {
                    expected = {7 * nodeID, 7 * nodeID + 6};
                }
                testAssert(actual.has_value() == expected.has_value());
                if (actual && expected) {
                    testAssert(actual.value().begin == expected.value().begin);
                    testAssert(actual.value().end == expected.value().end);
                }
            }}
        }
    };
}


// Returns a TestModule appropriate for the type of MPI node we are, i.e. primary or secondary.
static TestModule dispatchTestModule(InternodeCommunicator const& internodeCommunicator) {
    if (internodeCommunicator.getNodeID() == 0) {
        return primaryNodeTestModule(internodeCommunicator.getPrimaryNodeCommunicator());
    }
    else {
        return secondaryNodeTestModule(internodeCommunicator.getSecondaryNodeCommunicator());
    }
}


InternodeCommunicationTest::InternodeCommunicationTest(InternodeCommunicator const& internodeCommunicator) :
    TestModule{dispatchTestModule(internodeCommunicator)}
{}
