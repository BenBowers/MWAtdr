#include "../TestHelper.hpp"

#include "ChannelRemappingTest.hpp"
#include "MetadataFileReaderTest.hpp"
#include "NodeAntennaInputAssignerTest.hpp"
#include "OutputLogFileWriterTest.hpp"
#include "OutSignalWriterTest.hpp"
#include "ReadCoeDataTest.hpp"

#include <iostream>

int main(){
    std::cout << "Test random number generator seed: " << seedTestRandomEngine() << '\n' << std::endl;

    runTests({
        readCoeDataTest(),
        channelRemappingTest(),
        outSignalWriterTest(),
        metadataFileReaderTest(),
        nodeAntennaInputAssignerTest(),
        outputLogFileWriterTest()
    });
}
