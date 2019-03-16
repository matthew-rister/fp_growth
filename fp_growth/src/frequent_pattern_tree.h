#pragma once

#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * \brief Represents a tree used to efficiently store and extract frequent pattern itemsets.
 * \tparam T The type used to represent each item.
 */
template <typename T>
class FrequentPatternTree final {

	/** \brief Represents an item node in the frequent pattern tree. */
	struct FrequentPatternTreeNode final {

		/** \brief The number of total nodes created. */
		static inline uint32_t instance_count = 0;

		/** \brief The node ID. */
		uint32_t id;

		/** \brief The node item. */
		std::optional<T> item;

		/** \brief The parent node in the frequent pattern tree. */
		std::shared_ptr<FrequentPatternTreeNode> parent;

		/** \brief A mapping of child nodes by item type. */
		std::unordered_map<T, std::shared_ptr<FrequentPatternTreeNode>> children;

		/** \brief A count of the number of times this node item was encountered in an itemset. */
		uint32_t support = 1;

		/** \brief Initializes a frequent pattern tree node. */
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

	/** \brief Initializes a frequent pattern tree from a collection of itemsets. */
	explicit FrequentPatternTree(const std::vector<std::unordered_set<T>>& itemsets = {}) {

		std::unordered_map<T, uint32_t> item_support = GetItemSupport(itemsets);

		for (const auto& itemset : itemsets) {
			Insert(itemset, item_support);
		}
	}

	/**
	 * \brief Gets all frequently occurring itemsets.
	 * \param minimum_support The minimum support needed for an itemset to be considered frequent.
	 * \return All frequently occurring itemsets with support greater than \p minimum_support.
	 */
	std::vector<std::unordered_set<T>> GetFrequentItemsets(const uint32_t minimum_support) const {
		return GetFrequentItemsets({}, item_nodes_, minimum_support);
	}

private:

	/**
	 * \brief Looks for a node in an item node multimap.
	 * \param node The node to look for.
	 * \param item_nodes A multimap containing references to nodes by type.
	 * \return The node if present in \p item_nodes, otherwise \c nullptr. \c
	 */
	static std::shared_ptr<FrequentPatternTreeNode> FindNodeInItemRange(
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
	static std::unordered_map<T, uint32_t> GetItemSupport(const std::vector<std::unordered_set<T>>& itemsets) {

		std::unordered_map<T, uint32_t> item_support;

		for (const auto& itemset : itemsets) {
			for (const auto& item : itemset) {
				++item_support[item];
			}
		}

		return item_support;
	}

	/**
	 * \brief Gets the support for each item in an item node multimap.
	 * \param item_nodes A multimap containing references to nodes by item type.
	 * \return The support for each item in \p item_nodes
	 */
	static std::unordered_map<T, uint32_t> GetItemSupport(
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes) {

		std::unordered_map<T, uint32_t> item_support;

		for (const auto& [item, _] : item_nodes) {
			if (!item_support.count(item)) {
				item_support[item] = GetItemSupport(item, item_nodes);
			}
		}

		return item_support;
	}

	/**
	 * \brief Gets all frequent item nodes and their relative support which are ancestors of a target item node.
	 * \param target The target item to get frequent ancestor item nodes for.
	 * \param item_nodes A multimap containing references to nodes by item type.
	 * \param minimum_support The minimum support for an itemset to be considered frequent.
	 * \return All frequent item nodes which are ancestors of \p target.
	 */
	std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> GetConditionalItemNodes(
		const T& target,
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes,
		const uint32_t minimum_support) const {

		std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>> conditional_item_nodes;
		const auto target_range = item_nodes.equal_range(target);

		for (auto target_iterator = target_range.first; target_iterator != target_range.second; ++target_iterator) {
			for (auto node = target_iterator->second->parent; node->parent; node = node->parent) {

				if (auto item_node = FindNodeInItemRange(*node, conditional_item_nodes); item_node) {
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

	/**
	 * \brief Gets frequently occurring itemsets.
	 * \param current_itemset The current frequent itemset to generate candidate itemsets from.
	 * \param item_nodes A multimap containing references to nodes by item type.
	 * \param minimum_support The minimum support needed for an itemset to be considered frequent.
	 * \return Frequently occurring itemsets generated from \p current_itemset
	 */
	std::vector<std::unordered_set<T>> GetFrequentItemsets(
		const std::unordered_set<T>& current_itemset,
		const std::unordered_multimap<T, std::shared_ptr<FrequentPatternTreeNode>>& item_nodes,
		const uint32_t minimum_support) const {

		std::set<T> visited_items;
		std::vector<std::unordered_set<T>> frequent_itemsets;

		for (const auto& [next_item, _] : item_nodes) {
			if (!visited_items.count(next_item) && GetItemSupport(next_item, item_nodes) >= minimum_support) {

				std::unordered_set<T> next_itemset{current_itemset};
				next_itemset.insert(next_item);
				frequent_itemsets.push_back(next_itemset);

				const auto conditional_item_nodes = GetConditionalItemNodes(next_item, item_nodes, minimum_support);
				const auto next_itemsets = GetFrequentItemsets(next_itemset, conditional_item_nodes, minimum_support);
				frequent_itemsets.insert(frequent_itemsets.end(), next_itemsets.begin(), next_itemsets.end());
			}
			visited_items.insert(next_item);
		}

		return frequent_itemsets;
	}

	/**
	 * \brief Inserts an itemset into the frequent pattern tree.
	 * \param itemset The itemset to support.
	 * \param item_support A map containing support by item.
	 */
	void Insert(const std::unordered_set<T>& itemset, const std::unordered_map<T, uint32_t>& item_support) {

		auto iterator = root_;

		for (const auto& item : OrderItemsByDescendingSupport(itemset, item_support)) {
			if (!iterator->children.count(item)) {
				iterator->children[item] = std::make_shared<FrequentPatternTreeNode>(item, iterator);
				item_nodes_.insert(std::make_pair(item, iterator->children[item]));
			} else {
				++iterator->children[item]->support;
			}
			iterator = iterator->children[item];
		}
	}

	/**
	 * \brief Orders all items in an itemset by descending support.
	 * \param itemset The itemset to sort.
	 * \param item_support A map containing support by item.
	 * \return An ordered set of items sorted by descending support.
	 */
	std::set<T, std::function<bool(T, T)>> OrderItemsByDescendingSupport(
		const std::unordered_set<T>& itemset,
		const std::unordered_map<T, uint32_t>& item_support) const {

		return std::set<T, std::function<bool(T, T)>>{itemset.begin(), itemset.end(), [&](const T& a, const T& b) {
			return item_support.at(a) != item_support.at(b) ? item_support.at(a) > item_support.at(b) : a < b;
		}};
	}
};
