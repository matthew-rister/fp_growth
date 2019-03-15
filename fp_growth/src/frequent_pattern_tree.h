#pragma once

#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

template <typename T>
class frequent_pattern_tree final {

	struct frequent_pattern_tree_node final {

		std::optional<T> value;
		std::shared_ptr<frequent_pattern_tree_node> parent;
		std::map<T, std::shared_ptr<frequent_pattern_tree_node>> children;
		uint32_t support = 1;

		explicit frequent_pattern_tree_node(
			std::optional<T> value = {},
			std::shared_ptr<frequent_pattern_tree_node> parent = nullptr)
			: value{std::move(value)},
			  parent{std::move(parent)} {}

		friend bool operator==(const frequent_pattern_tree_node& lhs, const frequent_pattern_tree_node& rhs) {
			return &lhs == &rhs;
		}
	};

	std::shared_ptr<frequent_pattern_tree_node> root_ = std::make_shared<frequent_pattern_tree_node>();
	std::unordered_map<T, uint32_t> item_support_;
	std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>> item_nodes_;

public:

	explicit frequent_pattern_tree(const std::vector<std::unordered_set<T>>& itemsets = {})
		: item_support_{get_item_support(itemsets)} {

		for (const auto& itemset : itemsets) {
			insert(itemset);
		}
	}

	std::vector<std::unordered_set<T>> get_frequent_itemsets(const uint32_t minimum_support) const {

		std::vector<std::unordered_set<T>> frequent_itemsets;
		const auto frequent_items = get_frequent_items(minimum_support);

		for (auto current = frequent_items.begin(); current != frequent_items.end(); ++current) {
			const auto current_itemset = std::set<T>{*current};
			const auto conditional_item_nodes = get_conditional_item_nodes(*current, item_nodes_, minimum_support);
			frequent_itemsets.push_back(current_itemset);

			for (auto next = std::next(current); next != frequent_items.end(); ++next) {
				const auto next_itemsets = get_frequent_itemsets(current_itemset, *next, conditional_item_nodes,
				                                                 minimum_support);
				frequent_itemsets.insert(frequent_itemsets.end(), next_itemsets.begin(), next_itemsets.end());
			}
		}

		return frequent_itemsets;
	}

private:

	static std::unordered_map<T, uint32_t> get_item_support(const std::vector<std::unordered_set<T>>& itemsets) {

		std::unordered_map<T, uint32_t> item_support;

		for (const auto& itemset : itemsets) {
			for (const auto& item : itemset) {
				++item_support[item];
			}
		}

		return item_support;
	}

	static uint32_t get_item_support(
		const T& item,
		const std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>>& item_nodes) {

		const auto item_range = item_nodes.equal_range(item);

		return std::reduce(item_range.first, item_range.second, 0u, [](const auto sum, const auto& map_entry) {
			return sum + map_entry.second->support;
		});
	}

	std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>> get_conditional_item_nodes(
		const T& target,
		const std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>>& item_nodes,
		const uint32_t minimum_support) const {

		const auto target_range = item_nodes.equal_range(target);
		std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>> conditional_item_nodes;

		for (auto iterator = target_range.first; iterator != target_range.second; ++iterator) {
			for (auto node = iterator->second->parent; node->parent; node = node->parent) {

				const auto item = *node->value;
				const auto item_range = conditional_item_nodes.equal_range(item);
				const auto map_iterator = std::find_if(item_range.first, item_range.second, [&](const auto& map_entry) {
					return *node == *map_entry.second;
				});

				if (map_iterator != item_range.second) {
					map_iterator->second->support += iterator->second->support;
				} else {
					const auto conditional_node = std::make_shared<frequent_pattern_tree_node>(item, node->parent);
					conditional_node->support = iterator->second->support;
					conditional_item_nodes.insert(std::make_pair(item, conditional_node));
				}
			}
		}

		for (const auto& [item, _] : item_support_) {
			if (get_item_support(item, conditional_item_nodes) < minimum_support) {
				const auto item_range = conditional_item_nodes.equal_range(item);
				conditional_item_nodes.erase(item_range.first, item_range.second);
			}
		}

		return conditional_item_nodes;
	}

	std::set<T, std::function<bool(T, T)>> get_frequent_items(const uint32_t minimum_support) const {

		std::set<T, std::function<bool(T, T)>> frequent_items{
			[&](const T& a, const T& b) {
				return item_support_.at(a) != item_support_.at(b) ? item_support_.at(a) < item_support_.at(b) : a > b;
			}
		};

		for (const auto& [item, support] : item_support_) {
			if (support >= minimum_support) {
				frequent_items.insert(item);
			}
		}

		return frequent_items;
	}

	std::vector<std::unordered_set<T>> get_frequent_itemsets(
		const std::unordered_set<T>& frequent_itemset,
		const T& current,
		const std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>>& item_nodes,
		const uint32_t minimum_support) const {

		std::vector<std::unordered_set<T>> frequent_itemsets;
		const auto target_item_support = get_item_support(current, item_nodes);

		if (target_item_support >= minimum_support) {

			std::unordered_set<T> next_frequent_itemset{frequent_itemset};
			next_frequent_itemset.insert(current);
			frequent_itemsets.push_back(next_frequent_itemset);

			std::unordered_set<T> visited;
			for (const auto& [target, _] : item_nodes) {
				if (target != current && !visited.count(target)) {

					const auto conditional_item_nodes =
						get_conditional_item_nodes(current, item_nodes, minimum_support);

					const auto next_frequent_itemsets = get_frequent_itemsets(
						next_frequent_itemset, target, conditional_item_nodes, minimum_support);

					frequent_itemsets.insert(
						frequent_itemsets.end(), next_frequent_itemsets.begin(), next_frequent_itemsets.end());

					visited.insert(target);
				}
			}
		}

		return frequent_itemsets;
	}

	std::set<T, std::function<bool(T, T)>> order_items_by_support(const std::unordered_set<T>& itemset) const {

		return std::set<T, std::function<bool(T, T)>>{
			itemset.begin(), itemset.end(), [&](const T& a, const T& b) {
				return item_support_.at(a) != item_support_.at(b) ? item_support_.at(a) > item_support_.at(b) : a < b;
			}
		};
	}

	void insert(const std::unordered_set<T>& itemset) {

		auto iterator = root_;

		for (const auto& item : order_items_by_support(itemset)) {
			if (!iterator->children.count(item)) {
				iterator->children[item] = std::make_shared<frequent_pattern_tree_node>(item, iterator);
				item_nodes_.insert(std::make_pair(item, iterator->children[item]));
			} else {
				++iterator->children[item]->support;
			}
			iterator = iterator->children[item];
		}
	}

};
