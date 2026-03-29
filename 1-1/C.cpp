#include <iostream>
#include <vector>

namespace io {
template <typename T>
auto ReadInput() -> std::vector<T> {
    int size;
    std::cin >> size;
    std::vector<T> result(size);
    for (int i = 0; i < size; ++i) {
        std::cin >> result[i];
    }
    return result;
}
}  // namespace io

namespace sorting {

template <typename T, typename Comparator, typename Iterator>
void Merge(Iterator begin, Iterator middle, Iterator end, Iterator buffer, Comparator compare,
           size_t& inversion_count) {
    auto left_it = begin;
    auto right_it = middle;
    auto buffer_it = buffer;

    while (left_it != middle && right_it != end) {
        if (compare(*left_it, *right_it)) {
            *buffer_it++ = std::move(*left_it++);
        } else {
            *buffer_it++ = std::move(*right_it++);
            inversion_count += std::distance(left_it, middle);
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
void SortImpl(Iterator begin, Iterator end, Iterator buffer, Comparator compare,
              size_t& inversion_count) {
    auto size = std::distance(begin, end);
    if (size <= 1) {
        return;
    }

    auto vector_middle = std::next(begin, std::distance(begin, end) / 2);
    auto buffer_middle = std::next(buffer, std::distance(begin, vector_middle));

    SortImpl<T>(begin, vector_middle, buffer, compare, inversion_count);
    SortImpl<T>(vector_middle, end, buffer_middle, compare, inversion_count);
    Merge<T>(begin, vector_middle, end, buffer, compare, inversion_count);
}

// Returns number of inversions in the array
template <typename T, typename Comparator = std::less<>>
auto Sort(std::vector<T>& data) -> size_t {
    if (data.size() <= 1) {
        return 0;
    }

    std::vector<T> buffer(data.size());
    size_t inversion_count = 0;
    SortImpl<T>(data.begin(), data.end(), buffer.begin(), Comparator(), inversion_count);
    return inversion_count;
}

}  // namespace sorting

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto data = io::ReadInput<int>();
    auto inversion_count = sorting::Sort(data);
    std::cout << inversion_count << "\n";

    return 0;
}