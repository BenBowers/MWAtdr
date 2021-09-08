#include <iostream>

#include "TestHelper.hpp"
#include "OutSignalWriterTest.hpp"
#include "ChannelRemappingTest.hpp"
#include "NodeAntennaInputAssignerTest.hpp"
#include "ReadCoeDataTest.hpp"
#include <iostream>

int main(){
    std::cout << "Test random number generator seed: " << seedTestRandomEngine() << '\n' << std::endl;

    buildTestData();

    runTests({
        ReadCoeDataTest{},
        ChannelRemappingTest{},
        OutSignalWriterTest{},
        NodeAntennaInputAssignerTest{}
    });

}
