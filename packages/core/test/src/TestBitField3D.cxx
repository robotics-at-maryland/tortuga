/*
 * Copyright (C) 2011 Robotics at Maryland
 * Copyright (C) 2011 Jonathan Sternberg <jsternbe@umd.edu>
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/core/test/src/TestBitField3D.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Project Includes
#include "core/include/BitField3D.h"

using namespace ram::core;

TEST(bitfield_default)
{
    BitField3D bitfield(2, 2, 2);
    CHECK_EQUAL(8u, bitfield.size());

    for (size_t k = 0; k < 2; k++) {
        for (size_t j = 0; j < 2; j++) {
            for (size_t i = 0; i < 2; i++) {
                CHECK(!bitfield(i, j, k));
            }
        }
    }
}

TEST(bitfield_size)
{
    BitField3D bitfield(1, 2, 3);
    CHECK_EQUAL(1u, bitfield.length());
    CHECK_EQUAL(2u, bitfield.width());
    CHECK_EQUAL(3u, bitfield.height());
}

TEST(bitfield_assignment)
{
    BitField3D bitfield(2, 2, 2);
    bitfield(0, 1, 1) = true;
    CHECK(bitfield(0, 1, 1));
}
