#include <iostream>

#include "../TestHelper.hpp"
#include "OutSignalWriterTest.hpp"
#include "ChannelRemappingTest.hpp"
#include "NodeAntennaInputAssignerTest.hpp"

int main() {
    std::cout << "Test random number generator seed: " << seedTestRandomEngine() << '\n' << std::endl;
    
    runTests({
        ChannelRemappingTest{},
        OutSignalWriterTest{},
        NodeAntennaInputAssignerTest{}
    });

}