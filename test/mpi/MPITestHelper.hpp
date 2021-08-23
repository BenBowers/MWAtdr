#pragma once

#include <vector>

#include "../../src/InternodeCommunication.hpp"
#include "../TestHelper.hpp"


// Runs a set of test modules. Information on the tests is output to stdout.
// Similar to runTests(), but outputs to stdout appropriately for having multiple nodes running at once.
void runMPITests(InternodeCommunicator const& internodeCommunicator, std::vector<TestModule> const& testModules);
