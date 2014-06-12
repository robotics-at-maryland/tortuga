/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  sandbox/image_analyzer/include/image_analyzer.h
 */

#include <string.h>

// root
#include <TROOT.h>
#include <TH1F.h>
#include <TCanvas.h>

// opencv
#include <cv.h>
#include <highgui.h>

int main(int argc, char **argv);
IplImage* loadImage(std::string filepath);
void analyzeBuoyImage(IplImage* img, IplImage* redMask,
                      IplImage* greenMask, IplImage *yellowMask);
