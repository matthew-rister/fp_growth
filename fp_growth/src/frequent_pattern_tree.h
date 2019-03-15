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
class FrequentPatternTree final {

	struct FrequentPatternTreeNode final {

		std::optional<T> value;
		std::shared_ptr<FrequentPatternTreeNode> parent;
		std::map<T, std::shared_ptr<FrequentPatternTreeNode>> children;
		uint32_t support = 1;

		explicit FrequentPatternTreeNode(
			std::optional<T> value = std::nullopt,
			std::shared_ptr<FrequentPatternTreeNode> parent = nullptr)
			: value{std::move(value)},
			  parent{std::move(parent)} {}

		friend bool operator==(const FrequentPatternTreeNode& lhs, const FrequentPatternTreeNode& rhs) {
			return &lhs == &rhs;
		}
	};

	std::shared_ptr<FrequentPatternTreeNode> root_ = std::make_shared<FrequentPatternTreeNode>();
	std::unordered_map<T, uint32_t> item_support_;
	std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> item_nodes_;

public:

	explicit FrequentPatternTree(const std::vector<std::unordered_set<T>>& itemsets = {})
		: item_support_{GetItemSupport(itemsets)} {

		for (const auto& itemset : itemsets) {
			Insert(itemset);
		}
	}

	std::vector<std::unordered_set<T>> GetFrequentItemsets(const uint32_t minimum_support) const {
		std::vector<std::unordered_set<T>> frequent_itemsets;
		const auto frequent_items = GetFrequentItemsOrderedBySupportAscending(minimum_support);

		for (auto current = frequent_items.begin(); current != frequent_items.end(); ++current) {
			const auto conditional_item_nodes = GetConditionalItemNodes(*current, item_nodes_, minimum_support);
			const std::unordered_set<T> current_itemset{*current};
			frequent_itemsets.push_back(current_itemset);

			for (auto next = std::next(current); next != frequent_items.end(); ++next) {
				const auto next_itemsets = GetFrequentItemsets(current_itemset, *next, conditional_item_nodes, minimum_support);
				frequent_itemsets.insert(frequent_itemsets.end(), next_itemsets.begin(), next_itemsets.end());
			}
		}

		return frequent_itemsets;
	}

private:

	static std::unordered_map<T, uint32_t> GetItemSupport(const std::vector<std::unordered_set<T>>& itemsets) {

		std::unordered_map<T, uint32_t> item_support;

		for (const auto& itemset : itemsets) {
			for (const auto& item : itemset) {
				++item_support[item];
			}
		}

		return item_support;
	}

	static uint32_t GetItemSupport(
		const T& item,
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

		const auto item_range = item_nodes.equal_range(item);

		return std::reduce(item_range.first, item_range.second, 0u, [](const auto sum, const auto& map_entry) {
			return sum + map_entry.second->support;
		});
	}

	std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> GetConditionalItemNodes(
		const T& target,
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes,
		const uint32_t minimum_support) const {

		const auto target_range = item_nodes.equal_range(target);
		std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> conditional_item_nodes;

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
					const auto conditional_item_node = std::make_shared<FrequentPatternTreeNode>(item, node->parent);
					conditional_item_node->support = iterator->second->support;
					conditional_item_nodes.insert(std::make_pair(item, conditional_item_node));
				}
			}
		}

		for (const auto& [item, _] : item_support_) {
			if (GetItemSupport(item, conditional_item_nodes) < minimum_support) {
				const auto item_range = conditional_item_nodes.equal_range(item);
				conditional_item_nodes.erase(item_range.first, item_range.second);
			}
		}

		return conditional_item_nodes;
	}

	std::set<T, std::function<bool(T, T)>> GetFrequentItemsOrderedBySupportAscending(
		const uint32_t minimum_support) const {

		std::set<T, std::function<bool(T, T)>> frequent_items{[&](const T& a, const T& b) {
			return item_support_.at(a) != item_support_.at(b) ? item_support_.at(a) < item_support_.at(b) : a > b;
		}};

		for (const auto& [item, support] : item_support_) {
			if (support >= minimum_support) {
				frequent_items.insert(item);
			}
		}

		return frequent_items;
	}

	std::vector<std::unordered_set<T>> GetFrequentItemsets(
		const std::unordered_set<T>& current_itemset,
		const T& next_item,
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& conditional_item_nodes,
		const uint32_t minimum_support) const {

		std::vector<std::unordered_set<T>> frequent_itemsets;

		if (GetItemSupport(next_item, conditional_item_nodes) >= minimum_support) {

			std::unordered_set<T> visited;
			std::unordered_set<T> next_itemset{current_itemset};
			next_itemset.insert(next_item);
			frequent_itemsets.push_back(next_itemset);

			for (const auto& [item, _] : conditional_item_nodes) {
				if (!visited.count(item)) {

					const auto next_conditional_item_nodes =
						GetConditionalItemNodes(next_item, conditional_item_nodes, minimum_support);

					const auto next_itemsets = GetFrequentItemsets(
						next_itemset, item, next_conditional_item_nodes, minimum_support);

					frequent_itemsets.insert(
						frequent_itemsets.end(), next_itemsets.begin(), next_itemsets.end());

					visited.insert(item);
				}
			}
		}

		return frequent_itemsets;
	}

	std::set<T, std::function<bool(T, T)>> OrderItemsBySupportDescending(const std::unordered_set<T>& itemset) const {

		return std::set<T, std::function<bool(T, T)>>{
			itemset.begin(), itemset.end(), [&](const T& a, const T& b) {
				return item_support_.at(a) != item_support_.at(b) ? item_support_.at(a) > item_support_.at(b) : a < b;
			}
		};
	}

	void Insert(const std::unordered_set<T>& itemset) {

		auto iterator = root_;

		for (const auto& item : OrderItemsBySupportDescending(itemset)) {
			if (!iterator->children.count(item)) {
				iterator->children[item] = std::make_shared<FrequentPatternTreeNode>(item, iterator);
				item_nodes_.insert(std::make_pair(item, iterator->children[item]));
			} else {
				++iterator->children[item]->support;
			}
			iterator = iterator->children[item];
		}
	}

};
