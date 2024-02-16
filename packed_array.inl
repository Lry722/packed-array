#pragma once
#include "packed_array.h"
#include <stdexcept>
#include <tuple>
#include <format>
#include <algorithm>

template <typename T>
    requires std::is_unsigned_v<T>
class PackedArray<T>::Access
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

template <typename T>
    requires std::is_unsigned_v<T>
class PackedArray<T>::iterator
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

template <typename T>
    requires std::is_unsigned_v<T>
class PackedArray<T>::const_iterator
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

static auto calcIndexInfo(const std::size_t index, const std::size_t element_size, const std::size_t unit_size)
{
    return std::tuple{(index * element_size) / unit_size, (index * element_size) % unit_size};
}

std::size_t minimumBitsRequired(auto n)
{
    if (n == 0)
    {
        return 1;
    }

    std::size_t bits = 0;
    while (n > 0)
    {
        n >>= 1;
        bits++;
    }
    return bits;
}

template <typename T>
    requires std::is_unsigned_v<T>
inline void PackedArray<T>::resize(const std::size_t size)
{
    size_ = size;
    data_.resize((size_ * element_size_ + kUnitSize - 1) / kUnitSize);
}

template <typename T>
    requires std::is_unsigned_v<T>
inline void PackedArray<T>::push_back(const T value)
{
    resize(size_ + 1);
    set(size_ - 1, value);
}

template <typename T>
    requires std::is_unsigned_v<T>
inline T PackedArray<T>::get(const std::size_t index) const
{
    if (index >= size_)
        throw std::out_of_range(std::format("PackedArray: index {} out of range!", index));

    const auto [index_in_data, index_in_unit] = calcIndexInfo(index, element_size_, kUnitSize);

    // 读取当前unit中的位
    T result{static_cast<T>((data_[index_in_data] >> index_in_unit) & mask_)};
    if (kUnitSize - index_in_unit < element_size_)
        // 读取超出当前unit的位
        result |= ((data_[index_in_data + 1] << index_in_unit) & mask_);

    return result;
}

template <typename T>
    requires std::is_unsigned_v<T>
inline void PackedArray<T>::set(const std::size_t index, const T value)
{
    if (index >= size_)
        throw std::out_of_range(std::format("PackedArray: index {} out of range!", index));

    const auto [index_in_data, index_in_unit] = calcIndexInfo(index, element_size_, kUnitSize);

    // 写入当前unit中的位
    data_[index_in_data] = data_[index_in_data] & ~(mask_ << index_in_unit) | (value << index_in_unit);
    if (kUnitSize - index_in_unit < element_size_)
        // 写入超出当前unit的位
        data_[index_in_data + 1] = data_[index_in_data + 1] & ~(mask_ >> (kUnitSize - index_in_unit)) | (value >> index_in_unit);
}

template <typename T>
    requires std::is_unsigned_v<T>
inline void PackedArray<T>::transform(const std::size_t element_size)
{
    if (element_size == element_size_)
        return;

    PackedArray<T> tmp(size_, element_size);
    for (int i = 0; i < size_; ++i)
        tmp.set(i, get(i));

    *this = std::move(tmp);
}

template <typename T>
    requires std::is_unsigned_v<T>
inline void PackedArray<T>::grow()
{
    transform(element_size_ + 1);
}

template <typename T>
    requires std::is_unsigned_v<T>
inline void PackedArray<T>::fit()
{
    const T maximum = *std::max_element(cbegin(), cend());
    transform(minimumBitsRequired(maximum));
}
