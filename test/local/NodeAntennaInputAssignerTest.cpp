#include "NodeAntennaInputAssignerTest.hpp"

#include "../../src/NodeAntennaInputAssigner.hpp"
#include "../TestHelper.hpp"

#include <optional>
#include <stdexcept>
#include <vector>

NodeAntennaInputAssignerTest::NodeAntennaInputAssignerTest() : TestModule{"Node antenna input assigner unit test", {
	{"One node, multiple inputs", []() {
		auto const actual = assignNodeAntennaInputs(1, 256);
		std::vector<std::optional<AntennaInputRange>> const expected;
		testAssert(actual == expected);
	}},
    {"Multiple nodes, multiple inputs (more nodes)", []() {

	}},
    {"Multiple nodes, multiple inputs (more inputs)", []() {

	}},
    {"Multiple nodes, one input", []() {

	}},
    {"Invalid nodes (none)", []() {

	}},
    {"Invalid inputs (none)", []() {

	}}
}} {}