#include "MetadataFileReaderTest.hpp"

#include "../../src/Common.hpp"
#include "../../src/MetadataFileReader.hpp"
#include "../TestHelper.hpp"

#include <filesystem>

#define TEST_OBSERVATION_ID 1294797712

MetadataFileReaderTest::MetadataFileReaderTest() : TestModule{"Metadata file reader unit test", {
	{"Testing AntennaConfig == (true)", []() {
		AntennaConfig const lhs = {{{{4, 'X'}}}, {14, 15}};
		AntennaConfig const rhs = {{{{4, 'X'}}}, {14, 15}};
		testAssert (lhs == rhs);
	}},
    {"Testing AntennaConfig == (false)", []() {
		AntennaConfig const lhs = {{{{4, 'X'}}}, {14, 15}};
		AntennaConfig const rhs = {{{{5, 'Y'}}}, {29, 30}};
		testAssert (!(lhs == rhs));
	}},
	{"Valid metafits and one voltage file", []() {
		auto mfr = MetadataFileReader({"/app/signals/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", ""});
		auto actual = mfr.getAntennaConfig();
		AntennaConfig expected = {{}, {109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
		                               121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132}};
		// TODO: update to use non-standard tile numbers
		testAssert(actual.antennaInputs.size() == 256 &&
		           actual.frequencyChannels == expected.frequencyChannels);
	}},
    {"Invalid metafits file", []() {
		try {
			auto mfr = MetadataFileReader({"/app/signals/invalid_metafits/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", ""});
			failTest();
		}
		catch (MetadataException const&) {}
		catch (std::runtime_error const&) {}
    }},
    {"Invalid voltage file", []() {
	    try {
			auto mfr = MetadataFileReader({"/app/signals/invalid_voltage/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", ""});
			failTest();
		}
		catch (MetadataException const&) {}
		catch (std::runtime_error const&) {}
    }},
    {"No metafits file", []() {
		try {
			auto mfr = MetadataFileReader({"/app/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", ""});
			failTest();
		}
		catch (std::runtime_error const&) {}
    }},
    {"No voltage file", []() {
		try {
		    auto mfr = MetadataFileReader({"/app/no_voltage", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", ""});
		    failTest();
		}
		catch (std::runtime_error const&) {}
    }}
}} {}