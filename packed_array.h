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
    class Access
    {
        PackedArray<T> &data_;
        const std::size_t index_;

    public:
        Access(PackedArray &data, std::size_t index) : data_(data), index_(index) {}
        Access &operator=(const T value)
        {
            data_.set(index_, value);
            return *this;
        }
        operator T() const
        {
            return data_.get(index_);
        }
    };
    friend class Access;

    class iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = Access;

        iterator(PackedArray<T> *array, const std::size_t index)
            : array_(array), index_(index) {}

        reference operator*() const { return (*array_)[index_]; }

        iterator &operator++()
        {
            ++index_;
            return *this;
        }
        iterator operator++(int)
        {
            iterator tmp(*this);
            operator++();
            return tmp;
        }

        iterator &operator--()
        {
            --index_;
            return *this;
        }
        iterator operator--(int)
        {
            iterator tmp(*this);
            operator--();
            return tmp;
        }

        iterator &operator+=(difference_type n)
        {
            index_ += n;
            return *this;
        }
        iterator &operator-=(difference_type n)
        {
            index_ -= n;
            return *this;
        }

        friend iterator operator+(iterator it, difference_type n) { return it += n; }
        friend iterator operator+(difference_type n, iterator it) { return it += n; }
        friend iterator operator-(iterator it, difference_type n) { return it -= n; }

        friend difference_type operator-(iterator lhs, iterator rhs) { return lhs.index_ - rhs.index_; }

        bool operator==(const iterator &other) const { return array_ == other.array_ && index_ == other.index_; }
        bool operator<(const iterator &other) const { return index_ < other.index_; }

    private:
        PackedArray<T> *array_;
        std::size_t index_;
    };

    class const_iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = T;

        const_iterator(const PackedArray<T> *array, const std::size_t index)
            : array_(array), index_(index) {}

        reference operator*() const { return (*array_)[index_]; }

        const_iterator &operator++()
        {
            ++index_;
            return *this;
        }
        const_iterator operator++(int)
        {
            const_iterator tmp(*this);
            operator++();
            return tmp;
        }

        const_iterator &operator--()
        {
            --index_;
            return *this;
        }
        const_iterator operator--(int)
        {
            const_iterator tmp(*this);
            operator--();
            return tmp;
        }

        const_iterator &operator+=(difference_type n)
        {
            index_ += n;
            return *this;
        }
        const_iterator &operator-=(difference_type n)
        {
            index_ -= n;
            return *this;
        }

        friend const_iterator operator+(const_iterator it, difference_type n) { return it += n; }
        friend const_iterator operator+(difference_type n, const_iterator it) { return it += n; }
        friend const_iterator operator-(const_iterator it, difference_type n) { return it -= n; }

        friend difference_type operator-(const_iterator lhs, const_iterator rhs) { return lhs.index_ - rhs.index_; }

        bool operator==(const const_iterator &other) const { return array_ == other.array_ && index_ == other.index_; }
        bool operator<(const const_iterator &other) const { return index_ < other.index_; }

    private:
        const PackedArray<T> *array_;
        std::size_t index_;
    };

    typedef T value_type;
    typedef Access reference;

public:
    PackedArray() = default;
    PackedArray(const std::size_t size, const std::size_t element_size = 4) noexcept : size_(size), element_size_(element_size), mask_((1ULL << element_size_) - 1), data_((size * element_size_ + kUnitSize - 1) / kUnitSize) {}

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    std::size_t memory() const { return data_.size() * kUnitSize; }

    void resize(const std::size_t size)
    {
        size_ = size;
        data_.resize((size_ * element_size_ + kUnitSize - 1) / kUnitSize);
    }
    void push_back(const T value)
    {
        resize(size_ + 1);
        set(size_ - 1, value);
    }

    T get(const std::size_t index) const;
    void set(const std::size_t index, const T value);
    void transform(const std::size_t element_size);
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
