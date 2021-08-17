#pragma once

#include "../../src/InternodeCommunication.hpp"
#include "../TestHelper.hpp"


// Unit test for the internode communication module (InternodeCommunication.hpp and InternodeCommunication.cpp).
// Must be run with MPI and with multiple nodes.
class InternodeCommunicationTest : public TestModule {
public:
    InternodeCommunicationTest(InternodeCommunicator const& internodeCommunicator);
};
