/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/include/Convert.h
 */

// Project Includes
#include "math/include/Matrix3.h"
#include "vision/include/Image.h"

#ifndef RAM_VISION_CONVERT_H_06_30_2010
#define RAM_VISION_CONVERT_H_06_30_2010

namespace ram {
namespace vision {

class LCHConverter
{
public:
    // Converts a single pixel
    static void convertPixel(unsigned char &r,
                             unsigned char &g,
                             unsigned char &b);

    static void createLookupTable(bool verbose = false);
    static void saveLookupTable(const char *);
    static bool loadLookupTable();

    static void convert(vision::Image* image);

private:
    /* Here are the steps to convert a BGR pixel to a CIELCH pixel
       assuming a pointer px = &channel 1

       1. invGammaCorrection(px, px + 1, px + 2);
       2. rgb2xyz(px, px + 1, px + 2);
       3. xyz2luv(px, px + 1, px + 2);
       4. luv2lch_uv(px, px + 1, px + 2);

       The pixel channels are now converted to CIELCh
       If gamma correction is turned off, the first step should be ignored
    */
    static void invGammaCorrection(double *ch1, double *ch2, double *ch3);

    // convert the values of a single pixel from rgb to xyz
    // requires input in range [0, 1]
    static void rgb2xyz(double *r2x, double *g2y, double *b2z);

    // convert the values of a single pixel from xyz to lab
    // requires input processed from rgb2xyz()
    static void xyz2lab(double *x2l, double *y2a, double *z2b);

    // convert the values of a single pixel from xyz to luv
    // requires input processed from rgb2xyz()
    static void xyz2luv(double *x2l, double *y2u, double *z2v);

    // convert the values of a single pixel from lab to lch
    // output is (0, 255)
    // requires input process from xyz2lab() / xyz2luv() respectively
    static void lab2lch_ab(double *l2l, double *a2c, double *b2h);
    static void luv2lch_uv(double *l2l, double *a2c, double *b2h);

    static unsigned char rgb2lchLookup[256][256][256][3];

    static bool lookupInit;

    LCHConverter() {};

    LCHConverter(const LCHConverter& c) {};
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_CONVERT_H_06_30_2010
