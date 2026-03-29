#include <ios>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace data_struct {


    
template <typename T>
class LinkedQueue {
    struct NodeBase {
        NodeBase* next;
        NodeBase* prev;

        NodeBase() : next(this), prev(this) {
        }
    };

    struct Node : NodeBase {
        T data;

        template <typename U>
        explicit Node(U&& data) : data(std::forward<U>(data)) {
        }
    };

public:
    explicit LinkedQueue() = default;

    ~LinkedQueue() noexcept {
        while (!IsEmpty()) {
            Pop();
        }
    }

    LinkedQueue(const LinkedQueue&) = delete;
    LinkedQueue& operator=(const LinkedQueue&) = delete;
    LinkedQueue(LinkedQueue&&) = delete;
    LinkedQueue& operator=(LinkedQueue&&) = delete;

    template <typename U>
    void Push(U&& value) {
        Node* new_node = new Node(std::forward<U>(value));

        NodeBase* const tail = sentinel_.prev;

        new_node->next = &sentinel_;
        new_node->prev = tail;

        tail->next = new_node;
        sentinel_.prev = new_node;
    }

    void Pop() {
        if (IsEmpty()) {
            throw std::runtime_error("Queue is empty");
        }

        NodeBase* const to_die = sentinel_.next;
        NodeBase* const new_head = to_die->next;

        sentinel_.next = new_head;
        new_head->prev = &sentinel_;

        delete static_cast<Node*>(to_die);
    }

    const T& Front() const {
        if (IsEmpty()) {
            throw std::runtime_error("Queue is empty");
        }

        return static_cast<Node*>(sentinel_.next)->data;
    }

    const T& Back() const {
        if (IsEmpty()) {
            throw std::runtime_error("Queue is empty");
        }

        return static_cast<Node*>(sentinel_.prev)->data;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return sentinel_.next == &sentinel_;
    }

private:
    NodeBase sentinel_;
};

template <typename T>
class CircularBuffer {
public:
    explicit CircularBuffer(std::size_t power_of_two_size) {
        if (power_of_two_size > kMaxPowerOfTwoSize) {
            throw std::invalid_argument("Power of two size is too large");
        }

        buffer_.resize(1LL << power_of_two_size);
        mask_ = buffer_.size() - 1;
    }

    template <typename U>
    void Push(U&& value) {
        buffer_[tail_] = std::forward<U>(value);
        tail_ = (tail_ + 1) & mask_;
        if (tail_ == head_) {
            head_ = (head_ + 1) & mask_;
        }
    }

    void Pop() {
        if (IsEmpty()) {
            throw std::runtime_error("Queue is empty");
        }

        if constexpr (!std::is_trivially_destructible_v<T>) {
            buffer_[head_] = T{};
        }

        head_ = (head_ + 1) & mask_;
    }

    bool IsEmpty() const noexcept {
        return head_ == tail_;
    }

    const T& Front() const {
        if (IsEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        return buffer_[head_];
    }

    const T& Back() const {
        if (IsEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        return buffer_[(tail_ + mask_) & mask_];
    }

private:
    static constexpr std::size_t kMaxPowerOfTwoSize = 30;

    // 2^k - 1 elements stored
    std::vector<T> buffer_;

    // Clarifying what mask is: in run-time, we will have buffer_.size() == 2^k for some k, and
    // mask_ == 2^k - 1. Therefore, (i + 1) % buffer_.size() == (i + 1) & mask_ for any i, which
    // allows us to wrap around the indices efficiently.
    std::size_t mask_{};
    std::size_t head_{0};
    std::size_t tail_{0};
};

}  // namespace data_struct

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    return 0;
}