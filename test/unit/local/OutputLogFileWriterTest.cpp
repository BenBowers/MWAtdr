#include "OutputLogFileWriterTest.hpp"

#include "../../src/ChannelRemapping.hpp"
#include "../../src/Common.hpp"
#include "../../src/OutputLogFileWriter.hpp"
#include "../TestHelper.hpp"

#include <memory>
#include <stdexcept>

class OutputLogFileWriterTest : public StatelessTestModuleImpl {
public:
    OutputLogFileWriterTest();
};


OutputLogFileWriterTest::OutputLogFileWriterTest() : StatelessTestModuleImpl {{
    {"Write demo log file", []() {
		try {
		    AppConfig const appConfig = {"", 1000000000, 1000000008, "", "/mnt/test_output", false};
		    ChannelRemapping const channelRemapping = { 10, {{5, {5, false}}, {6, {4, true}},
                                                                {7, {3, true}}, {8, {2, true}},
													            {9, {1, true}} }};
		    ObservationProcessingResults const results = {{ {0, {true, {5, 6, 7, 8, 9}}},
		                                                    {1, {false, {7, 8}}},
													        {2, {true, {5, 6, 9}}},
													        {3, {true, {5, 6, 8, 9}}},
													        {4, {true, {5, 6, 7, 8, 9}}},
													        {5, {true, {5, 6, 7, 8, 9}}} }};
		    AntennaConfig const antennaConfig = {{ {67, 'X', false}, {67, 'Y', false}, {68, 'X', true}, {68, 'Y', true},
                                                   {71, 'X', false}, {71, 'Y', false}, {93, 'X', false}, {93, 'Y', false}},
		                                         {98, 100, 101, 103, 104, 106,
                                                  107, 108, 109, 110, 111, 112}};
		    writeLogFile(appConfig, channelRemapping, results, antennaConfig);
		}
		catch (LogWriterException const&) {
			failTest();
		}
	}},
    {"Invalid filepath", []() {
        try {
			AppConfig appConfig = {"", 1234, 1234, "", "/invalid_directory/", false};
			ChannelRemapping channelRemapping;
			ObservationProcessingResults results;
			AntennaConfig antennaConfig;
			writeLogFile(appConfig, channelRemapping, results, antennaConfig);
			failTest();
		}
		catch (LogWriterException const&) {}
	}}
}} {}


TestModule outputLogFileWriterTest() {
	return {
        "Output log file writer unit test",
		[]() { return std::make_unique<OutputLogFileWriterTest>(); }
	};
}