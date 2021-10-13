#include "MPITestHelper.hpp"

#include "../TestHelper.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>


void runMPITests(InternodeCommunicator const& internodeCommunicator, std::vector<TestModule> const& testModules) {
    // Note that technically the ordering of stdout from MPI processes is not unspecified.
    // However the output we do here happens to not get mangled by OpenMPI, so we'll just go with this (even though
    // relying on such behaviour is bad practice). The alternative is using MPI I/O to properly synchronise the output,
    // which I think is unnecessary just for test code.

    auto const nodeID = internodeCommunicator.getNodeID();
    
    unsigned long long moduleSetupSuccesses = 0;
    unsigned long long moduleSetupFailures = 0;
    unsigned long long casesPassed = 0;
    unsigned long long casesFailed = 0;

    auto const nodePrefix = std::string{"Node "} + std::to_string(nodeID) + " | ";

    for (auto const& testModule : testModules) {
        internodeCommunicator.synchronise();

        auto const modulePrefix = nodePrefix + '"' + testModule.name + "\" | ";

        std::unique_ptr<TestModule::Impl> testModuleImpl;
        try {
            testModuleImpl = testModule.factory();
        }
        catch (std::exception const& e) {
            std::cout << modulePrefix << "Module setup FAILED - unhandled std::exception: " << e.what() << std::endl;
        }
        catch (...) {
            // Unfortunately I don't think there is any way to get the thrown value.
            std::cout << modulePrefix << "Module setup FAILED - unhandled thrown value" << std::endl;
        }

        if (testModuleImpl) {
            ++moduleSetupSuccesses;

            for (auto const& testCase : testModuleImpl->getTestCases()) {
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

            try {
                testModuleImpl.reset();
            }
            catch (std::exception const& e) {
                std::cout << modulePrefix << "Module teardown FAILED - unhandled std::exception: " << e.what() << std::endl;
            }
            catch (...) {
                // Unfortunately I don't think there is any way to get the thrown value.
                std::cout << modulePrefix << "Module teardown FAILED - unhandled thrown value" << std::endl;
            }
        }
        else {
            ++moduleSetupFailures;
        }
    }

    internodeCommunicator.synchronise();
    // Really dodgy sleep just to wait for all output to stdout from all nodes to be written.
    std::this_thread::sleep_for(std::chrono::milliseconds{250});

    // TODO
    std::cout << nodePrefix << moduleSetupSuccesses << " test modules started successfully, "
        << moduleSetupFailures << " test modules failed to start, "
        << casesPassed << " test cases passed, " << casesFailed << " test cases failed." << std::endl;
}
