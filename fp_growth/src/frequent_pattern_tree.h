#pragma once

#include <map>
#include <memory>
#include <set>
#include <queue>
#include <utility>
#include <vector>

class frequent_pattern_tree_iterator;

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
	};

	std::shared_ptr<frequent_pattern_tree_node> root_ = std::make_shared<frequent_pattern_tree_node>(T{});
	std::map<T, std::vector<std::shared_ptr<frequent_pattern_tree_node>>> item_links_;

public:

	class frequent_pattern_tree_iterator final {

		std::queue<std::shared_ptr<frequent_pattern_tree_node>> queue_;

	public:

		explicit frequent_pattern_tree_iterator(const frequent_pattern_tree fpt) {
			for (const auto& [_, child] : fpt.root_->children) {
				queue_.push(child);
			}
		}

		operator bool() const {
			return !queue_.empty();
		}

		std::pair<T, uint32_t> operator*() const {
			if (queue_.empty()) throw std::exception{"Attempted to dereference an invalid iterator"};
			return std::make_pair(queue_.front()->value, queue_.front()->count);
		}

		void operator++() {
			for (const auto& [_, child] : queue_.front()->children) {
				queue_.push(child);
			}
			queue_.pop();
		}
	};

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
				create_item_link(iterator->children[item]);
			} else {
				++iterator->children[item]->count;
			}
			iterator = iterator->children[item];
		}
	}

private:

	void create_item_link(const std::shared_ptr<frequent_pattern_tree_node>& node) {
		if (!item_links_.count(node->value)) {
			item_links_[node->value] = {node};
		} else {
			item_links_[node->value].push_back(node);
		}
	}
};
