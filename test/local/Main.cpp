#include <iostream>

#include "../TestHelper.hpp"
#include "OutSignalWriterTest.hpp"
#include "ChannelRemappingTest.hpp"
#include "NodeAntennaInputAssignerTest.hpp"
#include "ReadInputFileTest.hpp"
#include "../../src/ReadInputFile.hpp"
#include "../TestHelper.hpp"
#include "ReadCoeDataTest.hpp"
#include <iostream>

int main(){
    std::cout << "Test random number generator seed: " << seedTestRandomEngine() << '\n' << std::endl;

    
    runTests({
        readCoeDataTest(),
        channelRemappingTest(),
        outSignalWriterTest(),
        readInputFileTest(),
        nodeAntennaInputAssignerTest()
    });
}
