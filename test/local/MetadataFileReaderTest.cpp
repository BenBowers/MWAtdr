#include "MetadataFileReaderTest.hpp"

#include "../../src/Common.hpp"
#include "../../src/MetadataFileReader.hpp"
#include "../TestHelper.hpp"

#include <filesystem>

const unsigned long long TEST_OBSERVATION_ID = 1294797712;

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
		testAssert(actual.antennaInputs.size() == 256 &&
		           actual.frequencyChannels == expected.frequencyChannels);
	}},
    {"Invalid metafits file", []() {
		try {
			auto mfr = MetadataFileReader({"/app/signals/invalid_metafits/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", ""});
			failTest();
		}
		catch (MetadataException const& e)
		{
			if (((std::string) e.what()).find("Invalid metafits") == -1) {
				failTest();
			}
		}
    }},
    {"No metafits file", []() {
		try {
			auto mfr = MetadataFileReader({"/app/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", ""});
			failTest();
		}
		catch (MetadataException const& e) {
			if (((std::string) e.what()).find("No metafits") == -1) {
				failTest();
			}
		}
    }},
	{"Invalid voltage files", []() {
	    try {
			auto mfr = MetadataFileReader({"/app/signals/invalid_voltage/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", ""});
			failTest();
		}
		catch (MetadataException const& e) {
			if (((std::string) e.what()).find("Invalid/no voltage files") == -1) {
				failTest();
			}
		}
    }},
	{"No voltage files, valid metafits", []() {
	    try {
			auto mfr = MetadataFileReader({"/app/signals/no_voltage/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", ""});
			failTest();
		}
		catch (MetadataException const& e) {
			if (((std::string) e.what()).find("Invalid/no voltage files") == -1) {
				failTest();
			}
		}
    }},
	{"Invalid directory", []() {
		try {
		    auto mfr = MetadataFileReader({"/no_directory/test", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", ""});
		    failTest();
		}
		catch (MetadataException const& e) {
			if (((std::string) e.what()).find("No metafits") == -1) {
				failTest();
			}
		}
    }}
}} {}