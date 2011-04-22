/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/core/include/BitField3D.h
 */

#ifndef RAM_CORE_BITFIELD3D_04_21_2011
#define RAM_CORE_BITFIELD3D_04_21_2011

// STD Includes
#include <vector>

namespace ram {
namespace core {

class BitField3D
{
public:
    BitField3D(size_t length, size_t width, size_t height);

    std::vector<bool>::reference operator() (size_t x, size_t y, size_t z);

    size_t length() const;
    size_t width() const;
    size_t height() const;
    size_t size() const;

private:
    std::vector<bool> m_bitfield;
    const size_t m_length;
    const size_t m_width;
    const size_t m_height;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_BITFIELD3D_04_21_2011
