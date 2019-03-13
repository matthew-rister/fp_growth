#pragma once

#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <vector>

template <typename T>
class frequent_pattern_tree final {

	struct frequent_pattern_tree_node final {

		std::unique_ptr<T> value;
		uint32_t support = 1;
		std::shared_ptr<frequent_pattern_tree_node> parent;
		std::map<T, std::shared_ptr<frequent_pattern_tree_node>> children;

		frequent_pattern_tree_node() = default;

		frequent_pattern_tree_node(const T& value, std::shared_ptr<frequent_pattern_tree_node> parent)
			: value{std::make_unique<T>(value)},
			  parent{std::move(parent)} {}
	};

	std::shared_ptr<frequent_pattern_tree_node> root_;
	std::unordered_set<T> items_;
	std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>> item_nodes_;

public:

	explicit frequent_pattern_tree(const std::vector<std::unordered_set<T>>& itemsets = {})
		: root_{std::make_shared<frequent_pattern_tree_node>()} {

		const auto frequency_counts = get_frequency_counts(itemsets);

		for (const auto& itemset : itemsets) {
			insert(itemset, frequency_counts);
		}
	}

	std::vector<std::unordered_set<T>> get_frequent_itemsets(const uint32_t min_support) const {

		std::vector<std::unordered_set<T>> frequent_itemsets;

		for (const auto& item : items_) {

			const auto [range_begin, range_end] = item_nodes_.equal_range(item);
			const auto support = std::reduce(range_begin, range_end, 0u, [](const auto sum, const auto& map_entry) {
	             return sum + map_entry.second->support;
	         });

			if (support >= min_support) {
				frequent_itemsets.push_back({ item });
			}
		}

		return frequent_itemsets;
	}

	//std::unordered_map<T, std::vector<std::shared_ptr<frequent_pattern_tree_node<T>>>> get_conditional_item_nodes(
	//	const T& target,
	//	const std::unordered_map<T, std::vector<std::shared_ptr<frequent_pattern_tree_node<T>>>>& item_nodes) {

	//	std::unordered_map<T, std::vector<std::shared_ptr<frequent_pattern_tree_node<T>>>> conditional_item_nodes;

	//	if (item_nodes.contains(target)) {
	//		for (const auto& node : item_nodes.at(target)) {
	//			for (const auto iterator = node->parent; iterator->parent; iterator = iterator->parent) {
	//				conditional_item_nodes[*iterator->value].push_back(iterator);
	//			}
	//		}
	//	}

	//	return conditional_item_nodes;
	//}

	//void get_frequent_itemsets(
	//	const std::set<T>& frequent_itemset,
	//	const T& target,
	//	const std::unordered_map<T, std::vector<std::shared_ptr<frequent_pattern_tree_node<T>>>>& node_list,
	//	const uint32_t minimum_support) {

	//	if (!node_list.contains(target)) return {};

	//	std::vector<std::set<T>> frequent_itemsets{frequent_itemset};
	//	const auto conditional_node_list = get_conditional_item_nodes(target, node_list);

	//	for (const auto& [item, nodes] : conditional_node_list) {
	//		if (nodes.size() >= minimum_support) {
	//			const std::set<T> clone{frequent_itemset};
	//			clone.insert(item);
	//			frequent_itemsets.push_back(clone);

	//			for (const auto& [candidate, _] : conditional_node_list) {
	//				if (candidate != item) {
	//					const auto sub_frequent_itemsets = get_frequent_itemsets(clone, candidate, conditional_node_list, minimum_support);
	//				}
	//			}
	//		}
	//	}
	//}

	class frequent_pattern_tree_iterator final {

		std::stack<std::shared_ptr<frequent_pattern_tree_node>> item_stack_;

	public:

		frequent_pattern_tree_iterator() = default;

		explicit frequent_pattern_tree_iterator(const std::shared_ptr<frequent_pattern_tree_node>& node) {
			for (auto iterator = node->children.rbegin(); iterator != node->children.rend(); ++iterator) {
				item_stack_.push(iterator->second);
			}
		}

		operator bool() const {
			return !item_stack_.empty();
		}

		std::pair<T, uint32_t> operator*() const {
			return std::make_pair(*item_stack_.top()->value, item_stack_.top()->support);
		}

		void operator++() {

			const auto current = item_stack_.top();
			item_stack_.pop();

			for (auto iterator = current->children.rbegin(); iterator != current->children.rend(); ++iterator) {
				item_stack_.push(iterator->second);
			}
		}

		bool operator==(const frequent_pattern_tree_iterator& iterator) {
			return item_stack_ == iterator.item_stack_;
		}

		bool operator!=(const frequent_pattern_tree_iterator& iterator) {
			return !(*this == iterator);
		}
	};

	frequent_pattern_tree_iterator begin() const {
		return frequent_pattern_tree_iterator{this->root_};
	}

	frequent_pattern_tree_iterator end() const {
		return frequent_pattern_tree_iterator{};
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

	static std::set<T, std::function<bool(T, T)>> get_ordered_itemset(
		const std::unordered_set<T>& itemset,
		const std::unordered_map<T, uint32_t>& frequency_counts) {

		const auto order_by_frequency = [&](const T& a, const T& b) {
			return frequency_counts.at(a) == frequency_counts.at(b)
				       ? a < b
				       : frequency_counts.at(a) > frequency_counts.at(b);
		};

		return std::set<T, std::function<bool(T, T)>>{itemset.begin(), itemset.end(), order_by_frequency};
	}

	void insert(const std::unordered_set<T>& itemset, const std::unordered_map<T, uint32_t>& frequency_counts) {

		auto iterator = root_;

		for (const auto& item : get_ordered_itemset(itemset, frequency_counts)) {
			if (!iterator->children.count(item)) {
				iterator->children[item] = std::make_shared<frequent_pattern_tree_node>(item, iterator);
				item_nodes_.insert(std::make_pair(item, iterator->children[item]));
			} else {
				++iterator->children[item]->support;
			}
			items_.insert(item);
			iterator = iterator->children[item];
		}
	}
};
