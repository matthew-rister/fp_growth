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

		class FrequentPatternTreeNode final {

		public:
			uint32_t id;
			std::optional<T> item;
			std::shared_ptr<FrequentPatternTreeNode> parent;
			std::unordered_map<T, std::shared_ptr<FrequentPatternTreeNode>> children;
			uint32_t support = 1;

			explicit FrequentPatternTreeNode(
				std::optional<const T> item = std::nullopt,
				std::shared_ptr<FrequentPatternTreeNode> parent = nullptr)
				: id{++instance_count_},
				  item{std::move(item)},
				  parent{std::move(parent)} {}

		private:
			static inline uint32_t instance_count_ = 0;
		};

	public:
		FrequentPatternTree() = default;
		
		FrequentPatternTree(const std::initializer_list<std::unordered_set<T>>& itemsets)
			: FrequentPatternTree{itemsets.begin(), itemsets.end()} {}

		template <typename ItemsetIterator> 
		FrequentPatternTree(const ItemsetIterator& begin, const ItemsetIterator end) {

			const auto item_support = get_item_support(begin, end);

			for (auto itemset = begin; itemset != end; ++itemset) {
				insert(*itemset, item_support);
			}
		}

		[[nodiscard]] std::vector<std::unordered_set<T>> get_frequent_itemsets(const uint32_t minimum_support) const {
			return get_frequent_itemsets({}, item_nodes_, minimum_support);
		}

	private:
		std::shared_ptr<FrequentPatternTreeNode> root_ = std::make_shared<FrequentPatternTreeNode>();
		std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> item_nodes_;

		template <typename ItemsetIterator>
		static std::unordered_map<T, uint32_t> get_item_support(const ItemsetIterator& begin, const ItemsetIterator& end) {

			std::unordered_map<T, uint32_t> item_support;

			for (auto itemset = begin; itemset != end; ++itemset) {
				for (const auto& item : *itemset) {
					++item_support[item];
				}
			};

			return item_support;
		}

		void insert(const std::unordered_set<T>& itemset, const std::unordered_map<T, uint32_t>& item_support) {

			std::set<T, std::function<bool(T, T)>> items_by_descending_support{itemset.cbegin(), itemset.cend(),
				[&](const T& a, const T& b) {
					return item_support.at(a) != item_support.at(b) ? item_support.at(a) > item_support.at(b) : a < b;
				}};

			auto iterator = root_;

			for (const auto& item : items_by_descending_support) {
				if (!iterator->children.count(item)) {
					iterator->children[item] = std::make_shared<FrequentPatternTreeNode>(item, iterator);
					item_nodes_.insert(std::make_pair(item, iterator->children[item]));
				} else {
					++iterator->children[item]->support;
				}
				iterator = iterator->children[item];
			}
		}

		static std::vector<std::unordered_set<T>> get_frequent_itemsets(
			const std::unordered_set<T>& current_itemset,
			const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes,
			const uint32_t minimum_support) {

			std::vector<std::unordered_set<T>> frequent_itemsets;

			for (const auto& next_item : get_unique_items(item_nodes)) {
				if (get_item_support(next_item, item_nodes) >= minimum_support) {

					std::unordered_set<T> next_itemset{current_itemset};
					next_itemset.insert(next_item);
					frequent_itemsets.push_back(next_itemset);

					const auto conditional_item_nodes = get_conditional_item_nodes(next_item, item_nodes);
					const auto next_itemsets = get_frequent_itemsets(next_itemset, conditional_item_nodes, minimum_support);
					frequent_itemsets.insert(frequent_itemsets.cend(), next_itemsets.cbegin(), next_itemsets.cend());
				}
			}

			return frequent_itemsets;
		}

		static std::unordered_set<T> get_unique_items(
			const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

			std::unordered_set<T> unique_items;

			std::transform(item_nodes.cbegin(), item_nodes.cend(), std::inserter(unique_items, unique_items.end()),
				[](const auto& map_entry) { return map_entry.first; });

			return unique_items;
		}

		static uint32_t get_item_support(
			const T& item,
			const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

			const auto item_range = item_nodes.equal_range(item);

			return std::reduce(item_range.first, item_range.second, 0u,
				[](const auto sum, const auto& map_entry) { return sum + map_entry.second->support; });
		}

		static std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> get_conditional_item_nodes(
			const T& target,
			const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

			std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> conditional_item_nodes;
			const auto target_range = item_nodes.equal_range(target);

			for (auto target_iterator = target_range.first; target_iterator != target_range.second; ++target_iterator) {
				for (auto node = target_iterator->second->parent; node->parent; node = node->parent) {

					if (auto item_node = find_item_node(*node, conditional_item_nodes); item_node) {
						item_node->support += target_iterator->second->support;
					} else {
						item_node = std::make_shared<FrequentPatternTreeNode>(*node);
						item_node->support = target_iterator->second->support;
						conditional_item_nodes.insert(std::make_pair(*node->item, item_node));
					}
				}
			}

			return conditional_item_nodes;
		}

		static std::shared_ptr<FrequentPatternTreeNode> find_item_node(
			const FrequentPatternTreeNode& item_node,
			const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

			const auto item = *item_node.item;
			const auto item_range = item_nodes.equal_range(item);
			const auto item_range_iterator = std::find_if(item_range.first, item_range.second,
				[&](const auto& map_entry) { return item_node.id == map_entry.second->id; });

			return item_range_iterator != item_range.second ? item_range_iterator->second : nullptr;
		}
	};
}
