#ifndef main_h
#define main_h

#include "ProcessList.h"

int distance_from_line(int avgxs[], IplImage* img);
int angle_from_center(int argxs[], IplImage* img);
double hough(IplImage* img);
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
void to_ratios(IplImage* img);
int red_blue(IplImage* img, float ratio);
int white_detect(IplImage* percents, IplImage* base,int* binx, int* biny);
void run(ProcessList* pl);
void walk(IplImage* img, ProcessList* pl);
#endif
