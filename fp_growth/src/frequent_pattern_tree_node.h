#pragma once

#include <map>
#include <memory>

template <typename T>
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
