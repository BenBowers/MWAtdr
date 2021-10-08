#include "CommandLineArgumentsTest.hpp"

#include "../../src/CommandLineArguments.hpp"
#include "../TestHelper.hpp"

#include <stdexcept>


class CommandLineArgumentsTest : public StatelessTestModuleImpl {
public:
    CommandLineArgumentsTest();
};


CommandLineArgumentsTest::CommandLineArgumentsTest() : StatelessTestModuleImpl{{
    {"validateInputDirectoryPath(): Non-existent directory", []() {
        try {
            validateInputDirectoryPath("/mnt/non_existant");
            failTest();
        }
        catch (std::invalid_argument const& e) {
            if ((int) ((std::string) e.what()).find("does not exist") == -1) {
                failTest();
            }
        }
    }},
    {"validateInputDirectoryPath(): Non-directory path", []() {
        try {
            validateInputDirectoryPath("/mnt/test_input/mfr/multi_voltage/1294797712.metafits");
            failTest();
        }
        catch (std::invalid_argument const& e) {
            if ((int) ((std::string) e.what()).find("not a directory") == -1) {
                failTest();
            }
        }
    }},
    {"validateInputDirectoryPath(): Empty directory", []() {
        try {
            validateInputDirectoryPath("/mnt/test_input/empty");
            failTest();
        }
        catch (std::invalid_argument const& e) {
            if ((int) ((std::string) e.what()).find("is empty") == -1) {
                failTest();
            }
        }
    }},
    {"validateInputDirectoryPath(): Valid path", []() {
        auto const actual = validateInputDirectoryPath("/mnt/test_input/mfr/multi_voltage/");
        std::string const expected = "/mnt/test_input/mfr/multi_voltage/";
        testAssert(actual.compare(expected) == 0);
    }},
    {"validateObservationID(): Invalid ID", []() {
        try {
            validateObservationID("1000000001");
            failTest();
        }
        catch (std::invalid_argument const&) {}
    }},
    {"validateObservationID(): Valid ID", []() {
        auto const actual = validateObservationID("1000000000");
        unsigned long long const expected = 1000000000;
        testAssert(actual == expected);
    }},
    {"validateSignalStartTime(): Invalid start time (not divisible by 8)", []() {
        try {
            validateSignalStartTime("1000000000", "1000000007");
            failTest();
        }
        catch (std::invalid_argument const& e) {
            if ((int) ((std::string) e.what()).find("8") == -1) {
                failTest();
            }
        }
    }},
    {"validateSignalStartTime(): Invalid start time (less than ID)", []() {
        try {
            validateSignalStartTime("1000000008", "1000000000");
            failTest();
        }
        catch (std::invalid_argument const& e) {
            if ((int) ((std::string) e.what()).find("greater than or equal") == -1) {
                failTest();
            }
        }
    }},
    {"validateSignalStartTime(): Valid start time", []() {
        auto const actual = validateSignalStartTime("1000000000", "1000000008");
        unsigned long long const expected = 1000000008;
        testAssert(actual == expected);
    }},
    {"validateInvPolyphaseFilterPath(): Non-existant path", []() {
        try {
            validateInvPolyphaseFilterPath("/mnt/non_existant");
        }
        catch (std::invalid_argument const& e) {
            if ((int) ((std::string) e.what()).find("does not exist") == -1) {
                failTest();
            }
        }
    }},
    {"validateInvPolyphaseFilterPath(): Not regular file", []() {
        try {
            validateInvPolyphaseFilterPath("/mnt/test_input");
        }
        catch (std::invalid_argument const& e) {
            if ((int) ((std::string) e.what()).find("not a regular file") == -1) {
                failTest();
            }
        }
    }},
    {"validateInvPolyphaseFilterPath(): File is empty", []() {
        try {
            validateInvPolyphaseFilterPath("/mnt/test_input/empty_filter.bin");
        }
        catch (std::invalid_argument const& e) {
            if ((int) ((std::string) e.what()).find("empty") == -1) {
                failTest();
            }
        }
    }},
    {"validateInvPolyphaseFilterPath(): Valid polyphase filter path", []() {
        auto const actual = validateInvPolyphaseFilterPath("/mnt/test_input/inverse_polyphase_filter.bin");
        std::string const expected = "/mnt/test_input/inverse_polyphase_filter.bin";
        testAssert(actual.compare(expected) == 0);
    }},
    {"validateOutputDirectoryPath(): Non-existant directory", []() {
        try {
            validateOutputDirectoryPath("/mnt/non_existant");
            failTest();
        }
        catch (std::invalid_argument const& e) {
            if ((int) ((std::string) e.what()).find("does not exist") == -1) {
                failTest();
            }
        }
    }},
    {"validateOutputDirectoryPath(): Non-directory path", []() {
        try {
            validateOutputDirectoryPath("/mnt/test_input/mfr/multi_voltage/1294797712.metafits");
            failTest();
        }
        catch (std::invalid_argument const& e) {
            if ((int) ((std::string) e.what()).find("not a directory") == -1) {
                failTest();
            }
        }
    }},
    {"validateOutputDirectoryPath(): Valid path", []() {
        auto const actual = validateOutputDirectoryPath("/mnt/test_output/");
        std::string const expected = "/mnt/test_output/";
        testAssert(actual.compare(expected) == 0);
    }},
    {"validateIgnoreErrors(): Invalid", []() {
        try {
            auto const actual = validateIgnoreErrors("yes");
            failTest();
        }
        catch (std::invalid_argument const&) {}
    }},
    {"validateIgnoreErrors(): Valid (true)", []() {
        auto const actual = validateIgnoreErrors("true");
        bool const expected = true;
        testAssert(actual == expected);
    }},
    {"validateIgnoreErrors(): Valid (false)", []() {
        auto const actual = validateIgnoreErrors("false");
        bool const expected = false;
        testAssert(actual == expected);
    }},
    {"createAppConfig(): Invalid number of arguments (less)", []() {
        try {
            createAppConfig(6, nullptr);
            failTest();
        }
        catch (std::invalid_argument const&) {}
    }},
    {"createAppConfig(): Invalid number of arguments (more)", []() {
        try {
            createAppConfig(8, nullptr);
            failTest();
        }
        catch (std::invalid_argument const&) {}
    }},
    {"createAppConfig(): Valid", []() {
        char* arguments[] = {"main", "/mnt/test_input/", "1000000000", "1000000008",
                             "/mnt/test_input/inverse_polyphase_filter.bin",
                             "/mnt/test_output", "true"};
        auto const actual = createAppConfig(7, arguments);
        AppConfig const expected = {"/mnt/test_input/", 1000000000, 1000000008,
                                    "/mnt/test_input/inverse_polyphase_filter.bin",
                                    "/mnt/test_output", true};
        testAssert(actual == expected);
    }}
}} {}


TestModule commandLineArgumentsTest() {
    return {
        "Command line arguments module unit test",
        []() { return std::make_unique<CommandLineArgumentsTest>(); }
    };
}
