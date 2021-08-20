#pragma once

#include "../TestHelper.hpp"

struct AntennaInputRange;

class NodeAntennaInputAssignerTest : public TestModule {
	public:
	    NodeAntennaInputAssignerTest();
};

bool compareRanges(std::vector<std::optional<AntennaInputRange>> const& lhs,
                   std::vector<std::optional<AntennaInputRange>> const& rhs);