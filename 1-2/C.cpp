#include <cstddef>
#include <cstdint>
#include <ios>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace data_struct {

template <typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<>>
class OpenAddressingHashMap {
    enum class SlotState : std::uint8_t { Empty = 0, Occupied = 1, Deleted = 2 };

    template <typename U, typename W>
    struct Slot {
        K key;
        V value;
        SlotState state{SlotState::Empty};

        [[nodiscard]] explicit Slot(U&& key, W&& value)
            : key(std::forward<U>(key)), value(std::forward<W>(value)), state(SlotState::Occupied) {
        }
    };

public:
    [[nodiscard]] explicit OpenAddressingHashMap(std::uint8_t power_of_two_size = kDefaultPowerOfTwoSize,
                                               const Hash& hash = Hash(),
                                               const KeyEqual& key_equal = KeyEqual())
        : data_(1LL << power_of_two_size), mask_(data_.size() - 1), hash_(hash), key_equal_(key_equal) {
    }

    OpenAddressingHashMap(const OpenAddressingHashMap&) = delete;
    OpenAddressingHashMap& operator=(const OpenAddressingHashMap&) = delete;
    OpenAddressingHashMap(OpenAddressingHashMap&&) = delete;
    OpenAddressingHashMap& operator=(OpenAddressingHashMap&&) = delete;

private:
    std::size_t FindSlot(const K& key) const noexcept {
        std::size_t index = hash_(key) & mask_;
        while (data_[index].state != SlotState::Empty) {
            if (data_[index].state == SlotState::Occupied && key_equal_(data_[index].key, key)) {
                return index;
            }
            index = (index + 1) & mask_;
        }
        return index;
    }

    static constexpr std::uint8_t kDefaultPowerOfTwoSize{8};

    std::vector<Slot<K, V>> data_;
    std::size_t mask_;
    Hash hash_;
    KeyEqual key_equal_;
};

template <typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<>>
class ChainHashMap {
public:
    [[nodiscard]] explicit ChainHashMap(std::size_t bucket_count = kDefaultBucketCount,
                                        const Hash& hash = Hash(),
                                        const KeyEqual& key_equal = KeyEqual())
        : buckets_(std::max(bucket_count, kDefaultBucketCount), nullptr),
          hash_(hash),
          key_equal_(key_equal) {
    }

    ~ChainHashMap() noexcept {
        for (Node* head : buckets_) {
            while (head != nullptr) {
                Node* to_die = head;
                head = head->next;
                delete to_die;
            }
        }
    }

    ChainHashMap(const ChainHashMap&) = delete;
    ChainHashMap& operator=(const ChainHashMap&) = delete;
    ChainHashMap(ChainHashMap&&) = delete;
    ChainHashMap& operator=(ChainHashMap&&) = delete;

    template <typename U, typename W>
    void Insert(U&& key, W&& value) {
        std::size_t bucket_index = hash_(key) % buckets_.size();

        Node** indirect = &buckets_[bucket_index];
        while (*indirect != nullptr) {
            if (key_equal_((*indirect)->key, key)) {
                (*indirect)->value = std::forward<W>(value);
                return;
            }
            indirect = &(*indirect)->next;
        }

        *indirect = new Node(std::forward<U>(key), std::forward<W>(value));
        ++size_;
    }

    [[nodiscard]] const V* Find(const K& key) const noexcept {
        std::size_t bucket_index = hash_(key) % buckets_.size();

        Node* current = buckets_[bucket_index];
        while (current != nullptr) {
            if (key_equal_(current->key, key)) {
                return &current->value;
            }
            current = current->next;
        }
        return nullptr;
    }

    void Erase(const K& key) {
        std::size_t bucket_index = hash_(key) % buckets_.size();

        Node** indirect = &buckets_[bucket_index];
        while (*indirect != nullptr) {
            if (key_equal_((*indirect)->key, key)) {
                Node* to_die = *indirect;
                *indirect = (*indirect)->next;
                delete to_die;
                --size_;
                return;
            }
            indirect = &(*indirect)->next;
        }
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

private:
    struct Node {
        Node* next{nullptr};
        K key;
        V value;

        template <typename U, typename W>
        [[nodiscard]] explicit Node(U&& key, W&& value, Node* next = nullptr)
            : next(next), key(std::forward<U>(key)), value(std::forward<W>(value)) {
        }
    };

    static constexpr std::size_t kDefaultBucketCount{16};
    std::size_t size_{0};
    std::vector<Node*> buckets_;
    Hash hash_;
    KeyEqual key_equal_;
};

template <typename T>
class LinkedQueue {
    struct NodeBase {
        NodeBase* next{this};
        NodeBase* prev{this};

        [[nodiscard]] explicit NodeBase() = default;
    };

    struct Node : NodeBase {
        T data;

        template <typename U>
        [[nodiscard]] explicit Node(U&& data) : data(std::forward<U>(data)) {
        }
    };

public:
    [[nodiscard]] explicit LinkedQueue() = default;

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

    [[nodiscard]] const T& Front() const {
        if (IsEmpty()) {
            throw std::runtime_error("Queue is empty");
        }

        return static_cast<Node*>(sentinel_.next)->data;
    }

    [[nodiscard]] const T& Back() const {
        if (IsEmpty()) {
            throw std::runtime_error("Queue is empty");
        }

        return static_cast<Node*>(sentinel_.prev)->data;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return sentinel_.next == &sentinel_;
    }

private:
    NodeBase sentinel_{};
};

template <typename T>
class CircularBuffer {
public:
    [[nodiscard]] explicit CircularBuffer(std::uint8_t power_of_two_size = kDefaultPowerOfTwoSize)
        : buffer_(1LL << power_of_two_size), mask_(buffer_.size() - 1) {
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

    [[nodiscard]] bool IsEmpty() const noexcept {
        return head_ == tail_;
    }

    [[nodiscard]] const T& Front() const {
        if (IsEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        return buffer_[head_];
    }

    [[nodiscard]] const T& Back() const {
        if (IsEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        return buffer_[(tail_ + mask_) & mask_];
    }

private:
    static constexpr std::uint8_t kDefaultPowerOfTwoSize{8};

    // 2^k - 1 elements stored
    std::vector<T> buffer_;

    // Clarifying what mask is: in run-time, we will have buffer_.size() == 2^k for some k, and
    // mask_ == 2^k - 1. Therefore, (i + 1) % buffer_.size() == (i + 1) & mask_ for any i, which
    // allows us to wrap around the indices efficiently.
    std::size_t mask_;
    std::size_t head_{0};
    std::size_t tail_{0};
};

}  // namespace data_struct

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    return 0;
}