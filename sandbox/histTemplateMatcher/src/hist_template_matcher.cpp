/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Wonders
 * All rights reserved.
 *
 * Author: Jonathan Wonders <jwonders@umd.edu>
 * File:  sandbox/image_analyzer/src/image_analyzer.cpp
 */

#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

// // grab all root stuff
// #include <TROOT.h>
// #include <TH1I.h>
// #include <TH3I.h>
// #include <TCanvas.h>
// #include <TFile.h>

#include <string>
// open cv
#include <cv.h>
#include <highgui.h>

// project includes
#include "hist_template_matcher.h"

#define NUM_REQUIRED_ARGS 3
#define ARG_MESSAGE "Usage: hist_template_matcher"
#define IMAGE_PATH "/home/jwonders/ram_code/trunk/sandbox/histTemplateMatcher/src/0078.png"

int main(int argc, char **argv)
{
    /* check for supplied argument */
    if( argc < NUM_REQUIRED_ARGS ) {
        fprintf( stderr,  ARG_MESSAGE);
        return 1;
    }
    std::string name_template = argv[1];
    std::string name_source = argv[2];

    match(name_template, name_source);

    return 0;
}

CvSize scaleSize(IplImage *orig, double amount)
{
    return cvSize((int) (orig->width * amount), (int) (orig->height * amount));
}

void match(std::string name_template, std::string name_source)
{
    // Set up some images
    IplImage *img_original = cvLoadImage(name_source.c_str());
    IplImage *img_template = cvLoadImage(name_template.c_str());
    IplImage *img_template_big = cvCreateImage(
        scaleSize(img_template, 1.25), IPL_DEPTH_8U, 3);
    IplImage *img_template_biggest = cvCreateImage(
        scaleSize(img_template, 1.50), IPL_DEPTH_8U, 3);
    IplImage *img_template_small = cvCreateImage(
        scaleSize(img_template, .75), IPL_DEPTH_8U, 3);
    IplImage *img_template_smallest = cvCreateImage(
        scaleSize(img_template, .50), IPL_DEPTH_8U, 3);

    cvResize(img_template, img_template_big);
    cvResize(img_template, img_template_biggest);
    cvResize(img_template, img_template_small);
    cvResize(img_template, img_template_smallest);
    
    assert(img_original != 0 && "Image failed to load");
    assert(img_template != 0 && "Template failed to load");

    IplImage *img_backprojection = cvCreateImage(cvGetSize(img_original), IPL_DEPTH_8U, 1);

    // Compute HSV image and separate into colors
    IplImage *img_hsv = cvCreateImage(cvGetSize(img_original), IPL_DEPTH_8U, 3);
    cvCvtColor(img_original, img_hsv, CV_BGR2HSV);

    IplImage *h_plane = cvCreateImage(cvGetSize(img_original), 8, 1);
    IplImage *s_plane = cvCreateImage(cvGetSize(img_original), 8, 1);
    IplImage *v_plane = cvCreateImage(cvGetSize(img_original), 8, 1);
    IplImage *planes[] = {h_plane, s_plane};
    cvCvtPixToPlane(img_hsv, h_plane, s_plane, v_plane, 0);

    // Build and fill the histogram
    int h_bins = 30, s_bins = 32;
    CvHistogram* hist;
    {
        int hist_size[] = {h_bins, s_bins};
        float h_ranges[] = {0, 180};
        float s_ranges[] = {0, 255};
        float * ranges[] = {h_ranges, s_ranges};
        hist = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
    }
    cvCalcHist(planes, hist, 0, 0); // compute histogram
    cvNormalizeHist(hist, 20*255);  // normalize it
    
    cvCalcBackProject(planes, img_backprojection, hist); // calculate the backprojection
    cvNormalizeHist(hist, 1.0); // normalize the histogram

    // Create an image to visualize the histogram
	int scale = 10;
	IplImage* img_hist = cvCreateImage(cvSize(h_bins * scale, s_bins * scale), 8, 3);
	cvZero(img_hist);

    // Populate the visualization
    float max_value = 0;
    cvGetMinMaxHistValue(hist, 0, &max_value, 0, 0);

    for(int h = 0; h < h_bins; h++){
        for(int s = 0; s < s_bins; s++){
            float bin_val = cvQueryHistValue_2D(hist, h, s);
            int intensity = cvRound(bin_val * 255 / max_value);
            cvRectangle(img_hist,
                        cvPoint(h * scale, s * scale),
                        cvPoint((h+1) * scale - 1, (s+1) * scale -  1),
                        CV_RGB(intensity, intensity, intensity),
                        CV_FILLED);
        }
    }
    
    // Show original image
    cvNamedWindow("Original Image", 1);
    cvShowImage("Original Image", img_original);

    // Show backprojection
    cvNamedWindow("Back Projection", 1);
    cvShowImage("Back Projection", img_backprojection);
    
    // Show equalized histogram
    cvNamedWindow("Equalized H-S Histogram", 1);
    cvShowImage("Equalized H-S Histogram", img_hist);
    

    
    assert(img_original != 0);
    assert(img_template != 0);
    int iwidth[] = {img_original->width - img_template_smallest->width + 1,
                    img_original->width - img_template_small->width + 1,
                    img_original->width - img_template->width + 1,
                    img_original->width - img_template_big->width + 1,
                    img_original->width - img_template_biggest->width + 1};

    int iheight[] = {img_original->height - img_template_smallest->height + 1,
                    img_original->height - img_template_small->height + 1,
                    img_original->height - img_template->height + 1,
                    img_original->height - img_template_big->height + 1,
                    img_original->height - img_template_biggest->height + 1};
 
    IplImage *match_ccorrNormed[] = {cvCreateImage(cvSize(iwidth[0], iheight[0]), 32, 1),
                                     cvCreateImage(cvSize(iwidth[1], iheight[1]), 32, 1),
                                     cvCreateImage(cvSize(iwidth[2], iheight[2]), 32, 1),
                                     cvCreateImage(cvSize(iwidth[3], iheight[3]), 32, 1),
                                     cvCreateImage(cvSize(iwidth[4], iheight[4]), 32, 1)};

    IplImage *match_coeff[] = {cvCreateImage(cvSize(iwidth[0], iheight[0]), 32, 1),
                               cvCreateImage(cvSize(iwidth[1], iheight[1]), 32, 1),
                               cvCreateImage(cvSize(iwidth[2], iheight[2]), 32, 1),
                               cvCreateImage(cvSize(iwidth[3], iheight[3]), 32, 1),
                               cvCreateImage(cvSize(iwidth[4], iheight[4]), 32, 1)};

    cvMatchTemplate(img_original, img_template_smallest, match_ccorrNormed[0], 3);
    cvMatchTemplate(img_original, img_template_small, match_ccorrNormed[1], 3);
    cvMatchTemplate(img_original, img_template, match_ccorrNormed[2], 3);
    cvMatchTemplate(img_original, img_template_big, match_ccorrNormed[3], 3);
    cvMatchTemplate(img_original, img_template_biggest, match_ccorrNormed[4], 3);

    cvMatchTemplate(img_original, img_template_smallest, match_coeff[0], 4);
    cvMatchTemplate(img_original, img_template_small, match_coeff[1], 4);
    cvMatchTemplate(img_original, img_template, match_coeff[2], 4);
    cvMatchTemplate(img_original, img_template_big, match_coeff[3], 4);
    cvMatchTemplate(img_original, img_template_biggest, match_coeff[4], 4);

    for(int i = 0; i < 5; i++)
    {
        cvNormalize(match_ccorrNormed[i], match_ccorrNormed[i], 1, 0, CV_MINMAX);
        cvNormalize(match_coeff[i], match_coeff[i], 1, 0, CV_MINMAX);


        // DISPLAY
        cvNamedWindow("Template", 0);
        cvShowImage("Template", img_template);
        cvNamedWindow("CCORR_NORMED", 0);
        cvShowImage("CCORR_NORMED", match_ccorrNormed[i]);
        cvNamedWindow("COEFF", 0);
        cvShowImage("COEFF", match_coeff[i]);
        
        cvWaitKey(0);
    }
    cvReleaseImage(&img_original);
    cvReleaseImage(&img_backprojection);
    cvReleaseImage(&img_hist);
    cvReleaseImage(&img_template);

}
