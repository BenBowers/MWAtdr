#include "../../src/InternodeCommunication.hpp"
#include "InternodeCommunicationTest.hpp"
#include "MPITestHelper.hpp"


int main() {
    auto const internodeCommunicator = InternodeCommunicator::init();

    runMPITests(*internodeCommunicator, {
        InternodeCommunicationTest{*internodeCommunicator}
    });
}
