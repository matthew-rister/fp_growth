#include <sstream>
#include <vector>

#include "catch.h"
#include "frequent_pattern_tree.h"

namespace {

	std::string to_string(const frequent_pattern_tree<std::string>& fpt) {
		std::ostringstream oss;
		frequent_pattern_tree<std::string>::frequent_pattern_tree_iterator iterator{fpt};

		while (iterator) {
			const auto& [value, count] = *iterator;
			oss << value << ":" << count << " ";
			++iterator;
		}

		return oss.str();
	}
}

TEST_CASE("FP-Tree construction", "[frequent_pattern_tree]") {

	SECTION("Creating an FP-tree from an empty list") {
		const frequent_pattern_tree<std::string> fpt;
		REQUIRE(to_string(fpt).empty());
	}

	SECTION("Creating an FP-tree from a single itemset") {
		const std::vector<std::set<std::string>> itemsets{{"A", "B", "C", "D", "E"}};
		const frequent_pattern_tree<std::string> fpt{itemsets};
		REQUIRE(to_string(fpt) == "A:1 B:1 C:1 D:1 E:1 ");
	}

	SECTION("Creating an FP-tree from multiple disjoint itemsets") {
		const std::vector<std::set<std::string>> itemsets{
			{"A", "B", "C", "D", "E"},
			{"F", "G", "H"},
			{"I", "J", "K", "L"}
		};
		const frequent_pattern_tree<std::string> fpt{itemsets};
		REQUIRE(to_string(fpt) == "A:1 F:1 I:1 B:1 G:1 J:1 C:1 H:1 K:1 D:1 L:1 E:1 ");
	}

	SECTION("Creating an FP-tree from multiple overlapping itemsets") {
		const std::vector<std::set<std::string>> itemsets{
			{"A", "B", "C", "D"},
			{"A", "B", "E", "F"},
			{"A", "E", "F"},
			{"A", "E", "F", "G"},
			{"B", "C"}
		};
		const frequent_pattern_tree<std::string> fpt{itemsets};
		REQUIRE(to_string(fpt) == "A:4 B:1 B:2 E:2 C:1 C:1 E:1 F:2 D:1 F:1 G:1 ");
	}

	SECTION("Creating an FP-tree with duplicate items") {
		const std::vector<std::set<std::string>> itemsets{{"A", "A", "A", "B", "B", "B", "C", "C", "C"}};
		const frequent_pattern_tree<std::string> fpt{itemsets};
		REQUIRE(to_string(fpt) == "A:1 B:1 C:1 ");
	}
}
