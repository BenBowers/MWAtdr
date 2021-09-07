#pragma once

#include "../../../src/InternodeCommunication.hpp"
#include "../TestHelper.hpp"


// Unit test for the internode communication module (InternodeCommunication.hpp and InternodeCommunication.cpp).
// Must be run with MPI. You should test with 1 and more than 1 nodes.
class InternodeCommunicationTest : public TestModule {
public:
    InternodeCommunicationTest(PrimaryNodeCommunicator communicator);
    InternodeCommunicationTest(SecondaryNodeCommunicator communicator);
};
