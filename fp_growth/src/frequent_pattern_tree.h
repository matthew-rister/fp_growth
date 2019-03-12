#pragma once

#include <map>
#include <memory>
#include <set>
#include <stack>
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
	};

	std::shared_ptr<frequent_pattern_tree_node> root_ = std::make_shared<frequent_pattern_tree_node>(T{});
	std::map<T, std::vector<std::shared_ptr<frequent_pattern_tree_node>>> item_links_;

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
			return std::make_pair(item_stack_.top()->value, item_stack_.top()->count);
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

	void create_link(const std::shared_ptr<frequent_pattern_tree_node>& node) {
		if (!item_links_.count(node->value)) {
			item_links_[node->value] = {node};
		} else {
			item_links_[node->value].push_back(node);
		}
	}
};
