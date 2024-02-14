#pragma once
#include "packed_array.h"
#include <stdexcept>
#include <tuple>
#include <format>
#include <algorithm>

static auto calcIndexInfo(const std::size_t index, const std::size_t element_size, const std::size_t unit_size)
{
    return std::tuple{(index * element_size) / unit_size, (index * element_size) % unit_size};
}

std::size_t minimumBitsRequired(auto n) {
    if (n == 0) {
        return 1;
    }

    std::size_t bits = 0;
    while (n > 0) {
        n >>= 1;
        bits++;
    }
    return bits;
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
inline void PackedArray<T>::fit()
{
    const T maximum = *std::max_element(cbegin(), cend());
    transform(minimumBitsRequired(maximum));
}
