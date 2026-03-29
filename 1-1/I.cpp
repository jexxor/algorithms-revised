#include <ios>
#include <iostream>
#include <cmath>
#include <type_traits>

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

template <typename T, typename Predicate>
    requires std::is_floating_point_v<T> && std::is_invocable_r_v<bool, Predicate, T>
T BinarySearch(T left, T right, Predicate predicate, int iterations = 100) {
    for (int i = 0; i < iterations; ++i) {
        T middle = left + (right - left) / 2;
        if (predicate(middle)) {
            right = middle;
        } else {
            left = middle;
        }
    }
    return left;
}

}  // namespace binary_search

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    double constant;
    std::cin >> constant;
    auto predicate = [constant](double x) { return x * x + std::sqrt(x) >= constant; };
    double result = binary_search::BinarySearch(0.0, constant, predicate);

    std::cout.precision(10);
    std::cout << std::fixed << result << "\n";
    return 0;
}