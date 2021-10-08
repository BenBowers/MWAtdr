#include <iostream>

#include "TestHelper.hpp"
#include "ChannelRemappingTest.hpp"
#include "CommandLineArgumentsTest.hpp"
#include "MetadataFileReaderTest.hpp"
#include "NodeAntennaInputAssignerTest.hpp"
#include "OutputLogFileWriterTest.hpp"
#include "OutSignalWriterTest.hpp"
#include "ReadCoeDataTest.hpp"
#include "ReadInputFileTest.hpp"
#include "SignalProcessingTest.hpp"

#include <iostream>

int main(){
    std::cout << "Test random number generator seed: " << seedTestRandomEngine() << '\n' << std::endl;

    runTests({
        commandLineArgumentsTest(),
        nodeAntennaInputAssignerTest(),
        outputLogFileWriterTest(),
        channelRemappingTest(),
        signalProcessingTest(),
        readCoeDataTest(),
        outSignalWriterTest(),
        metadataFileReaderTest(),
        readInputFileTest()
    });
}
