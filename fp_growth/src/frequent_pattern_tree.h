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

/**
 * \brief Represents a tree used to efficiently store and extract frequent pattern itemsets.
 * \tparam T The type used to represent each item.
 */
template <typename T>
class FrequentPatternTree final {

	/** \brief Represents an item node in the frequent pattern tree.*/
	struct FrequentPatternTreeNode final {

		/** \brief The number of created node instances. */
		static inline uint32_t instance_count = 0;

		/** \brief The node ID. */
		uint32_t id;

		/** \brief The node item value. */
		std::optional<T> item;

		/** \brief A reference to the parent node in the frequent pattern tree. */
		std::shared_ptr<FrequentPatternTreeNode> parent;

		/** \brief A mapping of child nodes references by item type. */
		std::map<T, std::shared_ptr<FrequentPatternTreeNode>> children;


		/**
		 * \brief A count of the number of times this node item was encountered in an itemset
		 *		  represented by the path to this node from the root of the frequent pattern tree.
		 */
		uint32_t support = 1;

		/**
		 * \brief Initializes a frequent pattern tree node.
		 * \param item The node item.
		 * \param parent A reference to the parent node.
		 */
		explicit FrequentPatternTreeNode(
			std::optional<T> item = std::nullopt,
			std::shared_ptr<FrequentPatternTreeNode> parent = nullptr)
			: id{++instance_count},
			  item{std::move(item)},
			  parent{std::move(parent)} {}
	};

	/** \brief The root of the frequent pattern tree. */
	std::shared_ptr<FrequentPatternTreeNode> root_ = std::make_shared<FrequentPatternTreeNode>();

	/** \brief A mapping of nodes in the tree by item type. */
	std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> item_nodes_;

public:

	/**
	 * \brief Initializes a frequent pattern tree from a collection of itemsets.
	 * \param itemsets A collection of itemsets to build the frequent pattern tree from.
	 */
	explicit FrequentPatternTree(const std::vector<std::unordered_set<T>>& itemsets = {}) {

		std::unordered_map<T, uint32_t> support_by_item = GetSupportByItem(itemsets);

		for (const auto& itemset : itemsets) {
			Insert(itemset, support_by_item);
		}
	}

	/**
	 * \brief Gets all frequently occurring itemsets.
	 * \param minimum_support The minimum support needed for an itemset to be considered frequent.
	 * \return All frequently occurring itemsets with support greater than \p minimum_support.
	 */
	std::vector<std::unordered_set<T>> GetFrequentItemsets(const uint32_t minimum_support) const {

		std::vector<std::unordered_set<T>> frequent_itemsets;
		const auto frequent_items = GetFrequentItemsOrderedByAscendingSupport(minimum_support, item_nodes_);

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

	/**
	 * \brief Looks for a node in an item node multimap.
	 * \param node The node to look for.
	 * \param item_nodes A multimap containing references to nodes by type.
	 * \return The node if present in the multimap, otherwise \c nullptr. \c
	 */
	static std::shared_ptr<FrequentPatternTreeNode> FindItemNodeInMap(
		const FrequentPatternTreeNode& node,
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

		const auto item = *node.item;
		const auto item_range = item_nodes.equal_range(item);
		const auto item_range_iterator = std::find_if(item_range.first, item_range.second, [&](const auto& map_entry) {
			return node.id == map_entry.second->id;
		});

		return item_range_iterator != item_range.second ? item_range_iterator->second : nullptr;
	}

	/**
	 * \brief Gets frequently occurring items in an item node multimap ordered by ascending support.
	 * \param minimum_support The minimum support needed for an item to be considered frequent.
	 * \param item_nodes A multimap containing references to nodes by item type.
	 * \return All frequently occurring items in \p item_nodes.
	 */
	static std::set<T, std::function<bool(T, T)>> GetFrequentItemsOrderedByAscendingSupport(
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

	/**
	 * \brief Gets the support for an item in an item node multimap.
	 * \param item The item to determine the support for.
	 * \param item_nodes A multimap containing references to nodes by item type.
	 * \return The support for \p item in \p item_nodes.
	 */
	static uint32_t GetItemSupport(
		const T& item,
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

		const auto item_range = item_nodes.equal_range(item);

		return std::reduce(item_range.first, item_range.second, 0u, [](const auto sum, const auto& map_entry) {
			return sum + map_entry.second->support;
		});
	}

	/**
	 * \brief Gets the support for each item type in a collection of itemsets.
	 * \param itemsets A collection of itemsets to determine support for.
	 * \return The support for each item in \p itemsets.
	 */
	static std::unordered_map<T, uint32_t> GetSupportByItem(const std::vector<std::unordered_set<T>>& itemsets) {

		std::unordered_map<T, uint32_t> support_by_item;

		for (const auto& itemset : itemsets) {
			for (const auto& item : itemset) {
				++support_by_item[item];
			}
		}

		return support_by_item;
	}

	/**
	 * \brief Gets the support for each item in an item node multimap.
	 * \param item_nodes A multimap containing references to nodes by item type.
	 * \return The support for each item in \p item_nodes
	 */
	static std::unordered_map<T, uint32_t> GetSupportByItem(
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

		std::unordered_map<T, uint32_t> support_by_item;

		for (const auto& [item, _] : item_nodes) {
			if (!support_by_item.count(item)) {
				support_by_item[item] = GetItemSupport(item, item_nodes);
			}
		}

		return support_by_item;
	}

	/**
	 * \brief Removes infrequent item nodes from an item node multimap.
	 * \param minimum_support The minimum support needed to be considered frequent.
	 * \param item_nodes A multimap containing references to nodes by item type.
	 */
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

	/**
	 * \brief Gets all frequent item nodes and their relative support which are ancestors of a target item node.
	 * \param target The target item to get frequent ancestor item nodes for.
	 * \param item_nodes A multimap containing references to nodes by item type.
	 * \param minimum_support The minimum support for an item to be considered frequent.
	 * \return All frequent item nodes which are ancestors of \p target.
	 */
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
					conditional_item_nodes.insert(std::make_pair(*node_iterator->item, item_node));
				}
			}
		}

		RemoveInfrequentItemNodes(minimum_support, conditional_item_nodes);
		return conditional_item_nodes;
	}

	/**
	 * \brief Recursively gets all frequently occurring itemsets.
	 * \param current_itemset The current itemset to build candidate frequent itemsets from.
	 * \param next_item The next item to merge into the current itemset.
	 * \param conditional_item_nodes The conditional item nodes built from current itemset.
	 * \param minimum_support The minimum support needed for an item to be considered frequent.
	 * \return All frequent itemsets built from the current itemset.
	 */
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

	/**
	 * \brief Orders all items in an itemset by descending support.
	 * \param itemset The itemset to sort.
	 * \param support_by_item A map containing support by item.
	 * \return An ordered set of items sorted by descending support.
	 */
	std::set<T, std::function<bool(T, T)>> OrderItemsByDescendingSupport(
		const std::unordered_set<T>& itemset,
		const std::unordered_map<T, uint32_t>& support_by_item) const {

		return std::set<T, std::function<bool(T, T)>>{itemset.begin(), itemset.end(), [&](const T& a, const T& b) {
			return support_by_item.at(a) != support_by_item.at(b)
				? support_by_item.at(a) > support_by_item.at(b)
				: a < b;
		}};
	}

	/**
	 * \brief Inserts an itemset into the frequent pattern tree.
	 * \param itemset The itemset to support.
	 * \param support_by_item A map containing support by item.
	 */
	void Insert(const std::unordered_set<T>& itemset, const std::unordered_map<T, uint32_t>& support_by_item) {

		auto iterator = root_;

		for (const auto& item : OrderItemsByDescendingSupport(itemset, support_by_item)) {
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
