#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fpt {

	template <typename T> class FrequentPatternTree final {

		struct FrequentPatternTreeNode final {

			explicit FrequentPatternTreeNode(
				std::optional<const T> item = std::nullopt,
				std::shared_ptr<FrequentPatternTreeNode> parent = nullptr)
				: id{++instance_count_},
				  item{std::move(item)},
				  parent{std::move(parent)} {}

			uint32_t id;
			std::optional<T> item;
			std::shared_ptr<FrequentPatternTreeNode> parent;
			std::unordered_map<T, std::shared_ptr<FrequentPatternTreeNode>> children;
			uint32_t support = 1;

		private:
			static inline uint32_t instance_count_ = 0;
		};

	public:
		FrequentPatternTree() = default;

		FrequentPatternTree(const std::initializer_list<std::unordered_set<T>>& itemsets)
			: FrequentPatternTree{std::cbegin(itemsets), std::cend(itemsets)} {}

		template <typename ItemsetIterator> FrequentPatternTree(const ItemsetIterator& begin, const ItemsetIterator& end) {
			const auto item_support = GetItemSupport(begin, end);

			for (auto itemset = begin; itemset != end; ++itemset) {
				Insert(*itemset, item_support);
			}
		}

		[[nodiscard]] std::vector<std::unordered_set<T>> GetFrequentItemsets(const uint32_t minimum_support) const {
			return GetFrequentItemsets({}, item_nodes_, minimum_support);
		}

	private:
		template <typename ItemsetIterator>
		static std::unordered_map<T, uint32_t> GetItemSupport(const ItemsetIterator& begin, const ItemsetIterator& end) {

			std::unordered_map<T, uint32_t> item_support;

			for (auto itemset = begin; itemset != end; ++itemset) {
				for (const auto& item : *itemset) {
					++item_support[item];
				}
			};

			return item_support;
		}

		void Insert(const std::unordered_set<T>& itemset, const std::unordered_map<T, uint32_t>& item_support) {

			std::set<T, std::function<bool(T, T)>> items_by_descending_support{itemset.cbegin(), itemset.cend(),
				[&](const T& a, const T& b) {
					return item_support.at(a) != item_support.at(b) ? item_support.at(a) > item_support.at(b) : a < b;
				}};

			auto iterator = root_;

			for (const auto& item : items_by_descending_support) {
				if (!iterator->children.count(item)) {
					iterator->children[item] = std::make_shared<FrequentPatternTreeNode>(item, iterator);
					item_nodes_.emplace(item, iterator->children[item]);
				} else {
					++iterator->children[item]->support;
				}
				iterator = iterator->children[item];
			}
		}

		static std::vector<std::unordered_set<T>> GetFrequentItemsets(
			const std::unordered_set<T>& current_itemset,
			const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes,
			const uint32_t minimum_support) {

			std::vector<std::unordered_set<T>> frequent_itemsets;

			for (const auto& next_item : GetUniqueItems(item_nodes)) {
				if (GetItemSupport(next_item, item_nodes) >= minimum_support) {

					std::unordered_set<T> next_itemset{current_itemset};
					next_itemset.insert(next_item);
					frequent_itemsets.push_back(next_itemset);

					const auto conditional_item_nodes = GetConditionalItemNodes(next_item, item_nodes);
					const auto next_itemsets = GetFrequentItemsets(next_itemset, conditional_item_nodes, minimum_support);
					frequent_itemsets.insert(frequent_itemsets.cend(), next_itemsets.cbegin(), next_itemsets.cend());
				}
			}

			return frequent_itemsets;
		}

		static std::unordered_set<T> GetUniqueItems(
			const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

			std::unordered_set<T> unique_items;

			std::transform(item_nodes.cbegin(), item_nodes.cend(), std::inserter(unique_items, std::end(unique_items)),
				[](const auto& map_entry) { return map_entry.first; });

			return unique_items;
		}

		static uint32_t GetItemSupport(
			const T& item,
			const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

			const auto item_range = item_nodes.equal_range(item);

			return std::transform_reduce(item_range.first, item_range.second, uint32_t{0}, std::plus<uint32_t>{},
				[](const auto& map_entry) { return map_entry.second->support; });
		}

		static std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> GetConditionalItemNodes(
			const T& target,
			const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

			std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> conditional_item_nodes;
			const auto target_range = item_nodes.equal_range(target);

			for (auto target_iterator = target_range.first; target_iterator != target_range.second; ++target_iterator) {
				for (auto node = target_iterator->second->parent; node->parent; node = node->parent) {

					if (auto item_node = FindItemNode(*node, conditional_item_nodes); item_node) {
						item_node->support += target_iterator->second->support;
					} else {
						item_node = std::make_shared<FrequentPatternTreeNode>(*node);
						item_node->support = target_iterator->second->support;
						conditional_item_nodes.emplace(*node->item, item_node);
					}
				}
			}

			return conditional_item_nodes;
		}

		static std::shared_ptr<FrequentPatternTreeNode> FindItemNode(
			const FrequentPatternTreeNode& item_node,
			const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

			const auto item = *item_node.item;
			const auto item_range = item_nodes.equal_range(item);
			const auto item_range_iterator = std::find_if(item_range.first, item_range.second,
				[&](const auto& map_entry) { return item_node.id == map_entry.second->id; });

			return item_range_iterator != item_range.second ? item_range_iterator->second : nullptr;
		}

		std::shared_ptr<FrequentPatternTreeNode> root_ = std::make_shared<FrequentPatternTreeNode>();
		std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> item_nodes_;
	};
}
