/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/src/TestSWScale.cxx
 */


// Library Includes
#include <UnitTest++/UnitTest++.h>

extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"

#include "vision/test/include/UnitTestChecks.h"
#include "vision/test/include/Utility.h"

using namespace ram;

SUITE(SWScale) {

TEST(BGR_YUV420P)
{
    vision::OpenCVImage start(640, 480);
    vision::OpenCVImage yuv420P(640, 480);
    vision::OpenCVImage result(640, 480);

    
    vision::makeColor(&start, 160, 70, 30);
    vision::drawCircle(&start, 50, 50, 25, CV_RGB(255,0,0));
    vision::drawCircle(&start, 100, 100, 25, CV_RGB(0,255,0));
    vision::drawCircle(&start, 300, 300, 25, CV_RGB(0,0,255));
    
    vision::makeColor(&yuv420P, 255, 255, 255);
    vision::makeColor(&result, 255, 255, 255);

    
    int middleFormat = PIX_FMT_YUV420P;
    struct SwsContext *BGRToYUV420P = sws_getContext(640, 480,
                                                     PIX_FMT_BGR24,
                                                     640, 480,
                                                     middleFormat,
                                                     SWS_BICUBIC, NULL, NULL, NULL);
    struct SwsContext *YUV420PToBGR = sws_getContext(640, 480,
                                                     middleFormat,
                                                     640, 480,
                                                     PIX_FMT_RGB24, //YUV420P,
                                                     SWS_BICUBIC, NULL, NULL, NULL);

    
    // Use ffmpeg to generate our needed values
    AVPicture startPicture;
    avpicture_fill(&startPicture, start.getData(), PIX_FMT_BGR24,
                   start.getWidth(), start.getHeight());
    AVPicture yuvPicture;
    avpicture_fill(&yuvPicture, yuv420P.getData(), middleFormat, //YUV420P,
                   yuv420P.getWidth(), yuv420P.getHeight());

    int ret = sws_scale(BGRToYUV420P, startPicture.data, startPicture.linesize,
                        0, 480, yuvPicture.data, yuvPicture.linesize);
    CHECK_EQUAL(0, ret);

    // Now lets go back
    AVPicture resultPicture;
    avpicture_fill(&resultPicture, result.getData(), PIX_FMT_RGB24,
                   result.getWidth(), result.getHeight());
    
    ret = sws_scale(YUV420PToBGR, yuvPicture.data, yuvPicture.linesize,
              0, 480, resultPicture.data, resultPicture.linesize);
    CHECK_EQUAL(0, ret);

//    vision::Image::showImage(&start, "Start");
//    vision::Image::showImage(&yuv420P, "YUV420P");
//    vision::Image::showImage(&result, "Result");

    // Free up resources
    sws_freeContext(BGRToYUV420P);
    sws_freeContext(YUV420PToBGR);

    // Check results
    // TODO: This currently fails
}

} // SUITE(SWScale)
