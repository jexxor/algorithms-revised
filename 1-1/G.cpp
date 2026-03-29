#include <algorithm>
#include <cstddef>
#include <ios>
#include <iostream>

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

} // namespace binary_search

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t copies_left;
    std::size_t first_speed;
    std::size_t second_speed;
    std::cin >> copies_left >> first_speed >> second_speed;

    auto predicate = [copies_left, first_speed, second_speed](std::size_t time) {
        const auto first_copy_time = std::min(first_speed, second_speed);
        if (copies_left == 0) {
            return true;
        }

        if (time < first_copy_time) {
            return false;
        }
        
        auto time_after_first = time - first_copy_time;
        return 1 + time_after_first / first_speed + time_after_first / second_speed >= copies_left;
    };

    std::size_t left = 1, right = 1;
    while (!predicate(right)) {
        right *= 2;
    }
    std::cout << binary_search::BinarySearch(left, right, predicate) << "\n";

    return 0;
}
