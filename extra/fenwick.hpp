#pragma once

#include <cstddef>
#include <vector>

namespace fenwick_tree {

template <typename T, std::size_t Dims = 1>
    requires(Dims > 1)
class [[nodiscard]] FenwickTree {
private:
    using SubTree = FenwickTree<T, Dims - 1>;
    std::vector<SubTree> tree_{};

public:
    template <typename... Args>
    explicit FenwickTree(std::size_t size, Args&&... args)
        : tree_(size + 1, SubTree(std::forward<Args>(args)...)) {
    }

    template <typename... Args>
    void Add(std::size_t index, Args&&... args) {
        for (++index; index < tree_.size(); index += LSB(index)) {
            tree_[index].Add(std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    [[nodiscard]] T PrefixSum(std::size_t index, Args&&... args) const {
        T result{};
        for (++index; index > 0; index -= LSB(index)) {
            result += tree_[index].PrefixSum(std::forward<Args>(args)...);
        }
        return result;
    }

private:
    [[nodiscard]] constexpr static std::size_t LSB(std::size_t x) noexcept {
        return x & (~x + 1);
    }
};

template <typename T>
class [[nodiscard]] FenwickTree<T, 1> {
private:
    std::vector<T> tree_{};

public:
    explicit FenwickTree(std::size_t size) : tree_(size + 1, T{}) {
    }

    template <typename U>
    void Add(std::size_t index, U&& value) {
        for (++index; index < tree_.size(); index += LSB(index)) {
            tree_[index] += std::forward<U>(value);
        }
    }

    [[nodiscard]] T PrefixSum(std::size_t index) const {
        T result{};
        for (++index; index > 0; index -= LSB(index)) {
            result += tree_[index];
        }
        return result;
    }

private:
    [[nodiscard]] constexpr static std::size_t LSB(std::size_t x) noexcept {
        return x & (~x + 1);
    }
};

}  // namespace fenwick_tree