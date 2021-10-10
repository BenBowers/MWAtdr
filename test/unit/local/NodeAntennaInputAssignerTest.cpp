#include "NodeAntennaInputAssignerTest.hpp"

#include "NodeAntennaInputAssigner.hpp"
#include "TestHelper.hpp"

#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>


class NodeAntennaInputAssignerTest : public StatelessTestModuleImpl {
public:
	NodeAntennaInputAssignerTest();
};


NodeAntennaInputAssignerTest::NodeAntennaInputAssignerTest() : StatelessTestModuleImpl{{
    {"Comparing vectors: Equal vectors (with values and null)", []() {
		std::vector<std::optional<AntennaInputRange>> const lhs{{{0, 1}}, {{2, 3}}, {}};
		std::vector<std::optional<AntennaInputRange>> const rhs{{{0, 1}}, {{2, 3}}, {}};
		testAssert(lhs == rhs);
	}},
    {"Comparing vectors: Different vectors", []() {
		std::vector<std::optional<AntennaInputRange>> const lhs{{{1, 4}}, {{3, 6}}, {}};
		std::vector<std::optional<AntennaInputRange>> const rhs{{{0, 1}}, {}, {}};
		testAssert(!(lhs == rhs));
	}},
    {"Comparing vectors: Different size vectors", []() {
		std::vector<std::optional<AntennaInputRange>> const lhs{{{0, 1}}};
		std::vector<std::optional<AntennaInputRange>> const rhs{{{0, 1}}, {{2, 3}}};
		testAssert(!(lhs == rhs));
	}},
	{"One node, multiple inputs", []() {
		auto const actual = assignNodeAntennaInputs(1, 256);
		std::vector<std::optional<AntennaInputRange>> const expected{{{0, 255}}};
		testAssert(actual == expected);
	}},
    {"Multiple nodes, multiple inputs (more nodes)", []() {
        auto const actual = assignNodeAntennaInputs(4, 3);
		std::vector<std::optional<AntennaInputRange>> const expected{{{0, 0}}, {{1, 1}}, {{2, 2}}, {}};
		testAssert(actual == expected);
	}},
    {"Multiple nodes, multiple inputs (more inputs)", []() {
        auto const actual = assignNodeAntennaInputs(3, 5);
		std::vector<std::optional<AntennaInputRange>> const expected{{{0, 1}}, {{2, 3}}, {{4, 4}}};
		testAssert(actual == expected);
	}},
    {"Multiple nodes, one input", []() {
        auto const actual = assignNodeAntennaInputs(4, 1);
		std::vector<std::optional<AntennaInputRange>> const expected{{{0, 0}}, {}, {}, {}};
		testAssert(actual == expected);
	}},
    {"Real world test (multiple nodes, 256 inputs)", []() {
        auto const actual = assignNodeAntennaInputs(13, 256);
		std::vector<std::optional<AntennaInputRange>> const expected{{{0, 19}}, {{20,39}}, {{40, 59}},
		                                                             {{60, 79}}, {{80, 99}}, {{100, 119}},
																	 {{120, 139}}, {{140, 159}}, {{160, 179}},
																	 {{180, 198}}, {{199, 217}}, {{218, 236}},
																	 {{237, 255}}};
		testAssert(actual == expected);
	}},
    {"Invalid nodes (none)", []() {
		try {
			assignNodeAntennaInputs(0, 5);
		    failTest();
		}
		catch (std::invalid_argument const&) {}
	}},
    {"Invalid inputs (none)", []() {
		try {
			assignNodeAntennaInputs(2, 0);
			failTest();
		}
		catch (std::invalid_argument const&) {}
	}}
}} {}


TestModule nodeAntennaInputAssignerTest() {
	return {
		"Node antenna input assigner unit test",
		[]() { return std::make_unique<NodeAntennaInputAssignerTest>(); }
	};
}
