#include "TestHelper.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>


StatelessTestModuleImpl::StatelessTestModuleImpl(std::vector<TestCase> testCases) :
    _testCases{testCases}
{}

std::vector<TestCase> StatelessTestModuleImpl::getTestCases() {
    return _testCases;
}


void runTests(std::vector<TestModule> const& testModules) {
    unsigned long long moduleSetupSuccesses = 0;
    unsigned long long moduleSetupFailures = 0;
    unsigned long long moduleTeardownFailures = 0;
    unsigned long long casesPassed = 0;
    unsigned long long casesFailed = 0;
    unsigned long long modulesWithFailure = 0;

    for (auto const& testModule : testModules) {
        std::cout << "Test module \"" << testModule.name << '"' << std::endl;

        std::unique_ptr<TestModule::Impl> testModuleImpl;
        try {
            testModuleImpl = testModule.factory();
        }
        catch (std::exception const& e) {
            std::cout << "  Module setup FAILED - unhandled std::exception: " << e.what() << std::endl;
        }
        catch (...) {
            // Unfortunately I don't think there is any way to get the thrown value.
            std::cout << "  Module setup FAILED - unhandled thrown value" << std::endl;
        }

        if (testModuleImpl) {
            ++moduleSetupSuccesses;

            bool hasFailure = false;
            for (auto const& testCase : testModuleImpl->getTestCases()) {
                std::cout << "  Test case \"" << testCase.name << "\"..." << std::flush;

                bool passed = false;
                try {
                    testCase.function();
                    passed = true;
                }
                catch (TestAssertionError const& e) {
                    std::cout << " FAILED - assertion failed: " << e.message << std::endl;
                }
                catch (std::exception const& e) {
                    std::cout << " FAILED - unhandled std::exception: " << e.what() << std::endl;
                }
                catch (...) {
                    // Unfortunately I don't think there is any way to get the thrown value.
                    std::cout << " FAILED - unhandled thrown value" << std::endl;
                }

                if (passed) {
                    std::cout << " PASSED" << std::endl;
                    ++casesPassed;
                }
                else {
                    ++casesFailed;
                    hasFailure = true;
                }
            }
            
            if (hasFailure) {
                ++modulesWithFailure;
            }

            bool teardownSuccess = false;
            try {
                testModuleImpl.reset();
                teardownSuccess = true;
            }
            catch (std::exception const& e) {
                std::cout << "  Module teardown FAILED - unhandled std::exception: " << e.what() << std::endl;
            }
            catch (...) {
                // Unfortunately I don't think there is any way to get the thrown value.
                std::cout << "  Module teardown FAILED - unhandled thrown value" << std::endl;
            }

            if (!teardownSuccess) {
                ++moduleTeardownFailures;
            }
        }
        else {
            ++moduleSetupFailures;
        }

        std::cout << std::endl;
    }

    auto const totalModules = moduleSetupSuccesses + moduleSetupFailures;
    std::cout << totalModules << " total test modules run." << std::endl;
    std::cout << moduleSetupSuccesses << " test modules started successfully." << std::endl;
    std::cout << moduleSetupFailures << " test modules failed to start." << std::endl;
    std::cout << moduleTeardownFailures << " test modules failed to clean up." << std::endl;
    std::cout << std::endl;

    auto const totalCases = casesPassed + casesFailed;
    std::cout << totalCases << " total test cases run." << std::endl;
    std::cout << casesPassed << " test cases passed." << std::endl;
    std::cout << casesFailed << " test cases failed, across " << modulesWithFailure << " test modules." << std::endl;
}


std::default_random_engine testRandomEngine{};


unsigned long long seedTestRandomEngine(unsigned long long seed) {
    testRandomEngine.seed(seed);
    return seed;
}