#include <sstream>
#include <vector>

#include "catch.h"
#include "frequent_pattern_tree.h"

TEST_CASE("FP-Tree construction", "[frequent_pattern_tree]") {

	SECTION("Creating an FP-tree from an empty list") {
		const frequent_pattern_tree<std::string> fpt;
		std::ostringstream oss;
		oss << fpt;
		REQUIRE(oss.str().empty());
	}

	SECTION("Creating an FP-tree from a single itemset") {
		const std::vector<std::set<std::string>> itemsets{{"A", "B", "C", "D", "E"}};
		const frequent_pattern_tree<std::string> fpt{itemsets};
		std::ostringstream oss;
		oss << fpt;
		REQUIRE(oss.str() == "A:1 B:1 C:1 D:1 E:1 ");
	}

	SECTION("Creating an FP-tree from multiple disjoint itemsets") {
		const std::vector<std::set<std::string>> itemsets{
			{"A", "B", "C", "D", "E"},
			{"F", "G", "H"},
			{"I", "J", "K", "L"}
		};
		const frequent_pattern_tree<std::string> fpt{itemsets};
		std::ostringstream oss;
		oss << fpt;
		REQUIRE(oss.str() == "A:1 B:1 C:1 D:1 E:1 F:1 G:1 H:1 I:1 J:1 K:1 L:1 ");
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
		std::ostringstream oss;
		oss << fpt;
		REQUIRE(oss.str() == "A:4 B:2 C:1 D:1 E:1 F:1 E:2 F:2 G:1 B:1 C:1 ");
	}
}
