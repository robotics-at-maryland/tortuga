/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/core/src/BitField3D.cpp
 */

// STD Includes
#include <assert.h>

// Project Includes
#include "core/include/BitField3D.h"

namespace ram {
namespace core {

BitField3D::BitField3D(size_t length, size_t width, size_t height)
    : m_bitfield(length*width*height) // default length
    , m_length(length)
    , m_width(width)
    , m_height(height)
{
}

std::vector<bool>::reference BitField3D::operator() (
    size_t x, size_t y, size_t z)
{
    assert(x >= 0 || x < m_length);
    assert(y >= 0 || y < m_width);
    assert(z >= 0 || z < m_height);
    return m_bitfield[m_length*m_width*z+m_length*y+x];
}

size_t BitField3D::length()
{
    return m_length;
}

size_t BitField3D::width()
{
    return m_width;
}

size_t BitField3D::height()
{
    return m_height;
}

size_t BitField3D::size()
{
    return m_bitfield.size();
}

} // namespace core
} // namespace ram
