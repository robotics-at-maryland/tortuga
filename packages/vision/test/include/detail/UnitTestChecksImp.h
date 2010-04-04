/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/test/include/detail/UnitTestChecksImp.h
 */

#ifndef RAM_VISION_TEST_UNITTESTCHECKSIMP_H_01_23_2008
#define RAM_VISION_TEST_UNITTESTCHECKSIMP_H_01_23_2008


// Library Includes
#include "UnitTest++/Checks.h"
#include "cv.h"

// Project Includes
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"

namespace UnitTest {
    
template<>
void CheckClose(TestResults& results,
                ram::vision::Image const& expected,
                ram::vision::Image const& actual,
                double const& tolerance,
                TestDetails const& details)
    
{
    UnitTest::MemoryOutStream stream;

    // Check Width and Height
    if (expected.getWidth() != actual.getWidth())
    {
        stream << "Expected image width " << expected.getWidth()
               << " but was " << actual.getWidth();
    
        results.OnTestFailure(details, stream.GetText());
        
        return;
    }

    if (expected.getHeight() != actual.getHeight())
    {
        stream << "Expected image height " << expected.getHeight()
               << " but was " << actual.getHeight();
    
        results.OnTestFailure(details, stream.GetText());
        
        return;
    }

    // Check Pixel Format
    if (expected.getPixelFormat() != actual.getPixelFormat())
    {
        stream << "Expected image pixel format " << expected.getPixelFormat()
               << " but was " << actual.getPixelFormat();
    
        results.OnTestFailure(details, stream.GetText());
        
        return;
    }

    int length = expected.getHeight() * expected.getWidth() *
        expected.getNumChannels();

    // Blur Images to allow for easier comparisions
    ram::vision::OpenCVImage blurredExpected(expected.getWidth(),
                                             expected.getHeight(),
                                             expected.getPixelFormat());
    ram::vision::OpenCVImage blurredActual(actual.getWidth(),
                                           actual.getHeight(),
                                           actual.getPixelFormat());
    // Zero Memory to start (keeps valgrind happy)
    memset(blurredExpected.getData(), 0, length);
    memset(blurredActual.getData(), 0, length);

    // Do a standard gaussion blur
    cvSmooth(expected.asIplImage(), blurredExpected.asIplImage(), CV_GAUSSIAN);
    cvSmooth(actual.asIplImage(), blurredActual.asIplImage(), CV_GAUSSIAN);
    
    // Now Compare Image buffers
    double totalDifference = 0;
    unsigned char* expectedData = blurredExpected.getData();
    unsigned char* actualData = blurredActual.getData();

    double difference = 0;
    for (int i = 0; i < length; ++i)
    {
        // Get absolute value of the difference
        difference = static_cast<double>(expectedData[i] - actualData[i]);
        if (difference < 0)
        {
            difference = -difference;
        }
        
        totalDifference += difference;
    }

    double averageDifference = totalDifference / (double)length;
    
    if (averageDifference > tolerance)
    {
        stream << "Average pixel difference expected to be below "
               << tolerance << " but was " << averageDifference;
    
        results.OnTestFailure(details, stream.GetText()); 
    }
}

template<>
void CheckClose(TestResults& results,
                ram::vision::Image const& expected,
                ram::vision::Image const& actual,
                int const& tolerance,
                TestDetails const& details)
{
    CheckClose(results, expected, actual, static_cast<double>(tolerance),
               details);
}

template<>
void CheckClose(TestResults& results,
                ram::vision::OpenCVImage* const& expected,
                ram::vision::OpenCVImage* const& actual,
                double const& tolerance,
                TestDetails const& details)
{
    CheckClose(results,
               *((ram::vision::Image*)(expected)),
               *((ram::vision::Image*)(actual)),
               tolerance, details);
}

template<>
void CheckClose(TestResults& results,
                ram::vision::OpenCVImage* const& expected,
                ram::vision::OpenCVImage* const& actual,
                int const& tolerance,
                TestDetails const& details)
{
    CheckClose(results,
               *((ram::vision::Image*)(expected)),
               *((ram::vision::Image*)(actual)),
               static_cast<double>(tolerance),
               details);
}

} // namespace UnitTest    

#endif // RAM_VISION_TEST_UNITTESTCHECKSIMP_H_01_23_2008
