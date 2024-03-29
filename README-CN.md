[English](README.md) | 中文

## 介绍

这是一个高效的 PackedArray 的 cpp 实现，用于存储自定义位数的无符号整数，具有以下特性：

- **紧凑存储**：所有数据被紧密排列在一起，8 个 4 位整数只会占用 32 位的空间，而不是 8 个字节，或是 8 个 int。
- **高效操作**：通过位运算实现存储，而不是逐位进行遍历。
- **广泛兼容**：实现了 STL 容器的接口，支持各种 STL 算法。

## 使用方法

只需要将 packed_array.h 和 packed_array.inl 文件复制到你的项目中，并进行 include 即可。

各个方法的使用方法详见 [packed_array.h](packed_array.h) 中的注释。
