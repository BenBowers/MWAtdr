#include "../TestHelper.hpp"
#include "OutSignalWriterTest.hpp"
#include "ChannelRemappingTest.hpp"
#include "NodeAntennaInputAssignerTest.hpp"

int main() {
    runTests({
        ChannelRemappingTest{},
        OutSignalWriterTest{},
        NodeAntennaInputAssignerTest{}
    });

}