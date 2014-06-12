/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/LineDetector.cpp
*/ 

// STD Includes
#include <algorithm>
#include <iostream>

// Library Includes
#include <UnitTest++/Checks.h>

// Project Includes
#include "core/include/PropertySet.h"
#include "vision/include/Image.h"
#include "vision/include/LineDetector.h"
#include "vision/include/OpenCVImage.h"

namespace ram {
namespace vision {

LineDetector::LineDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_minLineSize(0),
    data(0),
    m_dataSize(0)
{
    init(config);
}

LineDetector::LineDetector(int minimumLineSize) :
    Detector(core::EventHubPtr()),
    m_minLineSize(minimumLineSize),
    data(0),
    m_dataSize(0)
{
}

LineDetector::~LineDetector()
{
    delete data;
}

void LineDetector::init(core::ConfigNode config)
{
    core::PropertySetPtr propSet(getPropertySet());
    propSet->addProperty(config, false, "highThreshold",
                         "High threshold for canny edge detector",
                         200.0, &m_highThreshold, 0.0, 255.0);

    propSet->addProperty(config, false, "lowThreshold",
                         "Low threshold for canny edge detector",
                         .4*200, &m_lowThreshold, 0.0, 255.0);

    propSet->addProperty(config, false, "houghThreshold",
                         "Threshold for the hough transform",
                         100, &m_houghThreshold, 1, 255);

    propSet->addProperty(config, false, "maxLines",
                         "Maximum number of lines to find",
                         10, &m_maxLines);

    propSet->addProperty(config, false, "minLineSize",
                         "Minimum size of a line",
                         0.0, &m_minLineSize);

    propSet->addProperty(config, false, "maxGapLength",
                         "Maximum gap between two lines to be "
                         "considered the same", 0.0, &m_maxGapLength);

    propSet->addProperty(config, false, "maxRhoGap",
			 "Maximum rho difference two lines to endpoints to be"
			 " considered the same", 3.0, &m_rhoGap);

    propSet->addProperty(config, false, "maxSquareGap",
			 "Maximum distance between two lines to endpoints to be"
			 " considered the same", 3, &m_squareGap);


    /// TODO: Load this value
    m_thetaGap = math::Radian(0.005);

    data = new vision::OpenCVImage(640, 480, vision::Image::PF_GRAY_8);

    //propSet->verifyConfig(config, true);
}

void LineDetector::processImage(Image* input, Image* output)
{
    // Clear the line list
    m_lines.clear();

    // Grayscale images only
    assert(input->getPixelFormat() == Image::PF_GRAY_8 &&
           "LineDetector: processImage(): input must be a grayscale image");
    data->setSize((int)input->getWidth(), (int)input->getHeight());
   
    cvCanny(input->asIplImage(), data->asIplImage(),
            m_lowThreshold, m_highThreshold);
    houghTransform();

    // Sort based on theta, then rho
    size_t lineNum = m_lines.size();
    if (lineNum > 0) {
        std::sort(m_lines.begin(), m_lines.end(),
                  LineDetector::LineComparer::compare);
    }

    bool changed = true;
    while (changed) {
        // Assume no merges will take place
        changed = false;
	
	// Merge lines that are the same (opencv is stupid)
	lineNum = m_lines.size();
	for (size_t i=1; i<lineNum; i++) {
	    // Check the next line to see if it's roughly the same line
            Line expected = m_lines[i-1];
	    Line actual = m_lines[i];
	    bool sameRho = UnitTest::AreClose(expected.rho(), actual.rho(),
					      m_rhoGap);
	    bool sameTheta = UnitTest::AreClose(expected.theta(), 
						actual.theta(),
						m_thetaGap);
	    bool samePt1X = UnitTest::AreClose(expected.point1().x,
                                           actual.point1().x,
					       m_squareGap);
	    bool samePt1Y = UnitTest::AreClose(expected.point1().y,
					       actual.point1().y,
					       m_squareGap);
	    bool samePt2X = UnitTest::AreClose(expected.point2().x,
					       actual.point2().x,
					       m_squareGap);
	    bool samePt2Y = UnitTest::AreClose(expected.point2().y,
					       actual.point2().y,
					       m_squareGap);

	    // If both are true, merge
	    if (sameRho & sameTheta & samePt1X & samePt1Y & samePt2X & samePt2Y)
	    {
                // We are merging lines, so we need another pass
                changed = true;

                // Rest of the code
	        CvPoint pt1, pt2;
		double rho;
		math::Radian theta;
		
		// Remove the two original and average them
		m_lines.erase(m_lines.begin() + i);
		m_lines.erase(m_lines.begin() + i - 1);

		// Find the averages
		pt1.x = (expected.point1().x + actual.point1().x) / 2;
		pt1.y = (expected.point1().y + actual.point1().y) / 2;
		pt2.x = (expected.point2().x + actual.point2().x) / 2;
		pt2.y = (expected.point2().y + actual.point2().y) / 2;

		rho = (expected.rho() + actual.rho()) / 2;
		theta = (expected.theta() + actual.theta()) / 2;
		
		m_lines.insert(m_lines.begin() + i - 1,
			       LineDetector::Line(pt1, pt2, theta, rho));
		// One less line
		lineNum -= 1;
            } // if (should merge)
        } // foreach (line in m_lines)
    } // while(changed)

    // Debug stuff
    if (0 != output)
    {
        //output->copyFrom(input);
        // Copy filtered data into an RGB image
        output->copyFrom(data);
        output->setPixelFormat(Image::PF_RGB_8);

        BOOST_FOREACH(LineDetector::Line line, m_lines)
        {
            CvPoint pt1, pt1BoundUR, pt1BoundLL;
            CvPoint pt2, pt2BoundUR, pt2BoundLL;
            int boxsize = 3;
            int linesize = 1;

            pt1 = line.point1();
            pt1BoundUR.x = pt1.x + boxsize;
            pt1BoundUR.y = pt1.y - boxsize;
            pt1BoundLL.x = pt1.x - boxsize;
            pt1BoundLL.y = pt1.y + boxsize;
            pt2 = line.point2();
            pt2BoundUR.x = pt2.x + boxsize;
            pt2BoundUR.y = pt2.y - boxsize;
            pt2BoundLL.x = pt2.x - boxsize;
            pt2BoundLL.y = pt2.y + boxsize;

            // Draw a small box around the points
            cvRectangle(output->asIplImage(), pt1BoundUR, pt1BoundLL,
                        CV_RGB(0,255,0), 2, CV_AA, 0);
            cvRectangle(output->asIplImage(), pt2BoundUR, pt2BoundLL,
                        CV_RGB(0,255,0), 2, CV_AA, 0);

            // Draw the line
            cvLine(output->asIplImage(), pt1, pt2,
                   CV_RGB(0,0,255), linesize);
        }
    }
}

int LineDetector::houghTransform()
{
    // Create memory storage for cvHoughLines2
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;

    // ThetaThreshold = 0.5, RhoThreshold = 0.5
    lines = cvHoughLines2(data->asIplImage(),
                          storage,
                          CV_HOUGH_PROBABILISTIC,
                          0.5, CV_PI/360, m_houghThreshold,
                          m_minLineSize, m_maxGapLength); // Ignored if STANDARD

    // Convert these into our format
    for (int i = 0; i < std::min(lines->total, m_maxLines); i++ )
    {
        // Copied from OpenCV documentation
        CvPoint* line = (CvPoint *) cvGetSeqElem(lines, i);
        CvPoint pt1 = line[0];
        CvPoint pt2 = line[1];
        double rho;
        double theta;
        
        
          // TO FIND THETA:
          // The atan2 function allows us to do atan(y/x) without
          // division by 0 errors.

         //  range of theta: [-pi/2, pi/2)
          // (more on why -pi/2 is included, but pi/2 isn't later)

          // For every other situation, we know this:
         //  m = -(cos(theta)/sin(theta))
         //  m = -1/tan(theta)
         //  theta = atan(-1/m)
         //  m = rise/run
         //  m = (y2 - y1) / (x2 - x1)

         //  The division by 0 error when m = 0 is handled by atan2.
         //  atan2(-1, 0) = -pi/2
         //  (because it's a vector pointing straight down)

         //  This will give us the value of theta in radians.

         //  Now for the sake of simplicity in the rho calculation, we want to
         //  change the range to [0, pi). We can do this pretty simply by
         //  just adding pi to the theta value as long as it isn't negative.

          // After finding theta, rho is really easy.
         //  TO FIND RHO:
         //  b = rho/sin(theta)
          // rho = b*sin(theta)
                       
         //  y = mx + b
        //   b = y - mx (use one of the points for [x,y])
         //  Special case when theta = 0
         //  sin(0) = 0, rho = b*0 would mean rho = 0, which is wrong
         //  When theta = 0, m = infinity

         // When theta = 0, then it's a vertical line. The perpendicular
         //  line is then the horizontal line. The x values should
          // be the same, take one and use that as the rho in this case.
        
        float m = ((float) (pt1.y - pt2.y)) / ((float) (pt1.x - pt2.x));
        theta = atan2(-1, m);
        while (theta < 0) theta += CV_PI;

        if (theta != 0) {
            // General case
            float b = pt1.y - m*pt1.x;
            rho = b*sin(theta);
        } else {
            // theta = 0
            rho = pt1.x;
        }
        // Yes, that huge explanation was just for that

        m_lines.push_back(
            LineDetector::Line(pt1, pt2, math::Radian(theta), rho));
    }

    return m_lines.size();
}

void LineDetector::setMinimumLineSize(int pixels)
{
    m_minLineSize = pixels;
}

int LineDetector::getMinimumLineSize()
{
    return m_minLineSize;
}

std::vector<LineDetector::Line> LineDetector::getLines()
{
    return m_lines;
}

} // namespace vision
} // namespace ram

