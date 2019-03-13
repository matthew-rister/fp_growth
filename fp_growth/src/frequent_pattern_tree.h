#pragma once

#include <memory>
#include <set>
#include <vector>
#include <unordered_set>

#include "frequent_pattern_tree.h"
#include "frequent_pattern_tree_iterator.h"

template <typename T>
class frequent_pattern_tree final {

	std::shared_ptr<frequent_pattern_tree_node<T>> root_;
	std::unordered_map<T, std::vector<std::shared_ptr<frequent_pattern_tree_node<T>>>> node_links_;

public:

	explicit frequent_pattern_tree(const std::vector<std::unordered_set<T>>& itemsets = {})
		: root_{std::make_shared<frequent_pattern_tree_node<T>>()} {

		const auto frequency_counts = get_frequency_counts(itemsets);

		for (const auto& itemset : itemsets) {
			insert(itemset, frequency_counts);
		}
	}

	frequent_pattern_tree_iterator<T> begin() const {
		return frequent_pattern_tree_iterator<T>{this->root_};
	}

	frequent_pattern_tree_iterator<T> end() const {
		return frequent_pattern_tree_iterator<T>{};
	}

private:

	static std::unordered_map<T, uint32_t> get_frequency_counts(const std::vector<std::unordered_set<T>>& itemsets) {

		std::unordered_map<T, uint32_t> frequency_counts;

		for (const auto& itemset : itemsets) {
			for (const auto& item : itemset) {
				++frequency_counts[item];
			}
		}

		return frequency_counts;
	}

	std::set<T, std::function<bool(T, T)>> get_ordered_itemset(
		const std::unordered_set<T>& itemset,
		const std::unordered_map<T, uint32_t>& frequency_counts) const {

		static const auto set_comparator = [&](const T& a, const T& b) {
			return frequency_counts.at(a) == frequency_counts.at(b)
				       ? a < b
				       : frequency_counts.at(a) > frequency_counts.at(b);
		};

		return std::set<T, std::function<bool(T, T)>>{itemset.begin(), itemset.end(), set_comparator};
	}

	void insert(const std::unordered_set<T>& itemset, const std::unordered_map<T, uint32_t>& frequency_counts) {

		auto iterator = root_;

		for (const auto& item : get_ordered_itemset(itemset, frequency_counts)) {
			if (!iterator->children.count(item)) {
				iterator->children[item] = std::make_shared<frequent_pattern_tree_node<T>>(item, iterator);
				create_link(iterator->children[item]);
			} else {
				++iterator->children[item]->count;
			}
			iterator = iterator->children[item];
		}
	}

	void create_link(const std::shared_ptr<frequent_pattern_tree_node<T>>& node) {
		if (!node_links_.count(*node->value)) {
			node_links_[*node->value] = {node};
		} else {
			node_links_[*node->value].push_back(node);
		}
	}
};
