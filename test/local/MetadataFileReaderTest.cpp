#include "MetadataFileReaderTest.hpp"

#include "../../src/Common.hpp"
#include "../../src/MetadataFileReader.hpp"
#include "../TestHelper.hpp"

#include <filesystem>

const unsigned long long TEST_OBSERVATION_ID = 1294797712;

class MetadataFileReaderTest : public StatelessTestModuleImpl {
public:
    MetadataFileReaderTest();
};


MetadataFileReaderTest::MetadataFileReaderTest() : StatelessTestModuleImpl {{
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
		auto mfr = MetadataFileReader({"/mnt/test_input/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", "", false});
		auto actual = mfr.getAntennaConfig();
		AntennaConfig expected = {{}, {109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
		                               121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132}};
		// Populating expected.antennaInputs
		unsigned tiles[] = {51, 52, 53, 54, 55, 56, 57, 58, 71, 72, 73, 74, 75, 76, 77, 78, 101, 102, 104, 105,
		                    106, 107, 108, 111, 112, 113, 114, 115, 116, 117, 118, 121, 122, 123, 124, 125, 126,
							127, 128, 131, 132, 133, 134, 135, 136, 137, 138, 141, 142, 143, 144, 145, 146, 147,
							148, 151, 152, 153, 154, 155, 156, 157, 158, 161, 162, 163, 164, 165, 166, 167, 168,
							999, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014,
							2015, 2016, 2017, 2018, 2019, 2020, 2021, 2022, 2023, 2024, 2025, 2026, 2027, 2028, 2029,
							2030, 2031, 2032, 2033, 2034, 2035, 2036, 2037, 2038, 2039, 2040, 2041, 2042, 2043, 2044,
							2045, 2046, 2047, 2048, 2049, 2050, 2051, 2052, 2053, 2054, 2055, 2056};
		for (auto i : tiles) {
			expected.antennaInputs.push_back({i, 'X'});
			expected.antennaInputs.push_back({i, 'Y'});
		}
		testAssert(actual.antennaInputs == expected.antennaInputs &&
		           actual.frequencyChannels == expected.frequencyChannels);
	}},
    {"Invalid metafits file", []() {
		try {
			auto mfr = MetadataFileReader({"/mnt/test_input/invalid_metafits/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", "", false});
			failTest();
		}
		catch (MetadataException const& e)
		{
			if ((int) ((std::string) e.what()).find("Invalid metafits") == -1) {
				failTest();
			}
		}
    }},
    {"No metafits file", []() {
		try {
			auto mfr = MetadataFileReader({"/mnt/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", "", false});
			failTest();
		}
		catch (MetadataException const& e) {
			if ((int) ((std::string) e.what()).find("No metafits") == -1) {
				failTest();
			}
		}
    }},
	{"Invalid voltage files", []() {
	    try {
			auto mfr = MetadataFileReader({"/mnt/test_input/invalid_voltage/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", "", false});
			failTest();
		}
		catch (MetadataException const& e) {
			if ((int) ((std::string) e.what()).find("Invalid/no voltage files") == -1) {
				failTest();
			}
		}
    }},
	{"No voltage files, valid metafits", []() {
	    try {
			auto mfr = MetadataFileReader({"/mnt/test_input/no_voltage/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", "", false});
			failTest();
		}
		catch (MetadataException const& e) {
			if ((int) ((std::string) e.what()).find("Invalid/no voltage files") == -1) {
				failTest();
			}
		}
    }},
	{"Invalid directory", []() {
		try {
		    auto mfr = MetadataFileReader({"/invalid_directory/", TEST_OBSERVATION_ID, TEST_OBSERVATION_ID, "", "", false});
		    failTest();
		}
		catch (MetadataException const& e) {
			if ((int) ((std::string) e.what()).find("No metafits") == -1) {
				failTest();
			}
		}
    }}
}} {}


TestModule metadataFileReaderTest() {
	return {
		"Metadata file reader test",
		[]() { return std::make_unique<MetadataFileReaderTest>(); }
	};
}