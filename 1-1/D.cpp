#include <cstdlib>
#include <functional>
#include <ios>
#include <iostream>
#include <vector>
#include <stdexcept>

namespace binary_heap {

template <typename T, typename Compare = std::less<>>
class BinaryHeap {
public:
    explicit BinaryHeap(Compare compare = Compare()) : compare_(compare) {
    }

    auto Insert(const T& value) -> void {
        data_.push_back(value);
        SiftUp(data_.size() - 1);
    }

    auto Extract() -> T {
        if (IsEmpty()) {
            throw std::out_of_range("BinaryHeap is empty");
        }
        T value = data_[0];
        data_[0] = data_.back();
        data_.pop_back();
        SiftDown(0);
        return value;
    }

    auto IsEmpty() const -> bool {
        return data_.empty();
    }

private:
    auto SiftUp(size_t index) -> void {
        auto current = index;
        while (current > 0) {
            auto parent = (current - 1) / 2;
            if (compare_(data_[current], data_[parent])) {
                std::swap(data_[current], data_[parent]);
                current = parent;
            } else {
                break;
            }
        }
    }

    auto SiftDown(size_t index) -> void {
        auto current = index;
        auto size = data_.size();
        while (true) {
            auto left = 2 * current + 1;
            auto right = 2 * current + 2;
            auto smallest = current;
            if (left < size && compare_(data_[left], data_[smallest])) {
                smallest = left;
            }
            if (right < size && compare_(data_[right], data_[smallest])) {
                smallest = right;
            }
            if (smallest == current) {
                break;
            }
            std::swap(data_[current], data_[smallest]);
            current = smallest;
        }
    }

    std::vector<T> data_{};
    Compare compare_{};
};

}  // namespace binary_heap

namespace solution {

enum class OperationType { Insert = 0, Extract = 1 };

auto Process() -> void {
    std::size_t operations_count;
    std::cin >> operations_count;

    binary_heap::BinaryHeap<int, std::greater<>> heap;

    while (operations_count--) {
        int raw_command;
        std::cin >> raw_command;

        switch (static_cast<OperationType>(raw_command)) {
            case OperationType::Insert: {
                int value;
                std::cin >> value;
                heap.Insert(value);
                break;
            }
            case OperationType::Extract: {
                std::cout << heap.Extract() << '\n';
                break;
            }
            default: {
                throw std::invalid_argument("Invalid operation type");
            }
        }
    }
}

}  // namespace solution

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    solution::Process();
    return 0;
}