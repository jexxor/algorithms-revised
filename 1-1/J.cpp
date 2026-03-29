#include <cmath>
#include <ios>
#include <iostream>
#include <type_traits>
#include <cstdint>

namespace ternary_search {

template <typename T, typename U>
    requires std::is_floating_point_v<T>
T TernaryMin(T left, T right, U compute, int iterations = 100) {
    for (int i = 0; i < iterations; ++i) {
        T middle1 = left + (right - left) / 3;
        T middle2 = right - (right - left) / 3;
        if (compute(middle1) < compute(middle2)) {
            right = middle2;
        } else {
            left = middle1;
        }
    }
    return left;
}
}  // namespace ternary_search

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int32_t field_speed{}, jungle_speed{};
    std::cin >> field_speed >> jungle_speed;

    double field_jungle_split{};
    std::cin >> field_jungle_split;

    auto compute_time = [field_speed, jungle_speed, field_jungle_split](double x) {
        constexpr std::pair<double, double> kStart{0.0, 1.0};
        constexpr std::pair<double, double> kEnd{1.0, 0.0};

        auto field_length = std::hypot(kStart.second - field_jungle_split, kStart.first + x);
        auto jungle_length = std::hypot(kEnd.second - field_jungle_split, kEnd.first - x);
        return field_length / field_speed + jungle_length / jungle_speed;
    };

    double result = ternary_search::TernaryMin(0.0, 1.0, compute_time);
    std::cout.precision(10);
    std::cout << std::fixed << result << "\n";
    return 0;
}