#pragma once
#include <array>
#include <cstdint>
#include <concepts>

template <std::size_t ElementSize, std::size_t ArraySize, typename T = std::uint32_t>
    requires std::is_unsigned_v<T>
class BitArray
{
public:
    class Access
    {
        BitArray<ElementSize, ArraySize, T> &data_;
        std::size_t index_;

    public:
        Access(BitArray &data, std::size_t index) : data_(data), index_(index) {}
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
        using reference = T;

        iterator(BitArray<ElementSize, ArraySize, T> *array, std::size_t index)
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
        BitArray<ElementSize, ArraySize, T> *array_;
        std::size_t index_;
    };

    class const_iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using reference = Access;

        const_iterator(const BitArray<ElementSize, ArraySize, T> *array, std::size_t index)
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
        const BitArray<ElementSize, ArraySize, T> *array_;
        std::size_t index_;
    };

    typedef T value_type;
    typedef Access reference;

public:
    std::size_t size() const { return kArraySize; }
    bool empty() const { return false; }

    T get(std::size_t index) const;
    void set(std::size_t index, const T value);

    Access operator[](std::size_t index) { return Access(*this, index); }
    T operator[](std::size_t index) const { return get(index); }

    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, kArraySize); }
    const_iterator cbegin() const { return const_iterator(this, 0); }
    const_iterator cend() const { return const_iterator(this, kArraySize); }

private:
    // 当UnitSize为32时，该bitarray最多支持64位的ElementSize
    // 若UnitSize为16，则最多支持32位ElementSize，以此类推
    static inline const std::size_t kArraySize = ArraySize;
    static inline const std::size_t kElementSize = ElementSize;
    static inline const std::size_t kUnitSize = sizeof(T);
    static inline const std::size_t kInnerArraySize = (kElementSize * kArraySize + kUnitSize - 1) / kUnitSize;
    // kMask 参与的表达式都会自动转为ull，并在最后转为T时截断高位不需要的内容
    static inline const unsigned long long kMask = (1 << kElementSize) - 1;

    std::array<T, kInnerArraySize> data{};
};

#include "bitarray.inl"
