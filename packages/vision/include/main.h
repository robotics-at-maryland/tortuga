#ifndef main_h
#define main_h

#include "ProcessList.h"
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
int mask_orange(IplImage* img, bool alter_img, bool strict);
void mask_with_input(IplImage* img);
int angle_from_center(int argxs[], IplImage* img);
void correct(IplImage* img);
void filter(IplImage* img, bool red_flag, bool green_flag, bool blue_flag);
void RAM_EXPORT to_ratios(IplImage* img);
int red_blue(IplImage* img, float ratio);
int white_detect(IplImage* percents, IplImage* base, IplImage* temp, int* binx, int* biny);
int gateDetect(IplImage* percents, IplImage* base, int* gatex, int* gatey);
int redDetect(IplImage* percents, IplImage* base, int* redx, int* redy);
void RAM_EXPORT redMask(IplImage* percents, IplImage* base);
void run(ram::vision::ProcessList* pl);
void walk(IplImage* img, ram::vision::ProcessList* pl);
int RAM_EXPORT histogram(IplImage* img, int* centerx, int* centery);
int redMaskAndHistogram(IplImage* percents, IplImage* base, int* redx, int* redy);
void rotate90Deg(IplImage* image, IplImage* dest);
void rotate90DegClockwise(IplImage* src, IplImage* dest);
void calibrateCamera(int width, int height, int* cornerCounts, float* distortion, float* cameraMatrix, float* transVects, float* rotMat, int numImages,  CvPoint2D32f* array, CvPoint3D32f* buffer);
int findCorners(IplImage* image, CvPoint2D32f* array);
void undistort(IplImage* image, IplImage* dest, float* cameraMatrix, float* distortion);
#endif
