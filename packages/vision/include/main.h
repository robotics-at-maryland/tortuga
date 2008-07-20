/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/include/main.h
 */

#ifndef RAM_RED_LIGHT_MAIN_H_06_23_2007
#define RAM_RED_LIGHT_MAIN_H_06_23_2007

// Library Includes
#include <cv.h>

// Project Includes
#include "vision/include/Common.h"

// Must be included last
#include "vision/include/Export.h"

//Light constants
#define MINFRAMESON 3
#define MINFRAMESOFF 3
#define MAXFRAMESON 7
#define MAXFRAMESOFF 7
#define HOUGH_ERROR -10

int distance_from_line(int avgxs[], IplImage* img); 
int angle_from_center(int argxs[], IplImage* img);
double hough(IplImage* img, int* x, int* y);
void diff(IplImage* img, IplImage* oldImg, IplImage* destination);
int mask_red(IplImage* img, bool alter_img, int threshold);
void explore(IplImage* img, int x, int y, int* out, int color);
CvPoint find_flash(IplImage* img, bool display);
int guess_line(IplImage* img);

/** Makes all orange in the image white, and everything else black
 *
 *  @param img
 *      A raw image from the camera
 *  @param alter_img
 *      Whether or not to make the color changes to the image
 *  @param brightness
 *      A threshold which the sum of each pixels RGB parts must meet
 *  @param strict
 *      Whether or not to use a strict orange filtering
 */
int mask_orange(IplImage* img, bool alter_img, int brightness, bool strict);

int angle_from_center(int argxs[], IplImage* img);
void correct(IplImage* img);
void filter(IplImage* img, bool red_flag, bool green_flag, bool blue_flag);
void RAM_EXPORT to_ratios(IplImage* img);
int red_blue(IplImage* img, float ratio);

/**
 * @param percents
 *     Holds the percents of each color
 * @param base
 *     Original image from the computer
 * @param temp
 *      A work space/output image
 */
int white_detect(IplImage* percents, IplImage* base, IplImage* temp, int* binx, int* biny);

int white_mask(IplImage* percents, IplImage* base, IplImage* output, unsigned char minPercentIntensity = 30, unsigned char minIntensity = 190);
int black_mask(IplImage* percents, IplImage* base, IplImage* output);
int gateDetect(IplImage* percents, IplImage* base, int* gatex, int* gatey);
int redDetect(IplImage* percents, IplImage* base, int* redx, int* redy);

/* masks a suit from a bin*/
void suitMask(IplImage* percents, IplImage* base);

/* masks a treasure*/
void safeMask(IplImage* base);

/** Makes all red in the image white, and everything else black
 *
 *  @param percents
 *      The result of running the base image through vision::to_ratios
 *  @param base
 *      The starting image from the camera.
 *  @param redPercent
 *      The percent of the total pixel value that is red.
 *  @param redIntensity
 *      The the overall red value in the image.
 *
 */
void RAM_EXPORT redMask(IplImage* percents, IplImage* base,
                        int redPercent, int redIntensity);

/** Takes an image from redMask and finds the biggest red blob
 *
 *  @param img
 *      The image that is outputed by redMask
 *  @param centerX
 *      The X cordinate of the center of the biggest red blob
 *  @param centerY
 *      The Y cordinate of the center of the biggest red blob
 *  @param maxX
 *      The largest X cordinate of the pixels in the biggest red blob
 *  @param maxY
 *      The largest Y cordinate of the pixels in the biggest red blob
 *  @param minX
 *      The smallest X cordinate of the pixels in the biggest red blob
 *  @param minY
 *      The smallest Y cordinate of the pixels in the biggest red blob
 *
 *  @return
 *      The number of red pixels in the biggest blob
 */
int RAM_EXPORT histogram(IplImage* img, int* centerX, int* centerY, int* maxX,
                         int* maxY, int* minX, int* minY);

int redMaskAndHistogram(IplImage* percents, IplImage* base, int* redx, int* redy);
void rotate90Deg(IplImage* image, IplImage* dest);
void rotate90DegClockwise(IplImage* src, IplImage* dest);
void calibrateCamera(int width, int height, int* cornerCounts, float* distortion, float* cameraMatrix, float* transVects, float* rotMat, int numImages,  CvPoint2D32f* array, CvPoint3D32f* buffer);
int findCorners(IplImage* image, CvPoint2D32f* array);
void undistort(IplImage* image, IplImage* dest, float* cameraMatrix, float* distortion);
int suitDifference(int array1[], int array2[], int len);


#endif // RAM_RED_LIGHT_MAIN_H_06_23_2007
