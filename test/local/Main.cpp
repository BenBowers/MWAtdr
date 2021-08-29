#include "ChannelRemappingTest.hpp"
#include "NodeAntennaInputAssignerTest.hpp"
#include "../TestHelper.hpp"
#include "ReadCoeDataTest.hpp"
#include <iostream>
#include <vector>
#include <complex>
#include <fstream>
#include <iostream>

int main(){
    std::cout << "Test random number generator seed: " << seedTestRandomEngine() << '\n' << std::endl;

    buildTestData();

    runTests({
        ReadCoeDataTest{},
        ChannelRemappingTest{},
        NodeAntennaInputAssignerTest{}
    });

}
