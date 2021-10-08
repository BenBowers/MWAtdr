#include <variant>

#include "InternodeCommunication.hpp"
#include "InternodeCommunicationTest.hpp"
#include "MPITestHelper.hpp"


int main() {
    auto const internodeCommunicationContext = InternodeCommunicationContext::initialise();
    auto const internodeCommunicator = internodeCommunicationContext->getCommunicator();

    std::visit([](auto const& communicator) {
        runMPITests(communicator, {
            internodeCommunicationTest(communicator)
        });
    }, internodeCommunicator);
}
