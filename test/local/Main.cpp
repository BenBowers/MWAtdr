#include "../TestHelper.hpp"
#include "OutSignalWriterTest.hpp"
#include "ChannelRemappingTest.hpp"

int main() {
    runTests({
        ChannelRemappingTest{},
        OutSignalWriterTest{}
    });

}