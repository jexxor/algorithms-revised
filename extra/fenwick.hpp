#pragma once

#include <array>
#include <cstddef>
#include <vector>

namespace fenwick_tree {

template <typename T, std::size_t Dims>
    requires(Dims > 0 && std::is_arithmetic_v<T>)
class [[nodiscard]] FenwickTree {
public:
    explicit FenwickTree(const std::array<std::size_t, Dims>& sizes) : sizes_(sizes) {
        std::size_t total_size = 1;
        for (auto size : sizes) {
            total_size *= (size + 1);
        }

        tree_.assign(total_size, T{});
        strides_.back() = 1;
        for (std::size_t i = Dims - 1; i > 0; --i) {
            strides_[i - 1] = strides_[i] * (sizes_[i] + 1);
        }
    }

    template <typename... Args>
    [[nodiscard]] T Query(Args... args) const {
        static_assert(sizeof...(Args) == Dims, "Number of arguments must match dimensions");
        return Query({static_cast<std::size_t>(args)...});
    }

    template <typename... Args>
    void Update(const T& delta, Args... args) {
        static_assert(sizeof...(Args) == Dims, "Number of arguments must match dimensions");
        Update({static_cast<std::size_t>(args)...}, delta);
    }

    [[nodiscard]] T Query(std::array<std::size_t, Dims> coords) const {
        return QueryImpl<0>(0, coords);
    }

    void Update(std::array<std::size_t, Dims> coords, const T& delta) {
        for (auto& index : coords) {
            ++index;
        }
        UpdateImpl<0>(0, coords, delta);
    }

private:
    static constexpr std::size_t LSB(std::size_t x) noexcept {
        return x & (~x + 1);
    }

    template <std::size_t Dim>
    void UpdateImpl(std::size_t offset, const std::array<std::size_t, Dims>& coords,
                    const T& delta) {
        if constexpr (Dim == Dims) {
            tree_[offset] += delta;
        } else {
            const std::size_t stride = strides_[Dim];
            const std::size_t index = coords[Dim];

            for (std::size_t i = index; i <= sizes_[Dim]; i += LSB(i)) {
                UpdateImpl<Dim + 1>(offset + i * stride, coords, delta);
            }
        }
    }

    template <std::size_t Dim>
    [[nodiscard]] T QueryImpl(std::size_t offset,
                              const std::array<std::size_t, Dims>& coords) const {
        if constexpr (Dim == Dims) {
            return tree_[offset];
        } else {
            T result{};
            const std::size_t stride = strides_[Dim];

            for (std::size_t i = coords[Dim]; i > 0; i -= LSB(i)) {
                result += QueryImpl<Dim + 1>(offset + i * stride, coords);
            }

            return result;
        }
    }

    std::vector<T> tree_{};
    std::array<std::size_t, Dims> sizes_{};
    std::array<std::size_t, Dims> strides_{};
};

}  // namespace fenwick_tree
