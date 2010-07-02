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

class Convert
{
public:
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

    static void createLookupTable();
    static void saveLookupTable(const char *);
    static bool loadLookupTable();

    static void RGB2LCHuv(vision::Image* image);

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

private:
    static void initTransform();

    // static float fast_atan2(float y, float x);
    // static float fast_sqrt(float x);
    // static float fast_resqrt(float x);

    static unsigned char rgb2lchLookup[256][256][256][3];

    // gamma correction factor
    static double gamma;

    // matrix for transforming rgb to xyz
    static math::Matrix3 rgb2xyzTransform;

    // white points for xyz to lab transform
    static double X_ref;
    static double Y_ref;
    static double Z_ref;

    // ref points for xyz to luv transform
    static double u_prime_ref;
    static double v_prime_ref;
    static double eps; // CIE Standard
    static double kappa; // CIE Standard

    static bool lookupInit;

    Convert() {};

    Convert(const Convert& c) {};
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_CONVERT_H_06_30_2010
