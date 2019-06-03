# FP-Growth

[FP-Growth](https://github.com/matthew-rister/fp_growth/blob/master/fp_growth/fp_growth.pdf) is an algorithm designed to efficiently solve a subproblem of [Association Rule Mining](https://github.com/matthew-rister/fp_growth/blob/master/fp_growth/association_rule_mining.pdf) which requires identifying frequently occurring item combinations in a dataset.

## Usage

A frequent pattern tree can be constructed from a `std::vector<std::unordered_set<T>>` representing the itemsets to analyze for frequently occurring items.

```C++
#include "frequent_pattern_tree.h"

using namespace fpt;

const FrequentPatternTree<char> frequent_pattern_tree{{
    {'B', 'C', 'D'},
    {'B', 'C', 'D', 'E'},
    {'D', 'E'},
    {'A', 'B', 'C', 'D'},
    {'A', 'B', 'D'}
}};
````

Once the tree has been constructed, it can be queried for frequently occurring items by passing in the minimum support which represents the minimum number of times an itemset should occur to be considered frequent.

```C++
const auto frequent_itemsets = frequent_pattern_tree.get_frequent_itemsets(4);
```