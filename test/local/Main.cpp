#include <iostream>

#include "../TestHelper.hpp"
#include "ChannelRemappingTest.hpp"
#include "MetadataFileReaderTest.hpp"


int main() {
    std::cout << "Test random number generator seed: " << seedTestRandomEngine() << '\n' << std::endl;

    runTests({
        ChannelRemappingTest{},
        MetadataFileReaderTest{}
    });
}
