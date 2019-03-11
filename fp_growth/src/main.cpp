#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

template <typename T>
class fp_tree final {

	struct fp_tree_node final {

		T value;
		uint32_t count = 1;
		std::shared_ptr<fp_tree_node> parent;
		std::unordered_map<T, std::shared_ptr<fp_tree_node>> children;

		explicit fp_tree_node(const T& value, const std::shared_ptr<fp_tree_node> parent)
			: value{value},
			  parent{parent} {}

		friend std::ostream& operator<<(std::ostream& os, const fp_tree_node& node) {
			os << node.value << ":" << node.count << " ";
			for (const auto& [_, child] : children) {
				os << child;
			}
			return os;
		}
	};

public:
	explicit fp_tree(const std::vector<std::vector<T>>& itemsets) {
		for (const auto& itemset : itemsets) {
			insert(itemset);
		}
	}

	void insert(const std::vector<T>& itemset) {

		const auto iterator = root_;

		for (const auto& item : itemset) {

			if (!iterator->children.count(item)) {
				iterator->children[item] = std::make_shared<fp_tree_node>(item, iterator);
			} else {
				++iterator->children[item];
			}

			iterator = iterator->children[item];
		}
	}

	friend std::ostream& operator<<(std::ostream& os) {
		std::stack<fp_tree_node> stack;

		return os << *root_;
	}

private:
	std::shared_ptr<fp_tree_node> root_;
};

int main() {
	return 0;
}
