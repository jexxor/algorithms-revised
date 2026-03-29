#include <ios>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdint>

namespace data_struct {

template <typename T, typename Compare = std::less<>>
class MinStack {
public:
    explicit MinStack() = default;

    explicit MinStack(std::size_t capacity) {
        data_.reserve(capacity);
        min_offsets_.reserve(capacity);
    }

    auto Reserve(std::size_t capacity) -> void {
        data_.reserve(capacity);
        min_offsets_.reserve(capacity);
    }

    auto Push(const T& value) -> void {
        PushImpl(value);
    }

    auto Push(T&& value) -> void {
        PushImpl(std::move(value));
    }

    auto GetMin() const -> const T& {
        if (data_.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return data_[min_offsets_.back()];
    }

    auto Back() const -> const T& {
        if (data_.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return data_.back();
    }

    auto Pop() -> void {
        if (data_.empty()) {
            throw std::runtime_error("Stack is empty");
        }

        if (AreEqual(data_.back(), data_[min_offsets_.back()])) {
            min_offsets_.pop_back();
        }

        data_.pop_back();
    }

private:
    template <typename U>
    auto PushImpl(U&& value) {
        if (data_.empty() || !compare_(data_[min_offsets_.back()], value)) {
            min_offsets_.push_back(data_.size());
        }
        data_.push_back(std::forward<U>(value));
    }

    template <typename U>
    auto AreEqual(const U& a, const U& b) const -> bool {
        return !compare_(a, b) && !compare_(b, a);
    }

    std::vector<T> data_;
    std::vector<std::size_t> min_offsets_;
    Compare compare_{};
};

}  // namespace data_struct

namespace io {
enum class OperationType {
    Push = 1,
    Pop = 2,
    GetMin = 3,
};

template <typename T>
auto ProcessInput(data_struct::MinStack<T>& stack) -> void {
    std::size_t commands_amount;
    std::cin >> commands_amount;

    stack.Reserve(commands_amount);

    for (std::size_t i = 0; i < commands_amount; ++i) {
        int32_t raw_operation;
        std::cin >> raw_operation;
        auto operation = static_cast<OperationType>(raw_operation);
        switch (operation) {
            case OperationType::Push: {
                T value;
                std::cin >> value;
                stack.Push(std::move(value));
                break;
            }
            case OperationType::Pop: {
                stack.Pop();
                break;
            }
            case OperationType::GetMin: {
                std::cout << stack.GetMin() << "\n";
                break;
            }
        }
    }
}
}  // namespace io

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    data_struct::MinStack<int32_t> stack;
    io::ProcessInput(stack);
    return 0;
}