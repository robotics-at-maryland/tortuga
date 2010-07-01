/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  packages/vision/src/Convert.cpp
 */

// STD Includes
#include <cmath>
#include <iostream>
#include <fstream>

// Project Includes
#include "math/include/Math.h"
#include "math/include/Vector3.h"
#include "vision/include/Convert.h"

namespace ram {
namespace vision {

double Convert::gamma = 2.2;
math::Matrix3 Convert::rgb2xyzTransform = math::Matrix3::IDENTITY;

double Convert::X_ref = 0;
double Convert::Y_ref = 0;
double Convert::Z_ref = 0;

double Convert::u_prime_ref = 0;
double Convert::v_prime_ref = 0;
double Convert::eps = 0;
double Convert::kappa = 0;

unsigned char Convert::rgb2lchLookup[256][256][256][3] = {{{{0}}}};

bool Convert::lookupInit = false;

void Convert::initTransform()
{
    static bool initialized = false;
    if(!initialized) {
        // gamma correction factor
        gamma = 2.2; // sRGB
    
        // sRGB transform matrix
         rgb2xyzTransform = math::Matrix3(0.4124564, 0.3575761, 0.1804375,
                                          0.2126729, 0.7151522, 0.0721750,
                                          0.0193339, 0.1191920, 0.9503041);

        // NTSC RGB
        //rgb2xyzTransform = math::Matrix3(0.6068909,  0.1735011,  0.2003480,
        //                                 0.2989164,  0.5865990,  0.1144845,
        //                                 -0.0000000,  0.0660957,  1.1162243);


        /* We are currently using values from a 2 degree observer */

        /* X, Y, Z White Point tristimulus values can be calculated from x, y chromaticity
           values by setting Y = 1, then X = x/y and Z = (1 - x - y) / y */

        // D65 illuminant
        double Y = 1;
        double x = 0.31271;
        double y = 0.32902;

        // D75 illuminant
        // x = 0.29902;
        // y = 0.31485;

        // C illuminant
        //x = 0.31006;
        // y = 0.31616;

        // calculate xyz tristimulus values
        X_ref = (x * Y) / y;
        Y_ref = Y;
        Z_ref = (Y * (1 - x - y) )/ y;

        eps = 0.008856; // CIE Standard
        kappa = 903.3;  // CIE Standard

        double refDenom = (X_ref + (15 * Y_ref) + (3 * Z_ref));
        u_prime_ref = (4 * X_ref) / refDenom;
        v_prime_ref = (9 * Y_ref) / refDenom;

        initialized = true;
    }
}

void Convert::rgb2xyz(double *r2x, double *g2y, double *b2z)
{
    initTransform();
    math::Vector3 rgb(*r2x, *g2y, *b2z);
    math::Vector3 xyz = rgb2xyzTransform * rgb;
    *r2x = xyz[0]; // X
    *g2y = xyz[1]; // Y
    *b2z = xyz[2]; // Z
}

void Convert::xyz2lab(double *x2l, double *y2a, double *z2b)
{
    initTransform();
    double X = *x2l / X_ref;
    double Y = *y2a / Y_ref;
    double Z = *z2b / Z_ref;

    double fx = pow(X, 1/3);
    if(X < 0.008856)
        fx = X*(841/108) + (4/29);

    double fy = pow(Y, 1/3);
    if(Y < 0.008856)
        fy = Y*(841/108) + (4/29);

    double fz = pow(Z, 1/3);
    if(Z < 0.008856)
        fz = Z*(841/108) + (4/29);

    *x2l = 116 * fy - 16;   // L*
    *y2a = 500 * (fx - fy); // a*
    *z2b = 200 * (fy - fz); // b*
}

void Convert::xyz2luv(double *x2l, double *y2u, double *z2v)
{
    initTransform();
    double X = *x2l, Y = *y2u, Z = *z2v;

    double pxDenom = (X + (15 * Y) + (3 * Z));
    double u_prime = (4 * X) / pxDenom;
    double v_prime = (9 * Y) / pxDenom;

    double yr = Y / Y_ref;

    double L = 0, u = 0, v = 0;

    if(yr > eps)
        L = 116 * pow(yr, .3333) - 16;
    else
        L = kappa * yr;

    u = 13 * L * (u_prime - u_prime_ref);
    v = 13 * L * (v_prime - v_prime_ref);

    *x2l = L;
    *y2u = u;
    *z2v = v;
}

void Convert::lab2lch_ab(double *l2l, double *a2c, double *b2h)
{
    initTransform();
    double a = *a2c;
    double b = *b2h;

    // calculate the chrominance
    double c = sqrt(a*a + b*b);
    // calculate the hue
    double h = atan2(b, a) / math::Math::PI;

    while(h < 0)
        h += 2;
    while(h > 2)
        h -= 2;

    // scale to (0, 255)
    h *= 127.5;

    *a2c = c;
    *b2h = h;
}

void Convert::luv2lch_uv(double *l2l, double *u2c, double *v2h)
{
    initTransform();
    double u = *u2c;
    double v = *v2h;

    // calculate the chrominance
    double c = sqrt(u*u + v*v);
    // calculate the hue
    double h = atan2(v, u) / math::Math::PI;

    while(h < 0)
        h += 2;
    while(h > 2)
        h -= 2;

    // scale to (0, 255)
    h *= 127.5;

    *u2c = c;
    *v2h = h;
}

void Convert::invGammaCorrection(double *ch1, double *ch2, double *ch3)
{
    // this is a simplified gamma correction if the RGB system is sRGB
    initTransform();
    *ch1 = pow(*ch1, gamma);
    *ch2 = pow(*ch2, gamma);
    *ch3 = pow(*ch3, gamma);
}

void Convert::RGB2LCHuv(vision::Image* image)
{
    assert(image->getPixelFormat() == Image::PF_RGB_8 && "Incorrect Pixel Format");
    initTransform();
    unsigned char *data = (unsigned char *) image->getData();

    unsigned int numpixels = image->getWidth() * image->getHeight();
    for(unsigned int pix = 0; pix < numpixels; pix++)
    {
        double ch1 = (double) data[3*pix] / (double) 255;
        double ch2 = (double) data[1 + 3*pix] / (double) 255;
        double ch3 = (double) data[2 + 3*pix] / (double) 255;

        // remove this when gamma correction is turned off
        invGammaCorrection(&ch1, &ch2, &ch3);

        // convert to xyz then luv then lch
        rgb2xyz(&ch1, &ch2, &ch3);
        xyz2luv(&ch1, &ch2, &ch3);
        luv2lch_uv(&ch1, &ch2, &ch3);

        data[3*pix] = ch1;
        data[1 + 3*pix] = ch2;
        data[2 + 3*pix] = ch3;
    }
}

void Convert::createLookupTable()
{
    initTransform();
    double ch1 = 0, ch2 = 0, ch3 = 0;

    static unsigned char lookup[256][256][256][3];
    
    int counter = 0;
    int size = 256 * 256 * 256;
    for(int c1 = 0; c1 < 256; c1++){
        for(int c2 = 0; c2 < 256; c2++){
            for(int c3 = 0; c3 < 256; c3++){
                ch1 = (char) c1;
                ch2 = (char) c2;
                ch3 = (char) c3;
                
                invGammaCorrection(&ch1, &ch2, &ch3);
                rgb2xyz(&ch1, &ch2, &ch3);
                xyz2luv(&ch1, &ch2, &ch3);
                luv2lch_uv(&ch1, &ch2, &ch3);
                
                lookup[c1][c2][c3][0] = ch1;
                lookup[c1][c2][c3][1] = ch2;
                lookup[c1][c2][c3][2] = ch3;

                std::cout << "\r" << counter << " / " << size;
                //std::cout.flush();
                counter++;
            }
        }
    }
    std::cout << std::endl;
    saveLookupTable((char*)&(lookup[0][0][0][0]));
}

void Convert::saveLookupTable(const char *data)
{
    initTransform();
    std::ofstream lookupFile;
//        RAM_SVN_DIR
    lookupFile.open(" rgb2luvLookup.bin", std::ios::out | std::ios::binary);
    if(lookupFile.is_open()){   
        lookupFile.write(data, 256*256*256*3);
    } else {
        std::cerr << "Error opening file for output." << std::endl;
    }
}

void Convert::loadLookupTable()
{
    initTransform();
    std::ifstream lookupFile;
    char *data = (char *)(&rgb2lchLookup[0][0][0][0]);
    lookupFile.open("rgb2luvLookup.bin", std::ios::in | std::ios::binary);
        
    assert(lookupFile.is_open() && "Lookup file does not exist. Please generate it.");
    lookupFile.seekg(0, std::ios::beg);
    lookupFile.read(data, 256*256*256*3);
    lookupInit = true;
}


// float Convert::fast_atan2(float y, float x)
// {
//   /*
//     The function f(r)=atan((1-r)/(1+r)) for r in [-1,1] is easier to
//     approximate than atan(z) for z in [0,inf]. To approximate f(r) to
//     the third degree we may solve the system

//      f(+1) = c0 + c1 + c2 + c3 = atan(0) = 0
//      f(-1) = c0 - c1 + c2 - c3 = atan(inf) = pi/2
//      f(0)  = c0                = atan(1) = pi/4

//     which constrains the polynomial to go through the end points and
//     the middle point.

//     We still miss a constrain, which might be simply a constarint on
//     the derivative in 0. Instead we minimize the Linf error in the
//     range [0,1] by searching for an optimal value of the free
//     parameter. This turns out to correspond to the solution
     
//      c0=pi/4, c1=-0.9675, c2=0, c3=0.1821

//     which has maxerr = 0.0061 rad = 0.35 grad.
//   */

//   float angle, r ;
//   float const c3 = 0.1821 ;
//   float const c1 = 0.9675 ;
//   float abs_y    = abs(y) + float(1e-10) ;

//   if (x >= 0) {
//     r = (x - abs_y) / (x + abs_y) ;
//     angle = float(math::Math::PI/4.0) ;
//   } else {
//     r = (x + abs_y) / (abs_y - x) ;
//     angle = float(3*math::Math::PI/4.0) ;
//   } 
//   angle += (c3*r*r - c1) * r ; 
//   return (y < 0) ? -angle : angle ;
// }

// /** @brief Fast @c sqrt
//  ** @param x argument.
//  ** @return Approximation to @c sqrt(x).
//  **/
// float Convert::fast_sqrt(float x)
// {
//   return (x < 1e-8) ? 0 : x * fast_resqrt(x) ;
// }



// /** @brief Fast @c resqrt
//  ** @param x argument.
//  ** @return Approximation to @c resqrt(x).
//  **/

// float Convert::fast_resqrt(float x)
// {
//   // Works if VL::float_t is 32 bit ...
//   union {
//     float x ;
//     float i ;
//   } u ;
//   float xhalf = float(0.5) * x ;
//   u.x = x ;                               // get bits for floating value
//   u.i = 0x5f3759df - ((int)(u.i)>>1);            // gives initial guess y0
//   //u.i = 0xdf59375f - (u.i>>1);          // gives initial guess y0
//   u.x = u.x*(float(1.5) - xhalf*u.x*u.x); // Newton step (may repeat)
//   u.x = u.x*(float(1.5) - xhalf*u.x*u.x); // Newton step (may repeat)
//   return u.x ;
// }

} // namespace vision
} // namespace ram
