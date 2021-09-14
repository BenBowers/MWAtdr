#pragma once

#include <functional>
#include <random>
#include <string>
#include <vector>


// A single test case. Typically should test one discrete piece of functionality.
struct TestCase {
    // The name of the test case, e.g. what functionality is being tested.
    std::string name;

    // A function that performs the test.
    std::function<void()> function;
};


// A test of an entire unit/module. Contains multiple test cases.
// To implement a test module, one can create an instance of this class or inherit from it.
struct TestModule {
    // The name of the test, e.g. what unit/module is being tested.
    std::string name;

    // The associated test cases.
    std::vector<TestCase> testCases;
};


// Thrown when a test assertion does not pass.
struct TestAssertionError {
    // A description of what assertion failed.
    std::string message;
};


// Asserts that an expression is true.
// If the expression is not true, a TestAssertionError is thrown.
#define testAssert(expression) if (!(expression)) { throw TestAssertionError{"\"" #expression "\" is false. (" __FILE__ " : " + std::to_string(__LINE__) + ")"}; }

// Throws a TestAssertionError. Can be used to fail a test if execution reaches the call.
#define failTest() throw TestAssertionError{"Didn't expect this execution path. (" __FILE__ " : " + std::to_string(__LINE__) + ")"}


// Runs a set of test modules. Information on the tests is output to stdout.
void runTests(std::vector<TestModule> const& testModules);


// Pseudorandom number generator for testing.
extern std::default_random_engine testRandomEngine;


// Seeds testRandomEngine from a given value or from a true random number source.
unsigned long long seedTestRandomEngine(unsigned long long seed = std::random_device{}());
