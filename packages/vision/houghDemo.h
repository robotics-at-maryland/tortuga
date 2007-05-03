/*
 *  HoughDemo.h
 *  
 *
 *  Created by Daniel Hakim on 2/1/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include "cv.h"
#include "highgui.h"
#include "stdio.h"

void hough(IplImage*);
void LU2filter(IplImage*);
void sobel(IplImage*);
void mask(IplImage*, int, unsigned int);
void find_green(IplImage*);