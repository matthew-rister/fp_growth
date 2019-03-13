#include <sstream>

#include "catch.h"
#include "frequent_pattern_tree.h"

namespace {

	std::string to_string(const frequent_pattern_tree<std::string>& fpt) {

		std::ostringstream oss;

		for (auto iterator = fpt.begin(); iterator != fpt.end(); ++iterator) {
			const auto& [value, count] = *iterator;
			oss << value << ":" << count << " ";
		}

		return oss.str();
	}
}

TEST_CASE("FP-Tree construction", "[frequent_pattern_tree]") {

	SECTION("Creating an FP-tree from an empty list") {
		const frequent_pattern_tree<std::string> fpt;
		REQUIRE(to_string(fpt).empty());
	}

	SECTION("Creating an FP-tree from a single itemset in reverse lexicographical order") {
		const std::vector<std::unordered_set<std::string>> itemsets{{"E", "A", "C", "B", "D"}};
		const frequent_pattern_tree<std::string> fpt{itemsets};
		REQUIRE(to_string(fpt) == "A:1 B:1 C:1 D:1 E:1 ");
	}

	SECTION("Creating an FP-tree from multiple disjoint itemsets in lexicographical order") {
		const std::vector<std::unordered_set<std::string>> itemsets{
			{"E", "A", "C", "B", "D"},
			{"G", "H", "F"},
			{"K", "I", "J", "L"}
		};
		const frequent_pattern_tree<std::string> fpt{itemsets};
		REQUIRE(to_string(fpt) == "A:1 B:1 C:1 D:1 E:1 F:1 G:1 H:1 I:1 J:1 K:1 L:1 ");
	}

	SECTION("Creating an FP-tree from multiple overlapping itemsets") {
		const std::vector<std::unordered_set<std::string>> itemsets{
			{"B", "C", "D"},
			{"B", "C", "D", "E"},
			{"D", "E"},
			{"A", "B", "C", "D"},
			{"A", "B", "D"}
		};
		const frequent_pattern_tree<std::string> fpt{itemsets};
		REQUIRE(to_string(fpt) == "D:5 B:4 A:1 C:3 A:1 E:1 E:1 ");
	}

	SECTION("Creating an FP-tree with duplicate items") {
		const std::vector<std::unordered_set<std::string>> itemsets{{"A", "A", "A", "B", "B", "B", "C", "C", "C"}};
		const frequent_pattern_tree<std::string> fpt{itemsets};
		REQUIRE(to_string(fpt) == "A:1 B:1 C:1 ");
	}
}

TEST_CASE("Frequent Itemset Generation", "[frequent_pattern_tree]") {

	SECTION("this is a test") {
		const std::vector<std::unordered_set<std::string>> itemset{
			{"A", "B", "C"},
			{"A", "C", "D"},
			{"B", "D", "E"},
			{"F", "G"}
		};

		const frequent_pattern_tree<std::string> fpt{itemset};
		const auto frequent_itemsets = fpt.get_frequent_itemsets(2);

		REQUIRE(frequent_itemsets.size() == 4);
		REQUIRE(std::find(frequent_itemsets.begin(), frequent_itemsets.end(), std::unordered_set<std::string>{"A"}) != frequent_itemsets.end());
		REQUIRE(std::find(frequent_itemsets.begin(), frequent_itemsets.end(), std::unordered_set<std::string>{"B"}) != frequent_itemsets.end());
		REQUIRE(std::find(frequent_itemsets.begin(), frequent_itemsets.end(), std::unordered_set<std::string>{"C"}) != frequent_itemsets.end());
		REQUIRE(std::find(frequent_itemsets.begin(), frequent_itemsets.end(), std::unordered_set<std::string>{"D"}) != frequent_itemsets.end());
	}
}
