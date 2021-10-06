#include "../TestHelper.hpp"

#include "ChannelRemappingTest.hpp"
#include "SignalProcessingTest.hpp"
#include "MetadataFileReaderTest.hpp"
#include "NodeAntennaInputAssignerTest.hpp"
#include "ReadInputFileTest.hpp"
#include "../TestHelper.hpp"
#include "OutputLogFileWriterTest.hpp"
#include "OutSignalWriterTest.hpp"
#include "ReadCoeDataTest.hpp"

#include <iostream>

int main(){
    std::cout << "Test random number generator seed: " << seedTestRandomEngine() << '\n' << std::endl;

    runTests({
        signalProcessingTest(),
        readCoeDataTest(),
        channelRemappingTest(),
        outSignalWriterTest(),
        readInputFileTest(),
        nodeAntennaInputAssignerTest(),
        metadataFileReaderTest(),
        nodeAntennaInputAssignerTest(),
        outputLogFileWriterTest()
    });
}
