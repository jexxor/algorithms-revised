#pragma once

#include <array>
#include <cstddef>
#include <vector>

namespace fenwick_tree {

template <typename T, std::size_t Dims>
class [[nodiscard]] FenwickTree {
    static_assert(Dims > 0, "Fenwick tree must have at least one dimension");

public:
    explicit FenwickTree(const std::array<std::size_t, Dims>& sizes) : sizes_(sizes) {
        std::size_t total_size = 1;
        for (auto size : sizes) {
            total_size *= (size + 1);
        }

        tree_.assign(total_size, T{});
        strides_[Dims - 1] = 1;
        for (std::size_t i = Dims - 1; i > 0; --i) {
            strides_[i - 1] = strides_[i] * (sizes_[i] + 1);
        }
    }

    template <typename... Args>
    void Update(const T& delta, Args... args) {
        static_assert(sizeof...(Args) == Dims,
                      "Number of indices must match the number of dimensions");
        Update({static_cast<std::size_t>(args)...}, delta);
    }

    void Update(std::array<std::size_t, Dims> indices, const T& delta) {
        for (auto& index : indices) {
            ++index;
        }
        RecursiveUpdate(0, 0, indices, delta);
    }

    template <typename... Args>
    [[nodiscard]] T Query(Args... args) const {
        static_assert(sizeof...(Args) == Dims,
                      "Number of indices must match the number of dimensions");
        return Query({static_cast<std::size_t>(args)...});
    }

    [[nodiscard]] T Query(std::array<std::size_t, Dims> indices) const {
        for (auto& index : indices) {
            ++index;
        }
        return RecursiveQuery(0, 0, indices);
    }

private:
    static constexpr std::size_t LSB(std::size_t x) noexcept {
        return x & (~x + 1);
    }

    T RecursiveQuery(std::size_t dims, std::size_t offset,
                     std::array<std::size_t, Dims>& coords) const {
        if (dims == Dims) {
            return tree_[offset];
        }

        T result{};
        for (std::size_t i = coords[dims]; i > 0; i -= LSB(i)) {
            result += RecursiveQuery(dims + 1, offset + strides_[dims] * i, coords);
        }

        return result;
    }

    void RecursiveUpdate(std::size_t dims, std::size_t offset,
                         std::array<std::size_t, Dims>& coords, const T& delta) {
        if (dims == Dims) {
            tree_[offset] += delta;
            return;
        }

        for (std::size_t i = coords[dims]; i < sizes_[dims] + 1; i += LSB(i)) {
            RecursiveUpdate(dims + 1, offset + strides_[dims] * i, coords, delta);
        }
    }

    std::vector<T> tree_{};
    std::array<std::size_t, Dims> sizes_{};
    std::array<std::size_t, Dims> strides_{};
};

}  // namespace fenwick_tree
