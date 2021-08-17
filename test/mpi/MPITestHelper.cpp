#include "MPITestHelper.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>


void runMPITests(InternodeCommunicator const& internodeCommunicator, std::vector<TestModule> const& testModules) {
    // Note that technically the ordering of stdout from MPI processes is not unspecified.
    // However the output we do here happens to not get mangled by OpenMPI, so we'll just go with this (even though
    // relying on such behaviour is bad practice). The alternative is using MPI I/O to properly synchronise the output,
    // which I think is unnecessary just for test code.

    auto const nodeID = internodeCommunicator.getNodeID();
    
    unsigned long long casesPassed = 0;
    unsigned long long casesFailed = 0;

    auto const nodePrefix = std::string{"Node "} + std::to_string(nodeID) + " | ";

    for (auto const& testModule : testModules) {
        internodeCommunicator.sync();

        auto const modulePrefix = nodePrefix + '"' + testModule.name + "\" | ";

        for (auto const& testCase : testModule.testCases) {
            auto const casePrefix = modulePrefix + '"' + testCase.name + "\"... ";

            bool passed = false;
            try {
                testCase.function();
                passed = true;
            }
            catch (TestAssertionError const& e) {
                std::cout << casePrefix << "FAILED - assertion failed: " << e.message << std::endl;
            }
            catch (std::exception const& e) {
                std::cout << casePrefix << "FAILED - unhandled std::exception: " << e.what() << std::endl;
            }
            catch (...) {
                // Unfortunately I don't think there is any way to get the thrown value.
                std::cout << casePrefix << "FAILED - unhandled thrown value" << std::endl;
            }

            if (passed) {
                std::cout << casePrefix << "PASSED" << std::endl;
                ++casesPassed;
            }
            else {
                ++casesFailed;
            }
        }
    }

    internodeCommunicator.sync();
    // Really dodgy sleep just to wait for all output to stdout from all nodes to be written.
    std::this_thread::sleep_for(std::chrono::milliseconds{250});

    auto const totalCases = casesPassed + casesFailed;
    std::cout << nodePrefix << casesPassed << " total test cases passed, " << casesFailed << " total test cases failed."
        << std::endl;
}
