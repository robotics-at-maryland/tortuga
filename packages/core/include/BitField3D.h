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

// Libary Includes
#include <boost/serialization/vector.hpp>

namespace ram {
namespace core {

class BitField3D
{
public:
    BitField3D(size_t length, size_t width, size_t height);

    std::vector<bool>::reference operator() (size_t x, size_t y, size_t z);

    size_t length();
    size_t width();
    size_t height();
    size_t size();

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & m_bitfield;
        ar & m_length;
        ar & m_width;
        ar & m_height;
    }

    std::vector<bool> m_bitfield;
    size_t m_length;
    size_t m_width;
    size_t m_height;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_BITFIELD3D_04_21_2011
