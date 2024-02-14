English | [中文](README-CN.md)

## Description

This is an efficient C++ implementation of a BitArray designed to store custom-sized unsigned integers, featuring:

- **Compact Storage**: Data is tightly packed together, with eight 4-bit integers consuming only 32 bits of space instead of the usual 8 bytes.
- **Efficient Operations**: Storage and manipulation are achieved through bitwise operations rather than iterating over individual bits.
- **Wide Compatibility**: It implements interfaces compatible with STL containers, allowing for seamless integration with various STL algorithms.

## Usage

To use this library, simply clone the `bitarray.h` and `bitarray.inl` into your project and include them.

The meaning of template parameters is explained in the comments in [bitarray.h](bitarray.h) .
