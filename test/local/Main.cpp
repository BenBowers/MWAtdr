<<<<<<< HEAD
#include "../TestHelper.hpp"
#include "OutSignalWriterTest.hpp"

int main() {
    runTests({
        OutsignalWriterTest{}
    });
=======
#include <iostream>

#include "../TestHelper.hpp"
#include "ChannelRemappingTest.hpp"


int main() {
    std::cout << "Test random number generator seed: " << seedTestRandomEngine() << '\n' << std::endl;
>>>>>>> 6010caba9e9e33035881eb29240e62b373b4e21d

    runTests({
        ChannelRemappingTest{}
    });
}
