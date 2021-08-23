#include <iostream>

#include "../TestHelper.hpp"
#include "ChannelRemappingTest.hpp"
#include "SignalProcessingTest.hpp"

int main() {
    std::cout << "Test random number generator seed: " << seedTestRandomEngine() << '\n' << std::endl;

    runTests({
        ChannelRemappingTest{},
        SignalProcessingTest{}
    });
}
