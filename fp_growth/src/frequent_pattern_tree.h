#pragma once

#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <vector>

template <typename T>
class frequent_pattern_tree final {

	struct frequent_pattern_tree_node final {

		std::unique_ptr<T> value;
		std::shared_ptr<frequent_pattern_tree_node> parent;
		std::map<T, std::shared_ptr<frequent_pattern_tree_node>> children;

		frequent_pattern_tree_node() = default;

		frequent_pattern_tree_node(const T& value, std::shared_ptr<frequent_pattern_tree_node> parent)
			: value{std::make_unique<T>(value)},
			  parent{std::move(parent)} {}
	};

	std::shared_ptr<frequent_pattern_tree_node> root_;
	std::unordered_map<T, uint32_t> item_support_;
	std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>> item_nodes_;

public:

	explicit frequent_pattern_tree(const std::vector<std::unordered_set<T>>& itemsets = {})
		: root_{std::make_shared<frequent_pattern_tree_node>()},
		  item_support_{get_item_support(itemsets)} {

		for (const auto& itemset : itemsets) {
			insert(itemset);
		}
	}

	static std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>> get_conditional_item_nodes(
		const T& target,
		const std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>>& item_nodes) {

		const auto item_range = item_nodes.equal_range(target);
		std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>> conditional_item_nodes;

		for (auto iterator = item_range.first; iterator != item_range.second; ++iterator) {
			for (auto node = iterator->second->parent; node->parent; node = node->parent) {
				conditional_item_nodes.insert(std::make_pair(*node->value, node));
			}
		}

		return conditional_item_nodes;
	}

	std::vector<std::unordered_set<T>> get_frequent_itemsets(const uint32_t minimum_support) const {

		std::vector<T> frequent_items;
		for (const auto& [item, support] : item_support_) {
			if (support >= minimum_support) {
				frequent_items.push_back(item);
			}
		}

		std::sort(frequent_items.begin(), frequent_items.end(), [&](const T& a, const T& b) {
			const auto a_support = item_support_.at(a);
			const auto b_support = item_support_.at(b);
			return a_support == b_support ? a > b : a_support < b_support;
		});

		std::vector<std::unordered_set<T>> frequent_itemsets;
		for (size_t i = 0; i < frequent_items.size(); ++i) {
			const auto& current = frequent_items[i];
			frequent_itemsets.push_back({current});
			const auto conditional_node_list = get_conditional_item_nodes(current, item_nodes_);

			for (auto j = i + 1; j < frequent_items.size(); ++j) {
				const auto& target = frequent_items[j];
				const auto itemsets = get_frequent_itemsets(frequent_itemsets.back(), target, conditional_node_list,
				                                            minimum_support);
				frequent_itemsets.insert(frequent_itemsets.end(), itemsets.begin(), itemsets.end());
			}
		}

		return frequent_itemsets;
	}

	std::vector<std::unordered_set<T>> get_frequent_itemsets(
		const std::unordered_set<T>& current,
		const T& target,
		const std::unordered_multimap<T, std::shared_ptr<frequent_pattern_tree_node>>& item_nodes,
		const uint32_t minimum_support) const {

		std::vector<std::unordered_set<T>> frequent_itemsets;

		if (item_nodes.count(target) >= minimum_support) {
			frequent_itemsets.push_back(current);
			frequent_itemsets.back().insert(target);

			std::unordered_set<T> remaining_items;

			for (const auto& [item, _] : item_nodes) {
				if (item != target && !remaining_items.count(item)) {
					remaining_items.insert(item);

					const auto conditional_node_list = get_conditional_item_nodes(item, item_nodes);
					const auto itemsets = get_frequent_itemsets(frequent_itemsets.back(), item, conditional_node_list,
					                                            minimum_support);
					frequent_itemsets.insert(frequent_itemsets.end(), itemsets.begin(), itemsets.end());
				}
			}
		}

		return frequent_itemsets;
	}

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

		T operator*() const {
			return *item_stack_.top()->value;
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

	static std::unordered_map<T, uint32_t> get_item_support(const std::vector<std::unordered_set<T>>& itemsets) {

		std::unordered_map<T, uint32_t> item_support;

		for (const auto& itemset : itemsets) {
			for (const auto& item : itemset) {
				++item_support[item];
			}
		}

		return item_support;
	}

	std::set<T, std::function<bool(T, T)>> get_ordered_itemset(const std::unordered_set<T>& itemset) const {
		return std::set<T, std::function<bool(T, T)>>{
			itemset.begin(), itemset.end(), [&](const T& a, const T& b) {
				return item_support_.at(a) == item_support_.at(b) ? a < b : item_support_.at(a) > item_support_.at(b);
			}
		};
	}

	void insert(const std::unordered_set<T>& itemset) {

		auto iterator = root_;

		for (const auto& item : get_ordered_itemset(itemset)) {
			if (!iterator->children.count(item)) {
				iterator->children[item] = std::make_shared<frequent_pattern_tree_node>(item, iterator);
				item_nodes_.insert(std::make_pair(item, iterator->children[item]));
			}
			iterator = iterator->children[item];
		}
	}
};
