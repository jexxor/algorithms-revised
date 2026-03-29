#include <ios>
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <type_traits>

namespace io {

struct Config {
    std::size_t num_jewels;
    std::size_t num_to_keep;
};

struct Jewel {
    std::size_t id;
    int64_t value;
    int64_t weight;
};

auto ReadConfig() -> Config {
    Config config;
    std::cin >> config.num_jewels >> config.num_to_keep;
    return config;
}

auto ReadJewels(std::size_t num_jewels) -> std::vector<Jewel> {
    std::vector<Jewel> jewels(num_jewels);
    for (std::size_t i = 0; i < num_jewels; ++i) {
        jewels[i].id = i;
        std::cin >> jewels[i].value >> jewels[i].weight;
    }
    return jewels;
}

}  // namespace io

namespace binary_search {

template <typename T, typename Predicate>
    requires std::is_floating_point_v<T> && std::is_invocable_r_v<bool, Predicate, T>
auto BinarySearch(T left, T right, Predicate predicate, int iterations = 100) -> T {
    for (int i = 0; i < iterations; ++i) {
        T middle = left + (right - left) / 2;
        if (predicate(middle)) {
            left = middle;
        } else {
            right = middle;
        }
    }
    return left;
}

}  // namespace binary_search

namespace sorting {

template <typename Iterator, typename Compare>
auto Partition(Iterator first, Iterator last, Compare compare, std::mt19937& gen) -> Iterator {
    auto distance = std::distance(first, last);
    if (distance <= 1) {
        return first;
    }

    std::uniform_int_distribution<> pivot_gen(0, distance - 1);

    auto pivot_pos = std::next(first, pivot_gen(gen));
    auto pivot_value = *pivot_pos;
    std::iter_swap(first, pivot_pos);

    auto left = first + 1;
    auto right = last - 1;

    while (left <= right) {
        while (left <= right && !compare(pivot_value, *left)) {
            ++left;
        }
        while (left <= right && compare(pivot_value, *right)) {
            --right;
        }
        if (left < right) {
            std::iter_swap(left, right);
            ++left;
            --right;
        }
    }

    std::iter_swap(first, right);
    return right;
}

template <typename Iterator, typename Predicate>
auto QuickSelect(Iterator first, Iterator last, std::size_t k, Predicate predicate,
                 std::mt19937& gen) -> void {
    while (std::distance(first, last) > 1) {
        auto pivot = Partition(first, last, predicate, gen);
        auto pivot_index = static_cast<std::size_t>(std::distance(first, pivot));

        if (pivot_index == k) {
            return;
        } else if (pivot_index < k) {
            first = pivot + 1;
            k -= pivot_index + 1;
        } else {
            last = pivot;
        }
    }
}

}  // namespace sorting

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::random_device rd{};
    std::mt19937 gen(rd());

    auto config = io::ReadConfig();
    auto jewels = io::ReadJewels(config.num_jewels);

    struct RatedJewel {
        std::size_t id;
        double rating;
    };

    std::vector<RatedJewel> buffer(config.num_jewels);

    /**
    If assumed_value is an answer, there exists a set of k := num_jewels: a_1, ..., a_k, so that:
    sum (a_i.value) / sum (a_i.weight) >= assumed_value, which is equivalent to:
    sum (a_i.value) - assumed_value * sum (a_i.weight) >= 0

    If such exists, we sort an array of jewels by a_i.value - assumed_value * a_i.weight in
    descending order, and check if the sum of the first k elements is non-negative.
    */
    auto checker = [&jewels, &config, &buffer, &gen](double assumed_value) {
        for (std::size_t i = 0; i < config.num_jewels; ++i) {
            buffer[i].id = jewels[i].id;
            buffer[i].rating = jewels[i].value - assumed_value * jewels[i].weight;
        }

        sorting::QuickSelect(
            buffer.begin(), buffer.end(), config.num_to_keep,
            [](const RatedJewel& a, const RatedJewel& b) { return a.rating > b.rating; }, gen);

        double sum = 0.0;
        for (std::size_t i = 0; i < config.num_to_keep; ++i) {
            sum += buffer[i].rating;
        }

        if (sum < 0.0) {
            return false;
        }

        return true;
    };

    auto left = 0.0, right = 1'000'000'000.0;
    auto result = binary_search::BinarySearch(left, right, checker);

    checker(result);
    for (std::size_t i = 0; i < config.num_to_keep; ++i) {
        std::cout << buffer[i].id + 1 << " ";
    }
    std::cout << "\n";

    return 0;
}