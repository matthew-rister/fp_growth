#pragma once

#include <algorithm>
#include <memory>
#include <stack>

#include "frequent_pattern_tree_node.h"

template <typename T>
class frequent_pattern_tree_iterator final {

	std::stack<std::shared_ptr<frequent_pattern_tree_node<T>>> item_stack_;

public:

	frequent_pattern_tree_iterator() = default;

	explicit frequent_pattern_tree_iterator(const std::shared_ptr<frequent_pattern_tree_node<T>>& node) {

		std::for_each(
			node->children.rbegin(),
			node->children.rend(),
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

	bool operator==(const frequent_pattern_tree_iterator& iterator) {
		return item_stack_ == iterator.item_stack_;
	}

	bool operator!=(const frequent_pattern_tree_iterator& iterator) {
		return !(*this == iterator);
	}
};
