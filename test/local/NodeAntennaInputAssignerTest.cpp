#include "NodeAntennaInputAssignerTest.hpp"

#include "../../src/NodeAntennaInputAssigner.hpp"
#include "../TestHelper.hpp"

#include <optional>
#include <stdexcept>
#include <vector>

NodeAntennaInputAssignerTest::NodeAntennaInputAssignerTest() : TestModule{"Node antenna input assigner unit test", {
    {"compareRanges(): Both null", []() {
		std::vector<std::optional<AntennaInputRange>> lhs;
		std::vector<std::optional<AntennaInputRange>> rhs;
		// Populating lhs vector
		lhs.push_back(std::nullopt);
		// Populating rhs vector
		rhs.push_back(std::nullopt);
		testAssert(compareRanges(lhs, rhs))
	}},
    {"compareRanges(): One null (lhs)", []() {
		std::vector<std::optional<AntennaInputRange>> lhs;
		std::vector<std::optional<AntennaInputRange>> rhs;
		AntennaInputRange temp = {0, 0};
		// Populating lhs vector
		lhs.push_back(std::nullopt);
		// Populating rhs vector
		rhs.push_back(temp);
		testAssert(!compareRanges(lhs, rhs))
	}},
    {"compareRanges(): One null (rhs)", []() {
		std::vector<std::optional<AntennaInputRange>> lhs;
		std::vector<std::optional<AntennaInputRange>> rhs;
		AntennaInputRange temp = {0, 0};
		// Populating lhs vector
		lhs.push_back(temp);
		// Populating rhs vector
		rhs.push_back(std::nullopt);
		testAssert(!compareRanges(lhs, rhs))
	}},
    {"compareRanges(): Different size vectors", []() {
		std::vector<std::optional<AntennaInputRange>> lhs;
		std::vector<std::optional<AntennaInputRange>> rhs;
		AntennaInputRange temp[] = {{0, 0}, {1, 1}};
		// Populating lhs vector
		lhs.push_back(temp[0]);
		// Populating rhs vector
		rhs.push_back(temp[0]);
		rhs.push_back(temp[1]);
		testAssert(!compareRanges(lhs, rhs))
	}},
    {"compareRanges(): Equal entry values", []() {
		std::vector<std::optional<AntennaInputRange>> lhs;
		std::vector<std::optional<AntennaInputRange>> rhs;
		AntennaInputRange temp[] = {{0, 0}, {1, 1}};
		// Populating lhs vector
		lhs.push_back(temp[0]);
		lhs.push_back(temp[1]);
		// Populating rhs vector
		rhs.push_back(temp[0]);
		rhs.push_back(temp[1]);
		testAssert(compareRanges(lhs, rhs))
	}},
    {"compareRanges(): Different entry values", []() {
		std::vector<std::optional<AntennaInputRange>> lhs;
		std::vector<std::optional<AntennaInputRange>> rhs;
		AntennaInputRange temp[] = {{0, 0}, {1, 1}, {1, 2}};
		// Populating lhs vector
		lhs.push_back(temp[0]);
		lhs.push_back(temp[1]);
		// Populating rhs vector
		rhs.push_back(temp[0]);
		rhs.push_back(temp[2]);
		testAssert(!compareRanges(lhs, rhs))
	}},
	{"One node, multiple inputs", []() {
		auto const actual = assignNodeAntennaInputs(1, 256);
		std::vector<std::optional<AntennaInputRange>> expected;
		// Populating expected vector
		AntennaInputRange range = {0, 255};
		expected.push_back(range);
		testAssert(compareRanges(actual, expected));
	}},
    {"Multiple nodes, multiple inputs (more nodes)", []() {
        auto const actual = assignNodeAntennaInputs(4, 3);
		std::vector<std::optional<AntennaInputRange>> expected;
		// Populating expected vector
		AntennaInputRange ranges[] = {{0, 0}, {1, 1}, {2, 2}};
		for (AntennaInputRange i : ranges) {
			expected.push_back(i);
		}
		expected.push_back(std::nullopt);
		testAssert(compareRanges(actual, expected));
	}},
    {"Multiple nodes, multiple inputs (more inputs)", []() {
        auto const actual = assignNodeAntennaInputs(3, 5);
		std::vector<std::optional<AntennaInputRange>> expected;
        // Populating expected vector
		AntennaInputRange ranges[] = {{0, 1}, {2, 3}, {4, 4}};
		for (AntennaInputRange i : ranges) {
			expected.push_back(i);
		}
		testAssert(compareRanges(actual, expected));
	}},
    {"Multiple nodes, one input", []() {
        auto const actual = assignNodeAntennaInputs(4, 1);
		std::vector<std::optional<AntennaInputRange>> expected;
        // Populating expected vector
		AntennaInputRange range = {0, 0};
		expected.push_back(range);
		for (int i = 0; i < 3; i++) {
			expected.push_back(std::nullopt);
		}
		testAssert(compareRanges(actual, expected));
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

bool compareRanges(std::vector<std::optional<AntennaInputRange>> const& lhs,
                   std::vector<std::optional<AntennaInputRange>> const& rhs) {
	// Returns false if the vectors are of different size
    if (lhs.size() != rhs.size()) {
		return false;
	}

	for (unsigned i = 0; i < lhs.size(); i++) {
		// If the lhs vector contains a value 
		if (lhs.at(i).has_value()) {
			// If the rhs vector doesn't contain a value
			if (!rhs.at(i).has_value()) {
				return false;
			}
			// If any of the entries in the vectors aren't equal
			else if (lhs.at(i).value() != rhs.at(i).value()) {
				return false;
			}
		}
		else {
			// Returns false if
			if (rhs.at(i).has_value()) {
				return false;
			}
		}
	}
	return true;
}