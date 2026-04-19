#pragma once

#include <bit>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace sparse_table {

template <typename T, typename Op = std::less<>>
class [[nodiscard]] SparseTable {
public:
    explicit SparseTable(std::vector<T> data, const Op& op = Op{})
        : data_size_(data.size()), op_(op) {
        if (data.empty()) {
            throw std::invalid_argument("Data cannot be empty");
        }

        const std::size_t max_log = std::bit_width(data_size_);

        table_.reserve(max_log * data_size_);
        table_.insert(table_.end(), std::make_move_iterator(data.begin()),
                      std::make_move_iterator(data.end()));

        for (std::size_t log = 1; log < max_log; ++log) {
            const std::size_t previous_offset = (log - 1) * data_size_;
            const std::size_t step = 1ULL << (log - 1);

            for (std::size_t i = 0; i < data_size_; ++i) {
                if (i + step < data_size_) {
                    table_.push_back(Select(table_[previous_offset + i],
                                             table_[previous_offset + i + step]));
                } else {
                    table_.push_back(table_[previous_offset + i]);
                }
            }
        }
    }

    [[nodiscard]] const T& Query(std::size_t left, std::size_t right) const noexcept {
        assert(left < right && right <= data_size_);

        const std::size_t length = right - left;
        const std::size_t log = static_cast<std::size_t>(std::bit_width(length) - 1);
        const std::size_t offset = log * data_size_;
        const std::size_t step = 1ULL << log;

        return Select(table_[offset + left], table_[offset + right - step]);
    }

private:
    const T& Select(const T& a, const T& b) const noexcept {
        return op_(a, b) ? a : b;
    }

    std::vector<T> table_{};
    std::size_t data_size_{0};
    [[no_unique_address]] Op op_{};
};

}  // namespace sparse_table
