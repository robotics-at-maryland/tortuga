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

// grab all root stuff
#include <TROOT.h>
#include <TH1I.h>
#include <TH3I.h>
#include <TCanvas.h>
#include <TFile.h>

// open cv
#include <cv.h>
#include <highgui.h>

// project includes
#include "image_analyzer.h"

#define CHANNELS 3

int main(int argc, char **argv)
{
    /* check for supplied argument */
    if( argc < 3 ) {
        fprintf( stderr, "Usage: image_analyzer <folder path (must end in /)>"
                 " <filename without extension (must be .png for time being)>\n" );
        return 1;
    }

    // IplImage* img = loadImage(std::string(argv[1]));

    std::string absPath(argv[1]);
    std::string filename(argv[2]);
    std::string extension(".png");

    std::string rMaskStr("_RedBuoyMask");
    std::string yMaskStr("_YellowBuoyMask");
    std::string gMaskStr("_GreenBuoyMask");

    TFile *file = new TFile("results.root", "RECREATE");
    file->cd();

    IplImage *img = loadImage(absPath + filename + extension);
    IplImage *rMask = loadImage(absPath + filename + rMaskStr + extension);
    IplImage *yMask = loadImage(absPath + filename + yMaskStr + extension);
    IplImage *gMask = loadImage(absPath + filename + gMaskStr + extension);

    assert(img != NULL && "Source Image Does Not Exist");
    assert(rMask != NULL && "Red Buoy Binary Mask Images Does Not Exist");
    assert(yMask != NULL && "Yellow Buoy Binary Mask Images Does Not Exist");
    assert(gMask != NULL && "Green Buoy Binary Mask Images Does Not Exist");
    
    int width = img->width;
    int height = img->height;
    int nChannels = img->nChannels;

    unsigned char *imgData = (unsigned char*) img->imageData;
    unsigned char *rMaskData = (unsigned char*) rMask->imageData;
    unsigned char *yMaskData = (unsigned char*) yMask->imageData;
    unsigned char *gMaskData = (unsigned char*) gMask->imageData;

    // Histogram Initialization
    TH3I *h_dataScatterBk = new TH3I("Background Scatter", "Background Scatter", 256, 0, 255, 256, 0, 255, 256, 0, 255);
    TH3I *h_dataScatterRb = new TH3I("Red Buoy Scatter", "Red Buoy Scatter", 256, 0, 255, 256, 0, 255, 256, 0, 255);
    TH3I *h_dataScatterYb = new TH3I("Yellow Buoy Scatter", "Yellow Buoy Scatter", 256, 0, 255, 256, 0, 255, 256, 0, 255);
    TH3I *h_dataScatterGb = new TH3I("Green Buoy Scatter", "Green Buoy Scatter", 256, 0, 255, 256, 0, 255, 256, 0, 255);

    TH1I *h_redBuoyBlueChannel = new TH1I("RB Blue Channel", "RB Blue Channel", 256, 0, 256);
    TH1I *h_redBuoyGreenChannel = new TH1I("RB Green Channel", "RB Green Channel", 256, 0, 256);
    TH1I *h_redBuoyRedChannel = new TH1I("RB Red Channel", "RB Red Channel", 256, 0, 256);
    TH1I *h_redBuoyXCoordTruth = new TH1I("RB X Coordinate Truth", "RB X Coordinate Truth", width, 0, width);
    TH1I *h_redBuoyYCoordTruth = new TH1I("RB Y Coordinate Truth", "RB Y Coordinate Truth", height, 0, height);
    TH1I *h_redBuoyXCoordGuess = new TH1I("RB X Coordinate Guess", "RB X Coordinate Guess", width, 0, width);
    TH1I *h_redBuoyYCoordGuess = new TH1I("RB Y Coordinate Guess", "RB Y Coordinate Guess", height, 0, height);
 
    TH1I *h_yellowBuoyBlueChannel = new TH1I("YB Blue Channel", "YB Blue Channel", 256, 0, 256);
    TH1I *h_yellowBuoyGreenChannel = new TH1I("YB Green Channel", "YB Green Channel", 256, 0, 256);
    TH1I *h_yellowBuoyRedChannel = new TH1I("YB Red Channel", "YB Red Channel", 256, 0, 256);
    TH1I *h_yellowBuoyXCoordTruth = new TH1I("YB X Coordinate Truth", "YB X Coordinate Truth", width, 0, width);
    TH1I *h_yellowBuoyYCoordTruth = new TH1I("YB Y Coordinate Truth", "YB Y Coordinate Truth", height, 0, height);
    TH1I *h_yellowBuoyXCoordGuess = new TH1I("YB X Coordinate Guess", "YB X Coordinate Guess", width, 0, width);
    TH1I *h_yellowBuoyYCoordGuess = new TH1I("YB Y Coordinate Guess", "YB Y Coordinate Guess", height, 0, height);

    TH1I *h_greenBuoyBlueChannel = new TH1I("GB Blue Channel", "GB Blue Channel", 256, 0, 256);
    TH1I *h_greenBuoyGreenChannel = new TH1I("GB Green Channel", "GB Green Channel", 256, 0, 256);
    TH1I *h_greenBuoyRedChannel = new TH1I("GB Red Channel", "GB Red Channel", 256, 0, 256);
    TH1I *h_greenBuoyXCoordTruth = new TH1I("GB X Coordinate Truth", "GB X Coordinate Truth", width, 0, width);
    TH1I *h_greenBuoyYCoordTruth = new TH1I("GB Y Coordinate Truth", "GB Y Coordinate Truth", height, 0, height);
    TH1I *h_greenBuoyXCoordGuess = new TH1I("GB X Coordinate Guess", "GB X Coordinate Guess", width, 0, width);
    TH1I *h_greenBuoyYCoordGuess = new TH1I("GB Y Coordinate Guess", "GB Y Coordinate Guess", height, 0, height);
 
    TH1I *h_backgroundBlueChannel = new TH1I("BK Blue Channel", "BK Blue Channel", 256, 0, 256);
    TH1I *h_backgroundGreenChannel = new TH1I("BK Green Channel", "BK Green Channel", 256, 0, 256);
    TH1I *h_backgroundRedChannel = new TH1I("BK Red Channel", "BK Red Channel", 256, 0, 256);
 
    const unsigned char BLACK = 0;

    // Filter Initialization
    unsigned char rb_ch1_filter[256] = {0};
    unsigned char rb_ch2_filter[256] = {0};
    unsigned char rb_ch3_filter[256] = {0};

    unsigned char yb_ch1_filter[256] = {0};
    unsigned char yb_ch2_filter[256] = {0};
    unsigned char yb_ch3_filter[256] = {0};

    unsigned char gb_ch1_filter[256] = {0};
    unsigned char gb_ch2_filter[256] = {0};
    unsigned char gb_ch3_filter[256] = {0};

    // Red Filter Pass Range
    int rb_ch1_low = 95,  rb_ch3_high = 256;
    int rb_ch2_low = 130, rb_ch2_high = 256;
    int rb_ch3_low = 220, rb_ch1_high = 256;

    // Yellow Filter Pass Range
    int yb_ch1_low = 255, yb_ch1_high = 255;
    int yb_ch2_low = 255, yb_ch2_high = 255;
    int yb_ch3_low = 255, yb_ch3_high = 255;

    // Green Filter Pass Range
    int gb_ch1_low = 255, gb_ch1_high = 255;
    int gb_ch2_low = 255, gb_ch2_high = 255;
    int gb_ch3_low = 255, gb_ch3_high = 255;

    for(int i = 0; i < 256; i++)
    {
        // Red Buoy Filter Setup
        if(i >= rb_ch1_low && i <= rb_ch1_high)
            rb_ch1_filter[i] = 1;

        if(i >= rb_ch2_low && i <= rb_ch2_high)
            rb_ch2_filter[i] = 1;

        if(i >= rb_ch3_low && i <= rb_ch3_high)
            rb_ch3_filter[i] = 1;

        // Yellow Buoy Filter Setup
        if(i >= yb_ch1_low && i <= yb_ch1_high)
            yb_ch1_filter[i] = 1;

        if(i >= yb_ch2_low && i <= yb_ch2_high)
            yb_ch2_filter[i] = 1;

        if(i >= yb_ch3_low && i <= yb_ch3_high)
            yb_ch3_filter[i] = 1;

        // Green Buoy Filter Setup
        if(i >= gb_ch1_low && i <= gb_ch1_high)
            gb_ch1_filter[i] = 1;

        if(i >= gb_ch2_low && i <= gb_ch2_high)
            gb_ch2_filter[i] = 1;

        if(i >= gb_ch3_low && i <= gb_ch3_high)
            gb_ch3_filter[i] = 1;
    }

    for(int idx = 0; idx < width * height; idx++)
    {
        int x_coord = idx % width;
        int y_coord = idx / width;

        if(*rMaskData != BLACK)
        {
            h_redBuoyBlueChannel->Fill(*imgData);
            h_redBuoyGreenChannel->Fill(*(imgData+1));
            h_redBuoyRedChannel->Fill(*(imgData+2));
            h_redBuoyXCoordTruth->Fill(x_coord);
            h_redBuoyYCoordTruth->Fill(y_coord);
        }

        if(*yMaskData != BLACK)
        {
            h_yellowBuoyBlueChannel->Fill(*imgData);
            h_yellowBuoyGreenChannel->Fill(*(imgData+1));
            h_yellowBuoyRedChannel->Fill(*(imgData+2));
            h_yellowBuoyXCoordTruth->Fill(x_coord);
            h_yellowBuoyYCoordTruth->Fill(y_coord);

        }

        if(*gMaskData !=BLACK)
        {
            h_greenBuoyBlueChannel->Fill(*imgData);
            h_greenBuoyGreenChannel->Fill(*(imgData+1));
            h_greenBuoyRedChannel->Fill(*(imgData+2));
            h_greenBuoyXCoordTruth->Fill(x_coord);
            h_greenBuoyYCoordTruth->Fill(y_coord);

        }

        if(*rMaskData == BLACK && *yMaskData == BLACK && *gMaskData == BLACK)
        {
            h_backgroundBlueChannel->Fill(*imgData);
            h_backgroundGreenChannel->Fill(*(imgData+1));
            h_backgroundRedChannel->Fill(*(imgData+2));
        }

        bool redPass = rb_ch1_filter[*imgData] && rb_ch2_filter[*(imgData + 1)] && rb_ch3_filter[*(imgData + 2)];
        bool yellowPass = yb_ch1_filter[*imgData] && yb_ch2_filter[*(imgData + 1)] && yb_ch3_filter[*(imgData + 2)];
        bool greenPass = gb_ch1_filter[*imgData] && gb_ch2_filter[*(imgData + 1)] && gb_ch3_filter[*(imgData + 2)];

        if(redPass)
        {
            h_dataScatterRb->Fill(*imgData, *(imgData+1), *(imgData+2));
            h_redBuoyXCoordGuess->Fill(x_coord);
            h_redBuoyYCoordGuess->Fill(y_coord);
        }
        
        if(yellowPass)
        {
            h_dataScatterYb->Fill(*imgData, *(imgData+1), *(imgData+2));
            h_yellowBuoyXCoordGuess->Fill(x_coord);
            h_yellowBuoyYCoordGuess->Fill(y_coord);
        }

        if(greenPass)
        {
            h_dataScatterGb->Fill(*imgData, *(imgData+1), *(imgData+2));
            h_greenBuoyXCoordGuess->Fill(x_coord);
            h_greenBuoyYCoordGuess->Fill(y_coord);
        }

        if(!(redPass || greenPass || yellowPass))
            h_dataScatterBk->Fill(*imgData, *(imgData+1), *(imgData+2));

        imgData +=3;
        rMaskData +=3;
        yMaskData +=3;
        gMaskData +=3;
    }

    // Change marker colors
    h_dataScatterRb->SetMarkerColor(kRed);
    h_dataScatterYb->SetMarkerColor(kYellow);
    h_dataScatterGb->SetMarkerColor(kGreen);

    // Save the scatter plots to images
    TCanvas c1("c1");
    h_dataScatterBk->Draw();
    h_dataScatterRb->Draw("same");
    h_dataScatterYb->Draw("same");
    h_dataScatterGb->Draw("same");
    std::string outputBaseName = absPath + filename + "_PixelSpace";
    std::string picExt(".png");
    std::string rootExt(".root");
    std::string outputPicName = outputBaseName + picExt;
    std::string outputRootName = outputBaseName + rootExt;
    c1.Print(outputPicName.c_str());
    c1.Print(outputRootName.c_str());

    file->Write();
   
    return 0;
}

IplImage* loadImage(std::string filepath)
{
    /* data structure for the image */
    IplImage *img = 0;

    /* load the image */
    img = cvLoadImage( filepath.c_str(), CV_LOAD_IMAGE_COLOR );

    /* always check */
    if(img == 0) {
        fprintf(stderr, "Cannot load file %s!\n", filepath.c_str());
        return NULL;
    }

    return img;
}
