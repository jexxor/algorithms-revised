#include <iostream>
#include <vector>
#include <iterator>

namespace sorting {

namespace io {
std::vector<int> ReadInput() {
    int size;
    std::cin >> size;
    std::vector<int> result(size);
    for (int i = 0; i < size; ++i) {
        std::cin >> result[i];
    }
    return result;
}
}  // namespace io

namespace core {

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
}  // namespace core

}  // namespace sorting

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto input = sorting::io::ReadInput();
    sorting::core::Sort(input);
    for (const auto& element : input) {
        std::cout << element << " ";
    }
    std::cout << "\n";

    return 0;
}