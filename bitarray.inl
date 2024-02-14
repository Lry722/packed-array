#pragma once
#include "bitarray.h"
#include <stdexcept>
#include <tuple>
#include <format>

static auto calcIndexInfo(const std::size_t index, const std::size_t element_size, const std::size_t unit_size)
{
    return std::tuple{(index * element_size) / unit_size, (index * element_size) % unit_size};
}

template <std::size_t ElementSize, std::size_t ArraySize, typename T>
    requires std::is_unsigned_v<T>
inline T BitArray<ElementSize, ArraySize, T>::get(std::size_t index) const
{
    if (index >= kArraySize)
        throw std::out_of_range(std::format("BitArray: index {} out of range!", index));

    const auto [kIndexInData, kIndexInUnit] = calcIndexInfo(index, kElementSize, kUnitSize);

    // 读取当前unit中的位
    T result{static_cast<T>((data[kIndexInData] >> kIndexInUnit) & kMask)};
    if (kUnitSize - kIndexInUnit < kElementSize)
        // 读取超出当前unit的位
        result |= ((data[kIndexInData + 1] << kIndexInUnit) & kMask);

    return result;
}

template <std::size_t ElementSize, std::size_t ArraySize, typename T>
    requires std::is_unsigned_v<T>
inline void BitArray<ElementSize, ArraySize, T>::set(std::size_t index, const T value)
{
    if (index >= kArraySize)
        throw std::out_of_range(std::format("BitArray: index {} out of range!", index));

    const auto [kIndexInData, kIndexInUnit] = calcIndexInfo(index, kElementSize, kUnitSize);

    // 写入当前unit中的位
    data[kIndexInData] = data[kIndexInData] & ~(kMask << kIndexInUnit) | (value << kIndexInUnit);
    if (kUnitSize - kIndexInUnit < kElementSize)
        // 写入超出当前unit的位
        data[kIndexInData + 1] = data[kIndexInData + 1] & ~(kMask >> (kUnitSize - kIndexInUnit)) | (value >> kIndexInUnit);
}
