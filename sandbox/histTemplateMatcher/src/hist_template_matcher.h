/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  sandbox/image_analyzer/include/image_analyzer.h
 */

#include <string>

// opencv
#include <cv.h>
#include <highgui.h>

int main(int argc, char **argv);

void match(std::string name_template, std::string name_source);
CvSize scaleSize(IplImage *orig, double amount);
