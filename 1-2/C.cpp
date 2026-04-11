#include <cstddef>
#include <cstdint>
#include <ios>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

namespace data_struct {

template <typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<>>
class OpenAddressingHashMap {
    enum class SlotState : std::uint8_t { Empty = 0, Occupied = 1, Deleted = 2 };

    struct Slot {
        K key;
        V value;
        SlotState state{SlotState::Empty};

        template <typename U, typename W>
        Slot(U&& key, W&& value)
            : key(std::forward<U>(key)), value(std::forward<W>(value)), state(SlotState::Occupied) {
        }

        Slot() = default;
    };

public:
    [[nodiscard]] explicit OpenAddressingHashMap(
        std::uint8_t power_of_two_size = kDefaultPowerOfTwoSize, const Hash& hash = Hash(),
        const KeyEqual& key_equal = KeyEqual())
        : data_(1LL << power_of_two_size),
          mask_(data_.size() - 1),
          hash_(hash),
          key_equal_(key_equal) {
    }

    OpenAddressingHashMap(const OpenAddressingHashMap&) = delete;
    OpenAddressingHashMap& operator=(const OpenAddressingHashMap&) = delete;
    OpenAddressingHashMap(OpenAddressingHashMap&&) = delete;
    OpenAddressingHashMap& operator=(OpenAddressingHashMap&&) = delete;

    [[nodiscard]] const V* Find(const K& key) const noexcept {
        std::size_t index = AcquireSlot(key);
        if (data_[index].state == SlotState::Occupied) {
            return &data_[index].value;
        }
        return nullptr;
    }

    template <typename U, typename W>
    void Insert(U&& key, W&& value) {
        if (ShouldRehashNext()) {
            Rehash();
        }

        std::size_t index = AcquireSlot(key);
        if (data_[index].state != SlotState::Occupied) {
            data_[index] = Slot(std::forward<U>(key), std::forward<W>(value));
            ++size_;
        } else {
            data_[index].value = std::forward<W>(value);
        }
    }

    bool Erase(const K& key) {
        std::size_t index = AcquireSlot(key);
        if (data_[index].state == SlotState::Occupied) {
            data_[index].state = SlotState::Deleted;
            if constexpr (!std::is_trivially_destructible_v<V>) {
                data_[index].value = V{};
            }

            if constexpr (!std::is_trivially_destructible_v<K>) {
                data_[index].key = K{};
            }

            --size_;
            return true;
        }
        return false;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

private:
    // if found, returns index of the slot with the key;
    // otherwise, returns index of the first deleted slot or empty slot
    [[nodiscard]] std::size_t AcquireSlot(const K& key) const noexcept {
        std::size_t index = hash_(key) & mask_;
        std::optional<std::size_t> first_deleted;

        while (data_[index].state != SlotState::Empty) {
            if (data_[index].state == SlotState::Occupied && key_equal_(data_[index].key, key)) {
                return index;
            }
            if (data_[index].state == SlotState::Deleted && !first_deleted.has_value()) {
                first_deleted = index;
            }
            index = (index + 1) & mask_;
        }
        return first_deleted.value_or(index);
    }

    void Rehash() {
        std::size_t new_capacity = data_.size() * 2;
        std::vector<Slot> old_data;
        
        data_.swap(old_data);
        data_.resize(new_capacity);
        mask_ = new_capacity - 1;

        for (const auto& slot : old_data) {
            if (slot.state == SlotState::Occupied) {
                std::size_t index = hash_(slot.key) & mask_;

                while (data_[index].state == SlotState::Occupied) {
                    index = (index + 1) & mask_;
                }

                data_[index] = std::move(slot);
            }
        }
    }

    // LoadFactorAfterInsert = (size_ + 1) / data_.size() > 0.7, which is equivalent to
    // 10 * (size_ + 1) > 7 * data_.size()
    [[nodiscard]] bool ShouldRehashNext() const noexcept {
        constexpr std::int64_t kThresholdNumerator{60};
        constexpr std::int64_t kThresholdDenominator{100};

        return (size_ + 1) * kThresholdDenominator > data_.size() * kThresholdNumerator;
    }

    static constexpr std::uint8_t kDefaultPowerOfTwoSize{16};

    std::vector<Slot> data_;
    std::size_t size_{0};
    std::size_t mask_;
    Hash hash_;
    KeyEqual key_equal_;
};

template <typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<>>
class ChainHashMap {
    struct Node {
        Node* next{nullptr};
        K key;
        V value;

        template <typename U, typename W>
        Node(U&& key, W&& value, Node* next = nullptr)
            : next(next), key(std::forward<U>(key)), value(std::forward<W>(value)) {
        }
    };

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

        NodeBase() = default;
    };

    struct Node : NodeBase {
        T data;

        template <typename U>
        Node(U&& data) : data(std::forward<U>(data)) {
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
    static constexpr std::uint8_t kDefaultPowerOfTwoSize{16};

    // 2^k - 1 elements stored
    std::vector<T> buffer_;

    // Clarifying what mask is: in run-time, we will have buffer_.size() == 2^k for some k, and
    // mask_ == 2^k - 1. Therefore, (i + 1) % buffer_.size() == (i + 1) & mask_ for any i, which
    // allows us to wrap around the indices efficiently.
    std::size_t mask_;
    std::size_t head_{0};
    std::size_t tail_{0};
};

template <typename T>
class CircularDeque {
public:
    [[nodiscard]] explicit CircularDeque(std::uint8_t power_of_two_size = kDefaultPowerOfTwoSize)
        : buffer_(1LL << power_of_two_size), mask_(buffer_.size() - 1) {
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return head_ == tail_;
    }

    [[nodiscard]] std::size_t Size() const noexcept {
        return (tail_ - head_) & mask_;
    }

    template <typename U>
    void PushTail(U&& value) {
        buffer_[tail_] = std::forward<U>(value);
        tail_ = (tail_ + 1) & mask_;
        if (tail_ == head_) {
            ResizeFull();
        }
    }

    template <typename U>
    void PushHead(U&& value) {
        head_ = (head_ + mask_) & mask_;
        buffer_[head_] = std::forward<U>(value);
        if (tail_ == head_) {
            ResizeFull();
        }
    }

    [[nodiscard]] const T& Front() const {
        if (IsEmpty()) {
            throw std::runtime_error("Deque is empty");
        }
        return buffer_[head_];
    }

    [[nodiscard]] const T& Back() const {
        if (IsEmpty()) {
            throw std::runtime_error("Deque is empty");
        }
        return buffer_[(tail_ + mask_) & mask_];
    }

    void PopHead() {
        if (IsEmpty()) {
            throw std::runtime_error("Deque is empty");
        }

        if constexpr (!std::is_trivially_destructible_v<T>) {
            buffer_[head_] = T{};
        }

        head_ = (head_ + 1) & mask_;
    }

    void PopTail() {
        if (IsEmpty()) {
            throw std::runtime_error("Deque is empty");
        }

        if constexpr (!std::is_trivially_destructible_v<T>) {
            buffer_[(tail_ + mask_) & mask_] = T{};
        }

        tail_ = (tail_ + mask_) & mask_;
    }

private:
    void ResizeFull() {
        std::vector<T> new_buffer(buffer_.size() * 2);
        std::size_t new_mask = new_buffer.size() - 1;

        for (std::size_t i = 0; i < buffer_.size(); ++i) {
            new_buffer[i] = std::move(buffer_[(head_ + i) & mask_]);
        }

        buffer_.swap(new_buffer);
        mask_ = new_mask;
        head_ = 0;
        tail_ = buffer_.size() / 2;
    }

    static constexpr std::uint8_t kDefaultPowerOfTwoSize{16};

    // 2^k - 1 elements stored
    std::vector<T> buffer_;

    std::size_t mask_;
    std::size_t head_{0};
    std::size_t tail_{0};
};

}  // namespace data_struct

namespace astrocity_solution {
enum class EventType : std::uint8_t {
    PushTail = 1,
    PopHead = 2,
    PopTail = 3,
    DistanceFromHead = 4,
    GetHead = 5
};

std::istream& operator>>(std::istream& is, EventType& event_type) {
    std::int32_t event_type_value;
    if (is >> event_type_value) {
        event_type = static_cast<EventType>(event_type_value);
    }
    return is;
}

void HandleInput() {
    std::size_t events_count{};
    std::cin >> events_count;

    using IdenifierType = std::size_t;
    data_struct::CircularDeque<IdenifierType> deque;
    data_struct::OpenAddressingHashMap<IdenifierType, std::size_t> positions;

    struct {
        std::size_t tail_clock{};
        std::size_t head_clock{};
    } state;

    for (std::size_t i = 0; i < events_count; ++i) {
        EventType event_type{};
        std::cin >> event_type;

        switch (event_type) {
            case EventType::PushTail: {
                IdenifierType identifier;
                std::cin >> identifier;
                deque.PushTail(identifier);
                positions.Insert(identifier, state.tail_clock++);
                break;
            }
            case EventType::PopHead: {
                positions.Erase(deque.Front());
                deque.PopHead();
                ++state.head_clock;
                break;
            }
            case EventType::PopTail: {
                positions.Erase(deque.Back());
                deque.PopTail();
                --state.tail_clock;
                break;
            }
            case EventType::DistanceFromHead: {
                IdenifierType identifier;
                std::cin >> identifier;

                const std::size_t* position = positions.Find(identifier);

                if (position == nullptr) {
                    throw std::runtime_error("Identifier not found");
                }

                std::size_t distance = *position - state.head_clock;
                std::cout << distance << '\n';
                break;
            }
            case EventType::GetHead: {
                std::cout << deque.Front() << '\n';
                break;
            }
        }
    }
}

}  // namespace astrocity_solution

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    astrocity_solution::HandleInput();
    return 0;
}
