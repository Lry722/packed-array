#pragma once
#include <vector>
#include <cstdint>
#include <concepts>
#include <climits>

/// @brief A PackedArray class template that efficiently stores and manipulates arrays of fixed-size elements, each composed of a specified number of bits.
/// @tparam T The underlying unsigned integer type used to store the bit-packed elements. By default, it is `std::uint32_t`.
template <typename T = std::uint32_t>
    requires std::is_unsigned_v<T>
class PackedArray
{
public:
    class Access;
    friend class Access;

    class iterator;
    class const_iterator;

    typedef T value_type;
    typedef Access reference;

public:
    PackedArray() = default;
    PackedArray(const std::size_t size, const std::size_t element_size = 4) noexcept : size_(size), element_size_(element_size), mask_((1ULL << element_size_) - 1), data_((size * element_size_ + kUnitSize - 1) / kUnitSize) {}

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void resize(const std::size_t size);
    void push_back(const T value);

    T get(const std::size_t index) const;
    void set(const std::size_t index, const T value);
    
    void transform(const std::size_t element_size);
    void grow();
    void fit();

    Access operator[](const std::size_t index) { return Access(*this, index); }
    T operator[](const std::size_t index) const { return get(index); }

    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, size_); }
    const_iterator cbegin() const { return const_iterator(this, 0); }
    const_iterator cend() const { return const_iterator(this, size_); }

private:
    // 当UnitSize为32时，该bitarray最多支持64位的ElementSize
    // 若UnitSize为16，则最多支持32位ElementSize，以此类推
    static inline const std::size_t kUnitSize = sizeof(T) * CHAR_BIT;
    std::size_t element_size_ = 4;
    std::size_t size_ = 0;

    // Mask 参与的表达式都会自动转为ull，并在最后转为T时截断高位不需要的内容
    std::uint64_t mask_ = (1 << element_size_) - 1;

    std::vector<T> data_{};
};

#include "packed_array.inl"
