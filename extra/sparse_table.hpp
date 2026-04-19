#pragma once

#include <bit>
#include <cstddef>
#include <functional>
#include <stdexcept>

namespace sparse_table {

template <typename T, typename Op = std::less<>>
class [[nodiscard]] SparseTable {
public:
    explicit SparseTable(const std::vector<T>& data) : data_size_(data.size()) {
        if (data.empty()) {
            return;
        }

        const std::size_t max_log = std::bit_width(data_size_);

        table_.assign(max_log * data_size_, T{});
        for (std::size_t i = 0; i < data_size_; ++i) {
            table_[i] = data[i];
        }

        for (std::size_t log = 1; log < max_log; ++log) {
            const std::size_t offset = log * data_size_;
            const std::size_t previous_offset = (log - 1) * data_size_;
            const std::size_t step = 1ULL << log;
            const std::size_t half_step = step >> 1;

            for (std::size_t i = 0; i + step <= data_size_; ++i) {
                table_[offset + i] =
                    Select(table_[previous_offset + i], table_[previous_offset + i + half_step]);
            }
        }
    }

    [[nodiscard]] T Query(std::size_t left, std::size_t right) const {
        if (left >= right) {
            throw std::invalid_argument("Invalid query range");
        }

        const std::size_t length = right - left;
        const std::size_t log = static_cast<std::size_t>(std::bit_width(length) - 1);
        const std::size_t offset = log * data_size_;
        const std::size_t step = 1ULL << log;

        return Select(table_[offset + left], table_[offset + right - step]);
    }

private:
    static T Select(const T& a, const T& b) {
        return Op{}(a, b) ? a : b;
    }

    std::vector<T> table_{};
    std::size_t data_size_{0};
};

}  // namespace sparse_table
