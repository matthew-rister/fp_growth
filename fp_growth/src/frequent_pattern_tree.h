#pragma once

#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

template <typename T>class FrequentPatternTree final {

	struct FrequentPatternTreeNode final {

		static inline uint32_t instance_count = 0;
		uint32_t id;
		std::optional<T> value;
		std::shared_ptr<FrequentPatternTreeNode> parent;
		std::map<T, std::shared_ptr<FrequentPatternTreeNode>> children;
		uint32_t support = 1;

		explicit FrequentPatternTreeNode(
			std::optional<T> value = std::nullopt,
			std::shared_ptr<FrequentPatternTreeNode> parent = nullptr)
			: id{++instance_count},
			  value{std::move(value)},
			  parent{std::move(parent)} {}
	};

	std::shared_ptr<FrequentPatternTreeNode> root_ = std::make_shared<FrequentPatternTreeNode>();
	std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> item_nodes_;

public:

	explicit FrequentPatternTree(const std::vector<std::unordered_set<T>>& itemsets = {}) {

		std::unordered_map<T, uint32_t> item_support = GetSupportByItem(itemsets);

		for (const auto& itemset : itemsets) {
			Insert(itemset, item_support);
		}
	}

	std::vector<std::unordered_set<T>> GetFrequentItemsets(const uint32_t minimum_support) const {

		std::vector<std::unordered_set<T>> frequent_itemsets;
		const auto frequent_items = GetFrequentItemsOrderedBySupportAscending(minimum_support, item_nodes_);

		for (auto current_item = frequent_items.begin(); current_item != frequent_items.end(); ++current_item) {
			const auto conditional_item_nodes = GetConditionalItemNodes(*current_item, item_nodes_, minimum_support);
			const std::unordered_set<T> current_itemset{*current_item};
			frequent_itemsets.push_back(current_itemset);

			for (auto next_item = std::next(current_item); next_item != frequent_items.end(); ++next_item) {
				const auto next_itemsets = GetFrequentItemsets(
					current_itemset, *next_item, conditional_item_nodes, minimum_support);
				frequent_itemsets.insert(frequent_itemsets.end(), next_itemsets.begin(), next_itemsets.end());
			}
		}

		return frequent_itemsets;
	}

private:

	static std::shared_ptr<FrequentPatternTreeNode> FindItemNodeInMap(
		const FrequentPatternTreeNode& node,
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

		const auto item_range = item_nodes.equal_range(*node.value);
		const auto item_range_iterator = std::find_if(item_range.first, item_range.second, [&](const auto& map_entry) {
			return node.id == map_entry.second->id;
		});

		return item_range_iterator != item_range.second ? item_range_iterator->second : nullptr;
	}

	static std::set<T, std::function<bool(T, T)>> GetFrequentItemsOrderedBySupportAscending(
		const uint32_t minimum_support,
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

		const auto support_by_item = GetSupportByItem(item_nodes);

		std::set<T, std::function<bool(T, T)>> frequent_items{[&](const T& a, const T& b) {
			return support_by_item.at(a) != support_by_item.at(b)
				       ? support_by_item.at(a) < support_by_item.at(b)
				       : a > b;
		}};

		for (const auto& [item, support] : support_by_item) {
			if (support >= minimum_support) {
				frequent_items.insert(item);
			}
		}

		return frequent_items;
	}

	static uint32_t GetItemSupport(
		const T& item,
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

		const auto item_range = item_nodes.equal_range(item);

		return std::reduce(item_range.first, item_range.second, 0u, [](const auto sum, const auto& map_entry) {
			return sum + map_entry.second->support;
		});
	}

	static std::unordered_map<T, uint32_t> GetSupportByItem(const std::vector<std::unordered_set<T>>& itemsets) {

		std::unordered_map<T, uint32_t> support_by_item;

		for (const auto& itemset : itemsets) {
			for (const auto& item : itemset) {
				++support_by_item[item];
			}
		}

		return support_by_item;
	}

	static std::unordered_map<T, uint32_t> GetSupportByItem(
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

		std::unordered_map<T, uint32_t> item_support;

		for (const auto&[item, _] : item_nodes) {
			if (!item_support.count(item)) {
				item_support[item] = GetItemSupport(item, item_nodes);
			}
		}

		return item_support;
	}

	static void RemoveInfrequentItemNodes(
		const uint32_t minimum_support,
		std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

		for (const auto& [item, support] : GetSupportByItem(item_nodes)) {
			if (support < minimum_support) {
				const auto item_range = item_nodes.equal_range(item);
				item_nodes.erase(item_range.first, item_range.second);
			}
		}
	}

	std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> GetConditionalItemNodes(
		const T& target,
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes,
		const uint32_t minimum_support) const {

		std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> conditional_item_nodes;
		const auto target_range = item_nodes.equal_range(target);

		for (auto target_iterator = target_range.first; target_iterator != target_range.second; ++target_iterator) {
			for (auto node_iterator = target_iterator->second->parent; node_iterator->parent;
			     node_iterator = node_iterator->parent) {

				if (auto item_node = FindItemNodeInMap(*node_iterator, conditional_item_nodes); item_node) {
					item_node->support += target_iterator->second->support;
				} else {
					item_node = std::make_shared<FrequentPatternTreeNode>(*node_iterator);
					item_node->support = target_iterator->second->support;
					conditional_item_nodes.insert(std::make_pair(*node_iterator->value, item_node));
				}
			}
		}

		RemoveInfrequentItemNodes(minimum_support, conditional_item_nodes);
		return conditional_item_nodes;
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
					const auto next_conditional_item_nodes = GetConditionalItemNodes(
						next_item, conditional_item_nodes, minimum_support);
					const auto next_itemsets = GetFrequentItemsets(
						next_itemset, item, next_conditional_item_nodes, minimum_support);
					frequent_itemsets.insert(frequent_itemsets.end(), next_itemsets.begin(), next_itemsets.end());
					visited.insert(item);
				}
			}
		}

		return frequent_itemsets;
	}

	std::set<T, std::function<bool(T, T)>> OrderItemsBySupportDescending(
		const std::unordered_set<T>& itemset,
		const std::unordered_map<T, uint32_t>& support_by_item) const {

		return std::set<T, std::function<bool(T, T)>>{itemset.begin(), itemset.end(), [&](const T& a, const T& b) {
			return support_by_item.at(a) != support_by_item.at(b)
				? support_by_item.at(a) > support_by_item.at(b)
				: a < b;
		}};
	}

	void Insert(const std::unordered_set<T>& itemset, const std::unordered_map<T, uint32_t>& item_support) {

		auto iterator = root_;

		for (const auto& item : OrderItemsBySupportDescending(itemset, item_support)) {
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
