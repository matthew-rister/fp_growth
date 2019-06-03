#include <sstream>

#include "catch.h"
#include "frequent_pattern_tree.h"

SCENARIO("Frequent Itemset Generation", "[frequent_pattern_tree]") {

	GIVEN("A frequent pattern tree constructed from an empty itemset") {
		const FrequentPatternTree<char> fpt;

		WHEN("Frequent itemsets are extracted with a minimum support of 1") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(1);

			THEN("No frequent itemsets exist") {
				REQUIRE(frequent_itemsets.empty());
			}
		}
	}

	GIVEN("A frequent pattern tree constructed from a single itemset containing a one element") {
		const std::vector<std::unordered_set<char>> itemsets{{'A'}};
		const FrequentPatternTree<char> fpt{itemsets};

		WHEN("Frequent itemsets are extracted with a minimum support of 2") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(2);

			THEN("No frequent itemsets exist") {
				REQUIRE(frequent_itemsets.empty());
			}
		}

		WHEN("Frequent itemsets are extracted with a minimum support of 1") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(1);

			THEN("The is size of the frequent itemsets is one") {
				REQUIRE(frequent_itemsets.size() == 1);
			}

			THEN("The frequent itemsets contains the itemset {'A'}") {
				const auto itemset = std::unordered_set<char>{'A'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}
		}
	}

	GIVEN("A frequent pattern tree constructed from a single itemset containing multiple unique elements") {
		const std::vector<std::unordered_set<char>> itemsets{{'A', 'B', 'C'}};
		const FrequentPatternTree<char> fpt{itemsets};

		WHEN("Frequent itemsets are extracted with minimum support of 2") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(2);

			THEN("No frequent itemsets exist") {
				REQUIRE(frequent_itemsets.empty());
			}
		}

		WHEN("Frequent itemsets are extracted with a minimum support of 1") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(1);

			THEN("The size of the frequent itemsets equal to the number of unique combinations in the set") {
				REQUIRE(frequent_itemsets.size() == 7);
			}

			THEN("The frequent itemsets contains the itemset {'A'}") {
				const auto itemset = std::unordered_set<char>{'A'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'B'}") {
				const auto itemset = std::unordered_set<char>{'B'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'C'}") {
				const auto itemset = std::unordered_set<char>{'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'A', 'B'}") {
				const auto itemset = std::unordered_set<char>{'A', 'B'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'A', 'C'}") {
				const auto itemset = std::unordered_set<char>{'A', 'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'B', 'C'}") {
				const auto itemset = std::unordered_set<char>{'B', 'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'A', 'B', 'C'}") {
				const auto itemset = std::unordered_set<char>{'A', 'B', 'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}
		}
	}

	GIVEN("A frequent pattern tree constructed from multiple itemsets containing one element") {
		const std::vector<std::unordered_set<char>> itemsets{{'A'}, {'B'}, {'C'}};
		const FrequentPatternTree<char> fpt{itemsets};

		WHEN("Frequent itemsets are extracted with a minimum support of 2") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(2);

			THEN("No frequent itemsets exist") {
				REQUIRE(frequent_itemsets.empty());
			}
		}

		WHEN("Frequent itemsets are extracted with a minimum support of 1") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(1);

			THEN("The size of the frequent itemsets is equal to the number of unique items") {
				REQUIRE(frequent_itemsets.size() == 3);
			}

			THEN("The frequent itemsets contains the itemset {'A'}") {
				const auto itemset = std::unordered_set<char>{'A'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'B'}") {
				const auto itemset = std::unordered_set<char>{'B'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'C'}") {
				const auto itemset = std::unordered_set<char>{'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}
		}
	}

	GIVEN("A frequent pattern tree constructed from multiple itemsets containing multiple unique elements") {
		const std::vector<std::unordered_set<char>> itemsets{{'A', 'B', 'C'}, {'D', 'E'}, {'F'}};
		const FrequentPatternTree<char> fpt{itemsets};

		WHEN("Frequent itemsets are extracted with a minimum support of 2") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(2);

			THEN("No frequent itemsets exist") {
				REQUIRE(frequent_itemsets.empty());
			}
		}

		WHEN("Frequent itemsets are extracted with a minimum support of 1") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(1);

			THEN("The number of frequent itemsets is equal to the number of unique combinations in each itemset") {
				REQUIRE(frequent_itemsets.size() == 11);
			}

			THEN("The frequent itemsets contains the itemset {'A'}") {
				const auto itemset = std::unordered_set<char>{'A'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'B'}") {
				const auto itemset = std::unordered_set<char>{'B'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'C'}") {
				const auto itemset = std::unordered_set<char>{'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'A', 'B'}") {
				const auto itemset = std::unordered_set<char>{'A', 'B'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'A', 'C'}") {
				const auto itemset = std::unordered_set<char>{'A', 'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'B', 'C'}") {
				const auto itemset = std::unordered_set<char>{'B', 'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'A', 'B', 'C'}") {
				const auto itemset = std::unordered_set<char>{'A', 'B', 'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'D'}") {
				const auto itemset = std::unordered_set<char>{'E'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'E'}") {
				const auto itemset = std::unordered_set<char>{'E'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'D', 'E'}") {
				const auto itemset = std::unordered_set<char>{'D', 'E'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'F'}") {
				const auto itemset = std::unordered_set<char>{'F'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}
		}
	}

	GIVEN("A frequent pattern tree constructed from multiple itemsets with overlapping elements") {
		const std::vector<std::unordered_set<char>> itemsets{
			{'B', 'C', 'D'},
			{'B', 'C', 'D', 'E'},
			{'D', 'E'},
			{'A', 'B', 'C', 'D'},
			{'A', 'B', 'D'}
		};
		const FrequentPatternTree<char> fpt{itemsets};

		WHEN("Frequent itemsets are extracted with a minimum support of 6") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(6);

			THEN("No frequent itemsets exist") {
				REQUIRE(frequent_itemsets.empty());
			}
		}

		WHEN("Frequent itemsets are extracted with a minimum support of 5") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(5);

			THEN("The size of the frequent itemsets is equal to 1") {
				REQUIRE(frequent_itemsets.size() == 1);
			}

			THEN("The frequent itemsets contains the itemset {'D'}") {
				const auto itemset = std::unordered_set<char>{'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}
		}

		WHEN("Frequent itemsets are extracted with a minimum support of 4") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(4);

			THEN("The size of the frequent itemsets is equal to 3") {
				REQUIRE(frequent_itemsets.size() == 3);
			}

			THEN("The frequent itemsets contains the itemset {'B'}") {
				const auto itemset = std::unordered_set<char>{'B'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'D'}") {
				const auto itemset = std::unordered_set<char>{'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'B', 'D'}") {
				const auto itemset = std::unordered_set<char>{'B', 'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}
		}

		WHEN("Frequent itemsets are extracted with a minimum support of 3") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(3);

			THEN("The size of the frequent itemsets is equal to 6 ") {
				REQUIRE(frequent_itemsets.size() == 7);
			}

			THEN("The frequent itemsets contains the itemset {'B'}") {
				const auto itemset = std::unordered_set<char>{'B'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'C'}") {
				const auto itemset = std::unordered_set<char>{'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'D'}") {
				const auto itemset = std::unordered_set<char>{'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'B', 'C'}") {
				const auto itemset = std::unordered_set<char>{'B', 'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'B', 'D'}") {
				const auto itemset = std::unordered_set<char>{'B', 'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'C', 'D'}") {
				const auto itemset = std::unordered_set<char>{'C', 'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'B', 'C', 'D'}") {
				const auto itemset = std::unordered_set<char>{'B', 'C', 'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}
		}

		WHEN("Frequent itemsets are extracted with a minimum support of 2") {
			const auto frequent_itemsets = fpt.get_frequent_itemsets(2);

			THEN("The size of the frequent itemsets is equal to the 13") {
				REQUIRE(frequent_itemsets.size() == 13);
			}

			THEN("The frequent itemsets contains the itemset {'A'}") {
				const auto itemset = std::unordered_set<char>{'A'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'A', 'B'}") {
				const auto itemset = std::unordered_set<char>{'A', 'B'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'A', 'D'}") {
				const auto itemset = std::unordered_set<char>{'A', 'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'A', 'B', 'D'}") {
				const auto itemset = std::unordered_set<char>{'A', 'B', 'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'B'}") {
				const auto itemset = std::unordered_set<char>{'B'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'B', 'D'}") {
				const auto itemset = std::unordered_set<char>{'B', 'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'C'}") {
				const auto itemset = std::unordered_set<char>{'C'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'C', 'B'}") {
				const auto itemset = std::unordered_set<char>{'C', 'B'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'C', 'D'}") {
				const auto itemset = std::unordered_set<char>{'C', 'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'C', 'B', 'D'}") {
				const auto itemset = std::unordered_set<char>{'C', 'B', 'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'D'}") {
				const auto itemset = std::unordered_set<char>{'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'E'}") {
				const auto itemset = std::unordered_set<char>{'E'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}

			THEN("The frequent itemsets contains the itemset {'E', 'D'}") {
				const auto itemset = std::unordered_set<char>{'E', 'D'};
				const auto iterator = std::find(frequent_itemsets.begin(), frequent_itemsets.end(), itemset);
				REQUIRE(iterator != frequent_itemsets.end());
			}
		}
	}
}
