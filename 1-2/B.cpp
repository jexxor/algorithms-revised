#include <concepts>
#include <ios>
#include <iostream>
#include <vector>

namespace data_struct {

template <typename T>
struct CountPair {
    std::size_t count;
    T value;
};

template <std::equality_comparable T>
class CountStack {
public:
    CountStack() = default;

    void Push(const T& value) {
        if (data_.empty() || data_.back().value != value) {
            data_.push_back({1, value});
        } else {
            ++data_.back().count;
        }
    }

    void Push(T&& value) {
        if (data_.empty() || data_.back().value != value) {
            data_.push_back({1, std::move(value)});
        } else {
            ++data_.back().count;
        }
    }

    const T& Top() const {
        if (data_.empty()) {
            throw std::runtime_error("Stack is empty");
        }

        return data_.back().value;
    }

    const std::size_t& TopCount() const {
        if (data_.empty()) {
            throw std::runtime_error("Stack is empty");
        }

        return data_.back().count;
    }

    void PopPair() {
        if (data_.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        data_.pop_back();
    }

    bool IsEmpty() const {
        return data_.empty();
    }

private:
    std::vector<CountPair<T>> data_{};
};

}  // namespace data_struct

namespace balls_solution {
void HandleInput() {
    std::size_t balls_count{};
    std::cin >> balls_count;
    std::size_t collapsed_balls_count{};

    using Color = std::size_t;
    data_struct::CountStack<Color> stack;
    for (std::size_t i = 0; i < balls_count; ++i) {
        Color color;
        std::cin >> color;

        if (!stack.IsEmpty() && stack.Top() != color) {
            if (stack.TopCount() >= 3) {
                collapsed_balls_count += stack.TopCount();
                stack.PopPair();
            }
        }

        stack.Push(color);
    }

    if (!stack.IsEmpty() && stack.TopCount() >= 3) {
        collapsed_balls_count += stack.TopCount();
    }


    std::cout << collapsed_balls_count << "\n";
}
}  // namespace balls_solution

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    balls_solution::HandleInput();
    return 0;
}