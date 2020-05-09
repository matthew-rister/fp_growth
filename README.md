# FP-Growth

[FP-Growth](https://github.com/matthew-rister/fp_growth/blob/master/fp_growth.pdf) is an algorithm designed to efficiently solve a subproblem of [Association Rule Mining](https://github.com/matthew-rister/fp_growth/blob/master/association_rule_mining.pdf) which requires identifying frequently occurring item combinations in a dataset.

## Usage

A frequent pattern tree can be constructed from a `std::vector<std::unordered_set<T>>` representing the itemsets to analyze for frequently occurring items.

```C++
#include "frequent_pattern_tree.hpp"

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

## Build

To build the project, you must have cmake 3 installed and a compiler that supports the C++17 language standard. You can then build from your favorite IDE or by running `cmake -G Ninja . && ninja` from the command line.

## Test

This project uses the [Catch2](https://github.com/catchorg/Catch2) testing library which is included in this repository as a single header-only file. Tests are currently configured to run as part of the main executable after building. 
