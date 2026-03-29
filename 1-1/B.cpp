#include <concepts>
#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>

namespace countsort {
namespace io {

template <typename T>
auto ReadUntilEof() {
    std::vector<T> result;
    T x;
    while (std::cin >> x) {
        result.push_back(x);
    }
    return result;
}

template <typename T>
void PrintVector(const std::vector<T>& vec) {
    for (const auto& x : vec) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
}

}  // namespace io

namespace core {
template <std::integral T>
void CountSort(std::vector<T>& arr) {
    auto size = arr.size();
    if (size <= 1) {
        return;
    }

    auto [min_it, max_it] = std::minmax_element(arr.begin(), arr.end());
    auto min = *min_it;
    auto max = *max_it;

    std::vector<std::uint64_t> count(static_cast<std::size_t>(max - min + 1), 0);
    for (const auto& x : arr) {
        ++count[static_cast<std::size_t>(x - min)];
    }

    std::size_t index = 0;
    for (std::size_t i = 0; i < count.size(); ++i) {
        for (std::size_t j = 0; j < count[i]; ++j) {
            arr[index++] = static_cast<T>(min + static_cast<T>(i));
        }
    }
}

}  // namespace core
}  // namespace countsort

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto vector = countsort::io::ReadUntilEof<int64_t>();
    countsort::core::CountSort(vector);
    countsort::io::PrintVector(vector);

    return 0;
}