#pragma once

#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <utility>
#include <vector>

template <typename T>
class frequent_pattern_tree final {

	struct frequent_pattern_tree_node final {

		T value;
		uint32_t count = 1;
		std::shared_ptr<frequent_pattern_tree_node> parent;
		std::map<T, std::shared_ptr<frequent_pattern_tree_node>> children;

		explicit frequent_pattern_tree_node(T value, std::shared_ptr<frequent_pattern_tree_node> parent = nullptr)
			: value{std::move(value)},
			  parent{std::move(parent)} {}

		friend std::ostream& operator<<(std::ostream& os, const frequent_pattern_tree_node& node) {
			os << node.value << ":" << node.count << " ";
			for (const auto& [_, child] : node.children) {
				os << *child;
			}
			return os;
		}
	};

public:

	explicit frequent_pattern_tree(const std::vector<std::set<T>>& itemsets = {}) {
		for (const auto& itemset : itemsets) {
			insert(itemset);
		}
	}

	void insert(const std::set<T>& itemset) {

		auto iterator = root_;

		for (const auto& item : itemset) {
			if (!iterator->children.count(item)) {
				iterator->children[item] = std::make_shared<frequent_pattern_tree_node>(item, iterator);
			} else {
				++iterator->children[item]->count;
			}
			iterator = iterator->children[item];
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const frequent_pattern_tree& fpt) {
		for (const auto& [_, child] : fpt.root_->children) {
			os << *child;
		}
		return os;
	}

private:

	std::shared_ptr<frequent_pattern_tree_node> root_ = std::make_shared<frequent_pattern_tree_node>(T{});
};
