#pragma once

#include <map>
#include <memory>
#include <set>
#include <stack>
#include <vector>
#include <unordered_set>

template <typename T>
class frequent_pattern_tree final {

	struct frequent_pattern_tree_node final {

		std::unique_ptr<T> value;
		uint32_t count = 1;
		std::shared_ptr<frequent_pattern_tree_node> parent;
		std::map<T, std::shared_ptr<frequent_pattern_tree_node>> children;

		frequent_pattern_tree_node() = default;

		frequent_pattern_tree_node(const T& value, std::shared_ptr<frequent_pattern_tree_node> parent)
			: value{std::make_unique<T>(value)},
			  parent{std::move(parent)} {}
	};

	std::shared_ptr<frequent_pattern_tree_node> root_ = std::make_shared<frequent_pattern_tree_node>();
	std::unordered_map<T, std::vector<std::shared_ptr<frequent_pattern_tree_node>>> node_links_;

public:

	explicit frequent_pattern_tree(const std::vector<std::unordered_set<T>>& itemsets = {}) {
		for (const auto& itemset : get_ordered_itemsets(itemsets)) {
			insert(itemset);
		}
	}

	void insert(const std::set<T, std::function<bool(T, T)>>& itemset) {

		auto iterator = root_;

		for (const auto& item : itemset) {
			if (!iterator->children.count(item)) {
				iterator->children[item] = std::make_shared<frequent_pattern_tree_node>(item, iterator);
				create_link(iterator->children[item]);
			} else {
				++iterator->children[item]->count;
			}
			iterator = iterator->children[item];
		}
	}

	class frequent_pattern_tree_iterator final {

		std::stack<std::shared_ptr<frequent_pattern_tree_node>> item_stack_;

	public:

		frequent_pattern_tree_iterator() = default;

		explicit frequent_pattern_tree_iterator(const frequent_pattern_tree& fpt) {

			std::for_each(
				fpt.root_->children.rbegin(),
				fpt.root_->children.rend(),
				[&](const auto& map_entry) { item_stack_.push(map_entry.second); });
		}

		operator bool() const {
			return !item_stack_.empty();
		}

		std::pair<T, uint32_t> operator*() const {
			return std::make_pair(*item_stack_.top()->value, item_stack_.top()->count);
		}

		void operator++() {

			const auto current = item_stack_.top();
			item_stack_.pop();

			std::for_each(
				current->children.rbegin(),
				current->children.rend(),
				[&](const auto& map_entry) { item_stack_.push(map_entry.second); });
		}

		bool operator==(const frequent_pattern_tree_iterator& fpt) {
			return item_stack_ == fpt.item_stack_;
		}

		bool operator!=(const frequent_pattern_tree_iterator& fpt) {
			return !(*this == fpt);
		}
	};

	frequent_pattern_tree_iterator begin() const {
		return frequent_pattern_tree_iterator{*this};
	}

	frequent_pattern_tree_iterator end() const {
		return frequent_pattern_tree_iterator{};
	}

private:

	std::unordered_map<T, uint32_t> get_frequency_counts(const std::vector<std::unordered_set<T>>& itemsets) {

		std::unordered_map<T, uint32_t> frequency_counts;

		for (const auto& itemset : itemsets) {
			for (const auto& item : itemset) {
				++frequency_counts[item];
			}
		}

		return frequency_counts;
	}

	std::vector<std::set<T, std::function<bool(T, T)>>> get_ordered_itemsets(
		const std::vector<std::unordered_set<T>>& itemsets) {

		const auto frequency_counts = get_frequency_counts(itemsets);
		const auto set_comparator = [&](const T& a, const T& b) {
			return frequency_counts.at(a) == frequency_counts.at(b)
				       ? a < b
				       : frequency_counts.at(a) > frequency_counts.at(b);
		};

		std::vector<std::set<T, std::function<bool(T, T)>>> ordered_itemsets;
		ordered_itemsets.reserve(itemsets.size());

		for (const auto& itemset : itemsets) {
			std::set<T, std::function<bool(T, T)>> ordered_itemset(set_comparator);
			for (const auto& item : itemset) {
				ordered_itemset.insert(item);
			}
			ordered_itemsets.push_back(ordered_itemset);
		}

		return ordered_itemsets;
	}

	void create_link(const std::shared_ptr<frequent_pattern_tree_node>& node) {
		if (!node_links_.count(*node->value)) {
			node_links_[*node->value] = {node};
		} else {
			node_links_[*node->value].push_back(node);
		}
	}
};
