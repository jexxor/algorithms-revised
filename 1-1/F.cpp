#include <cstdint>
#include <ios>
#include <iostream>
#include <vector>
#include <algorithm>

namespace io {

struct Config {
    std::size_t data_size;
    std::size_t queries_size;
};

auto ReadConfig() -> Config {
    Config config;
    std::cin >> config.data_size >> config.queries_size;
    return config;
}

template <typename T>
auto ReadData(std::size_t data_size) -> std::vector<T> {
    std::vector<T> data(data_size);
    for (auto& element : data) {
        std::cin >> element;
    }

    return data;
}

template <typename Handler>
auto ProcessQueries(std::size_t queries_size, Handler&& handler) -> void {
    for (std::size_t i = 0; i < queries_size; ++i) {
        int64_t raw_query;
        std::cin >> raw_query;
        handler(raw_query);
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
auto FindNearest(Iterator begin, Iterator end, const T& value) -> T {
    if (begin == end) {
        throw std::invalid_argument("Empty range");
    }

    auto lower_bound = LowerBound(begin, end, value);
    if (lower_bound == begin) {
        return *lower_bound;
    }

    if (lower_bound == end) {
        return *std::prev(lower_bound);
    }

    auto previous = std::prev(lower_bound);

    using DifferenceType = decltype(*lower_bound - value);

    auto dist_left = std::abs(static_cast<DifferenceType>(*previous - value));
    auto dist_right = std::abs(static_cast<DifferenceType>(*lower_bound - value));

    if (dist_left <= dist_right) {    
        return *previous;
    }
    
    return *lower_bound;
}

}  // namespace binary_search

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto config = io::ReadConfig();

    auto data = io::ReadData<int64_t>(config.data_size);
    std::ranges::sort(data);

    io::ProcessQueries(config.queries_size, [&data](int64_t query) {
        auto closest = binary_search::FindNearest(data.begin(), data.end(), query);
        std::cout << closest << "\n";
    });

    return 0;
}