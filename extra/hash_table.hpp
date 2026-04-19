#pragma once

#include <cstddef>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>
#include <cstdint>
#include <optional>

namespace hash_table {
template <typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<>>
class HashTable {
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
    [[nodiscard]] explicit HashTable(std::uint8_t power_of_two_size = kDefaultPowerOfTwoSize,
                                     const Hash& hash = Hash(),
                                     const KeyEqual& key_equal = KeyEqual())
        : data_(1LL << power_of_two_size),
          mask_(data_.size() - 1),
          hash_(hash),
          key_equal_(key_equal) {
    }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;
    HashTable(HashTable&&) = delete;
    HashTable& operator=(HashTable&&) = delete;

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

    // Example:
    // LoadFactorAfterInsert = (size_ + 1) / data_.size() > 7, which is equivalent to
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

}  // namespace hash_table
