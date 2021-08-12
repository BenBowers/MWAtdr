#pragma once

#include <functional>
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
struct AssertionError {
    // A description of what assertion failed.
    std::string message;
};


// Creates a TestCase instance from a name and a statement block.
// E.g.:
//   defTestCase("my test case", {
//       foo();
//       bar();
//   })
#define defTestCase(name, block) TestCase{(name), [&]() block }


// Asserts that an expression is true.
// If the expression is not true, an AssertionError is thrown.
#define testAssert(expression) if (!(expression)) { throw AssertionError{"\"" #expression "\" is false. (" __FILE__ " : " + std::to_string(__LINE__) + ")"}; }


// Runs a set of test modules. Information on the tests is output to stdout.
void runTests(std::vector<TestModule> const& testModules);
