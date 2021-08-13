#include "TestHelper.hpp"

#include <iostream>


void runTests(std::vector<TestModule> const& testModules) {
    unsigned long long casesPassed = 0;
    unsigned long long casesFailed = 0;
    unsigned long long modulesWithFailure = 0;

    for (auto const& testModule : testModules) {
        std::cout << "Test module \"" << testModule.name << '"' << std::endl;

        bool hasFailure = false;
        for (auto const& testCase : testModule.testCases) {
            std::cout << "  Test case \"" << testCase.name << "\"...";

            bool passed = false;
            try {
                testCase.function();
                passed = true;
            }
            catch (AssertionError const& e) {
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
        std::cout << std::endl;

        if (hasFailure) {
            ++modulesWithFailure;
        }
    }

    auto const totalCases = casesPassed + casesFailed;
    std::cout << totalCases << " total test cases." << std::endl;
    std::cout << casesPassed << " test cases passed." << std::endl;
    std::cout << casesFailed << " test cases failed, across " << modulesWithFailure << " test modules" << std::endl;
}
