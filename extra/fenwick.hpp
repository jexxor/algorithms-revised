#pragma once

#include <vector>
#include <cstddef>

namespace fenwick_tree {

template <typename T>
class [[nodiscard]] FenwickTree {
public:
    explicit FenwickTree(std::size_t size) : tree_(size + 1, T()) {
    }

    [[nodiscard]] T Query(std::size_t index) const {
        ++index;
        T result = T();
        while (index > 0) {
            result += tree_[index];
            index -= LSB(index);
        }
        return result;
    }

    [[nodiscard]] T Query(std::size_t left, std::size_t right) const {
        if (left > right) {
            return T();
        }

        auto res = Query(right);
        if (left > 0) {
            res -= Query(left - 1);
        }
        return res;
    }

    void Update(std::size_t index, const T& value) {
        std::size_t i = index + 1;
        while (i < tree_.size()) {
            tree_[i] += value;
            i += LSB(i);
        }
    }

private:
    [[nodiscard]] static std::size_t LSB(std::size_t index) noexcept {
        return index & (~index + 1);
    }

    std::vector<T> tree_;
};

}  // namespace fenwick_tree
