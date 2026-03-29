#include <ios>
#include <iostream>
#include <vector>
#include <cstdint>
#include <iterator>

namespace io {

template <typename T>
auto ReadInput() -> std::vector<T> {
    std::size_t vector_size;
    std::cin >> vector_size;

    std::vector<T> result(vector_size);
    for (auto& element : result) {
        std::cin >> element;
    }

    return result;
}

template <typename Handler>
auto ProcessQueries(Handler&& handler) -> void {
    std::size_t queries_count;
    if (!(std::cin >> queries_count)) {
        return;
    }

    for (std::size_t i = 0; i < queries_count; ++i) {
        int64_t left, right;
        if (std::cin >> left >> right) {
            handler(left, right);
        }
    }
}

}  // namespace io

namespace binary_search {

template <typename T, typename Iterator>
auto LowerBound(Iterator begin, Iterator end, const T& value) -> Iterator {
    while (begin < end) {
        auto middle = begin + (end - begin) / 2;
        if (*middle < value) {
            begin = middle + 1;
        } else {
            end = middle;
        }
    }

    return begin;
}

template <typename T, typename Iterator>
auto UpperBound(Iterator begin, Iterator end, const T& value) -> Iterator {
    while (begin < end) {
        auto middle = begin + (end - begin) / 2;
        if (*middle <= value) {
            begin = middle + 1;
        } else {
            end = middle;
        }
    }

    return begin;
}

}  // namespace binary_search

namespace sorting {

template <typename T, typename Comparator, typename Iterator>
void Merge(Iterator begin, Iterator middle, Iterator end, Iterator buffer, Comparator compare) {
    auto left_it = begin;
    auto right_it = middle;
    auto buffer_it = buffer;

    while (left_it != middle && right_it != end) {
        if (compare(*left_it, *right_it)) {
            *buffer_it++ = std::move(*left_it++);
        } else {
            *buffer_it++ = std::move(*right_it++);
        }
    }

    while (left_it != middle) {
        *buffer_it++ = std::move(*left_it++);
    }

    while (right_it != end) {
        *buffer_it++ = std::move(*right_it++);
    }

    std::move(buffer, buffer_it, begin);
}

template <typename T, typename Iterator, typename Comparator>
void SortImpl(Iterator begin, Iterator end, Iterator buffer, Comparator compare) {
    auto size = std::distance(begin, end);
    if (size <= 1) {
        return;
    }

    auto vector_middle = std::next(begin, std::distance(begin, end) / 2);
    auto buffer_middle = std::next(buffer, std::distance(begin, vector_middle));

    SortImpl<T>(begin, vector_middle, buffer, compare);
    SortImpl<T>(vector_middle, end, buffer_middle, compare);
    Merge<T>(begin, vector_middle, end, buffer, compare);
}

template <typename T, typename Comparator = std::less<>>
void Sort(std::vector<T>& data) {
    if (data.size() <= 1) {
        return;
    }
    std::vector<T> buffer(data.size());
    SortImpl<T>(data.begin(), data.end(), buffer.begin(), Comparator());
}

}  // namespace sorting

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto input_vector = io::ReadInput<int>();
    sorting::Sort(input_vector);
    io::ProcessQueries([&input_vector](int64_t left, int64_t right) {
        auto left_it = binary_search::LowerBound(input_vector.begin(), input_vector.end(), left);
        auto right_it = binary_search::UpperBound(input_vector.begin(), input_vector.end(), right);
        std::cout << std::distance(left_it, right_it) << "\n";
    });
}