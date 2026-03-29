#include <ios>
#include <iostream>
#include <cstdint>

namespace binary_search {

template <typename T, typename Predicate>
    requires std::is_integral_v<T> && std::is_invocable_r_v<bool, Predicate, T>
T BinarySearch(T left, T right, Predicate predicate) {
    while (left < right) {
        T middle = left + (right - left) / 2;
        if (predicate(middle)) {
            right = middle;
        } else {
            left = middle + 1;
        }
    }
    return left;
}

}  // namespace binary_search

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int64_t width, height, amount;
    std::cin >> width >> height >> amount;

    auto predicate = [width, height, amount](int64_t size) {
        return (size / width) * (size / height) >= amount;
    };

    int64_t left = 1, right = 1;
    while (!predicate(right)) {
        right *= 2;
    }
    std::cout << binary_search::BinarySearch(left, right, predicate) << "\n";

    return 0;
}