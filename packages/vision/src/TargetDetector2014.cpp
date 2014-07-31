/*
 * Copyright (C) 2014 Robotics at Maryland
 * Copyright (C) 2014 Eliot Rudnick-Cohen
 * All rights reserved.
 *
 * Author: Eliot Rudnick-Cohen <erudnick@umd.edu>
 * File:  packages/vision/include/TargetDetector.cpp
 */

// STD Includes
#include <algorithm>
#include <cmath>
#include <sstream>

// Library Includes
#include "highgui.h"
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <log4cpp/Category.hh>

// Project Includes
#include "vision/include/TargetDetector2014.h"
#include "vision/include/main.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Events.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/Utility.h"

#include "core/include/PropertySet.h"

#ifndef M_PI
#define M_PI 3.14159
#endif
#ifndef PI
#define PI 3.14159265
#endif

namespace ram {
namespace vision {
namespace vision2014 {

static log4cpp::Category& logger(log4cpp::Category::getInstance("CupidDetector"));

    
TargetDetector::TargetDetector(core::ConfigNode config,
                               core::EventHubPtr eventHub) :
    Detector(eventHub),
    rng(12345),
    m_redFilter(new ColorFilter(0, 255, 0, 255, 0, 255)),
    m_image(new OpenCVImage(640, 480)),
    m_found(false),
    m_targetCenterX(0),
    m_targetCenterY(0),
    m_squareNess(0),
    m_range(0),
    m_minGreenPixels(0),
    m_erodeIterations(0),
    m_dilateIterations(0),
    m_maxAspectRatio(0),
    m_topRemovePercentage(0),
    m_bottomRemovePercentage(0)
{
    logger.info("Starting Target Detector");
    init(config);
}
    
void TargetDetector::init(core::ConfigNode config)
{
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());


    //propSet->addProperty(config, false, "topRemovePercentage",
    //                     "% of the screen from the top to be blacked out",
    //                     0.0, &m_topRemovePercentage, 0.0, 1.0);
    propSet->addProperty(config, false, "threshold block size",
                         "block size for adaptive thresholding",
                         5, &bSize, 1, 50);
    propSet->addProperty(config, false, "bilateral d param",
                         "d value for bilateral filter(NOTE: affects algorithm speed)",
                         5, &filterSize, 1, 15);
    propSet->addProperty(config, false, "color sigma",
                         "sigma for color in bilateral filter",
                         5.0, &colorSigma, 0.0, 150.0);
    propSet->addProperty(config, false, "space sigma",
                         "sigma for colorspace in bilateral filter",
                         5.0, &spaceSigma, 0.0, 150.0);
    propSet->addProperty(config, false, "c Hue",
                         "Hue constant for adaptive threshold",
                         1, &cH, -10, 10);
    propSet->addProperty(config, false, "c Saturation",
                         "saturation constant for adaptive threshold",
                         1, &cS, -10, 10);
    propSet->addProperty(config, false, "c Value",
                         "value constant for adaptive threshold",
                         1, &cV, -10, 10);
    
    threshType = CV_ADAPTIVE_THRESH_GAUSSIAN_C;

    propSet->addProperty(config, false, "closing iterations",
                         "closing iterations",
                         1, &morphIterations, 0, 10);

    propSet->addProperty(config, false, "dilate iterations",
                         "dilate iterations",
                         1, &dilateCount, 0, 10);

    propSet->addProperty(config, false, "erode iterations",
                         "erode iterations",
                         0, &erodeCount, 0, 10);

    propSet->addProperty(config, false, "xy binning range",
                         "xy binning range",
                         0.0, &xyRange, 0.0, 100.0);
    propSet->addProperty(config, false, "size binning range",
                         "size binning range",
                         0.0, &sizeRange, 0.0, 100.0);
    
    morph_elem = 0;//0 rect, 1 circle, 2 ellipse
    morph_size = 2;

   propSet->addProperty(config, false, "circularity bound",
                         "circle bound",
                         .35, &circularityBound, 0.0, 3.0);  
  
   propSet->addProperty(config, false, "rectangularity bound",
                         "rectangle bound",
                         .5, &rectangularityBound, 0.0, 3.0); 

   propSet->addProperty(config, false, "minimum radius",
                         "minimum radius",
                         10.0, &minR, 0.0, 500.0); 

   propSet->addProperty(config, false, "maximum radius",
                         "maximum radius",
                         200.0, &maxR, 0.0, 500.0); 

   propSet->addProperty(config, false, "min vertices",
                         "minimum number of vertices in valid contour",
                         10, &numPointsThreshold, 0, 100);     

   propSet->addProperty(config, false, "debug Images?",
                         "debug Images?",
                        0, &debugOut, 0, 1);
    // Make sure the configuration is valid
    //propSet->verifyConfig(config, true);
}
    
TargetDetector::~TargetDetector()
{
    delete m_image;
  //  delete m_redFilter;
  //  delete m_greenFilter;
  //  delete m_yellowFilter;
   // delete m_blueFilter;
}

bool TargetDetector::found()
{
    return m_found;
}

double TargetDetector::getX()
{
    return m_targetCenterX;
}

double TargetDetector::getY()
{
    return m_targetCenterY;
}

void TargetDetector::setTopRemovePercentage(double percent)
{
    m_topRemovePercentage = percent;
}

void TargetDetector::setBottomRemovePercentage(double percent)
{
    m_bottomRemovePercentage = percent;
}
 
   
void TargetDetector::processImage(Image* input, Image* output)
{
    //contour storage variables, maintained throughout code
    std::vector<std::vector<cv::Point> > prunedContours;
    std::vector<std::vector<cv::Point> > circleContours;
    std::vector<std::vector<cv::Point> > rectangleContours;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    //read input image
    cv::Mat image =  input->asIplImage();
    //change colorspace to hsv
    cv::Mat imageHSV;
    
    cv::cvtColor(image, imageHSV, CV_BGR2HSV);
    //blur image
    cv::Mat blurred;
    //removed gaussianblur to use bilateral filter instead
    //cv::GaussianBlur(imageHSV,blurred,cv::Size(bSize,bSize),blurSigma);

    //competition day 1, switched from HSV to BGR(no seriously, i'm not kidding)
    //cv::bilateralFilter(imageHSV, blurred, filterSize,colorSigma, spaceSigma);
    cv::bilateralFilter(image, blurred, filterSize,colorSigma, spaceSigma);


    //split the channels of the image
    std::vector<cv::Mat> hsvChannels;
    cv::split(blurred, hsvChannels); 
    //perform adaptive thresholding on each channel
    //Hue thresholding
    cv::Mat hThresh;
    cv::adaptiveThreshold(hsvChannels[0], hThresh,255,threshType,CV_THRESH_BINARY_INV,bSize,cH);
    //Saturation thresholding
    cv::Mat sThresh;
    cv::adaptiveThreshold(hsvChannels[1], sThresh,255,threshType,CV_THRESH_BINARY_INV,bSize,cS);
    //Value thresholding
    cv::Mat vThresh;
    cv::adaptiveThreshold(hsvChannels[2], vThresh,255,threshType,CV_THRESH_BINARY_INV,bSize,cV); 
      
    /*
    //perform otsu thresholding on each channel
    //Hue thresholding
    cv::Mat hThresh;
    cv::threshold(hsvChannels[0], hThresh,0,255,CV_THRESH_BINARY_INV+CV_THRESH_OTSU);
    //Saturation thresholding
    cv::Mat sThresh;
    cv::threshold(hsvChannels[1], sThresh,0,255,CV_THRESH_BINARY+CV_THRESH_OTSU);
    //Value thresholding
    cv::Mat vThresh;
    cv::threshold(hsvChannels[2], vThresh,0,255,CV_THRESH_BINARY_INV+CV_THRESH_OTSU); */

    //Initial MC results indicated value is the only decent channel, hue can have things, but full of noise
    //may be due to technique used for image generation
    //for now only value will be going forwards, can always OR it with another channel in future

    //competition day 1,, now using BGR, so looking for green channel, not red
    //cv::Mat threshResult = vThresh;
    cv::Mat threshResult = sThresh;

    //apply edge detector to result
    cv::Mat edges;

    //don't need to bother with canny, so removed it
    //cv::Canny(threshResult,edges,cannyT1, cannyT2);
    
    edges = threshResult;

    //forcibly zero a frame about the image to keep edge of image from becoming a contour
    edges.rowRange(0,10).setTo(cv::Scalar(0));
    edges.colRange(630,640).setTo(cv::Scalar(0));
    edges.colRange(0,10).setTo(cv::Scalar(0));
    edges.rowRange(470,480).setTo(cv::Scalar(0));
    

    //run primary contour check, identify any strong contours immediately present in edges
    //look for contours in the image
    cv::findContours( edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_KCOS, cv::Point(0, 0) );
    //prune invalid contours from list, also categorize, before any morphological transforms
    categorizeContours(contours, prunedContours, circleContours, rectangleContours);


    cv::Mat morphEdges;
    //apply closings to value channel
    cv::Mat element = cv::getStructuringElement( morph_elem, cv::Size( 2*morph_size + 1, 2*morph_size+1 ), cv::Point( morph_size, morph_size ) );
    cv::morphologyEx(edges, morphEdges, cv::MORPH_CLOSE, element, cv::Point(-1,-1), morphIterations);
    
    //post closing contour check
    //look for contours in the image
    cv::findContours( morphEdges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    //prune invalid contours from list, also categorize, post morphological transform, pre-erode-dilate cycle
    categorizeContours(contours, prunedContours, circleContours, rectangleContours);

    //apply dilates
    cv::Mat closeEdges;
    //apply dilates to value channel
    cv::dilate(morphEdges, closeEdges, element,cv::Point(-1,-1), dilateCount);
    //apply erosion
    cv::Mat erodeEdges;
    //apply erosions to value channel
    cv::erode(closeEdges, erodeEdges, element,cv::Point(-1,-1), erodeCount);

    //look for contours in the image
    cv::findContours( erodeEdges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    //prune invalid contours from list, also categorize, post morphological transform
    categorizeContours(contours, prunedContours, circleContours, rectangleContours);

    //DEBUG
    if(debugOut == 1)
    {
        cv::imshow("hsvImage",imageHSV);
        cv::imshow("filteredImage",blurred);
        cv::imshow("hThresh", hThresh);
        cv::imshow("sThresh", sThresh);
        cv::imshow("vThresh", vThresh);
        cv::imshow("edges", edges);
        cv::imshow("closed", morphEdges);
        cv::Mat drawing = cv::Mat::zeros( morphEdges.size(), CV_8UC3 );
        for( unsigned int i = 0; i< contours.size(); i++ )
        {
            cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            cv::drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
        }
        imshow("contours", drawing);
        //draw contours onto original image
        drawing = image;//cv::Mat::zeros( morphEdges.size(), CV_8UC3 );
        for( unsigned int i = 0; i< circleContours.size(); i++ )
        {
            cv::Scalar color = cv::Scalar( 255,255,255 );
            cv::drawContours( drawing, circleContours, i, color, 2, 8, hierarchy, 0, cv::Point() );
        }
        for( unsigned int i = 0; i< rectangleContours.size(); i++ )
        {
            cv::Scalar color = cv::Scalar( 0,0,0 );
            cv::drawContours( drawing, rectangleContours, i, color, 2, 8, hierarchy, 0, cv::Point() );
            //std::cout<<cv::arcLength(rectangleContours[i],true)<<std::endl;
        }
        imshow("pruned contours", drawing);
    }


    std::vector<Hole> holes;
    for(unsigned int i = 0; i< circleContours.size(); i++)
    {
        float idealRad;
        cv::Point2f center;
        cv::minEnclosingCircle(circleContours[i], center, idealRad);
        Hole h;
        h.x = center.x;
        h.y = center.y;
        h.size = idealRad;
        holes.push_back(h);
    }
    std::vector<std::vector<Hole> > sizeBinned;
    //std::cout<<"swordfishx"<<std::endl;
    TargetDetector::BinHolesSize(holes, sizeRange,sizeBinned);
    //std::cout<<"swordfishz"<<std::endl;
    //for(unsigned int i = 0; i < sizeBinned.size(); i++)
    //{
    //    std::cout<<"------------------------------"<<std::endl;
    //    for(unsigned int j =0; j < sizeBinned[i].size(); j++)
    //    {
    //        std::cout<<sizeBinned[i][j].x<<" "<<sizeBinned[i][j].y<<" "<<sizeBinned[i][j].size<<std::endl;
    //    }
    //}
    //std::cout<<"SWORDFISH FINALLY"<<std::endl;
    std::vector<std::vector<Hole> > xyBinned;
    holes.clear();
    for(unsigned int i = 0; (i < 2 && i < sizeBinned.size()); i++)
    {
        holes.insert(holes.begin(),sizeBinned[i].begin(),sizeBinned[i].end());
    }
    TargetDetector::BinHolesXY(holes, xyRange,xyBinned);
    //for(unsigned int i = 0; i < xyBinned.size(); i++)
    //{
    //    std::cout<<"------------------------------"<<std::endl;
    //    for(unsigned int j =0; j < xyBinned[i].size(); j++)
    //    {
    //        std::cout<<xyBinned[i][j].x<<" "<<xyBinned[i][j].y<<" "<<xyBinned[i][j].size<<std::endl;
    //    }
    //}
    //AND NOW, WE CATEGORIZE THE BINS
    //ASCII CAT GOES HERE
    
    //the three holes are leftmost, rightmost and downmost
    //they are allowed to be the same thing
    //this is an awesome way of doing things, as since the bins were only the two largest hole sizes, this is pretty accurate
    Hole leftmost;
    leftmost.x = 10000;
    Hole rightmost;
    rightmost.x = -1;
    Hole downmost;
    downmost.y = -1;
    holes.clear();
    for(unsigned int i = 0; i < xyBinned.size(); i++)
    {
        std::sort(xyBinned[i].begin(), xyBinned[i].end(), compareSize);
        //competition day 1, changed to appropriate corner distances
        /*if(xyBinned[i][0].x < leftmost.x)
        {
            leftmost = xyBinned[i][0];
        }
        if(xyBinned[i][0].x > rightmost.x)
        {
            rightmost = xyBinned[i][0];
        }
        */
        if(TargetDetector::computeSquaredCornerDistance(0,0,xyBinned[i][0]) < TargetDetector::computeSquaredCornerDistance(0,0,leftmost))
        {
            leftmost = xyBinned[i][0];
        }
        if(TargetDetector::computeSquaredCornerDistance(640,0,xyBinned[i][0]) < TargetDetector::computeSquaredCornerDistance(640,0,rightmost))
        {
            rightmost = xyBinned[i][0];
        }  
        if(xyBinned[i][0].y > downmost.y)
        {
            downmost = xyBinned[i][0];
        }
    }
    std::cout<<leftmost.x<<" "<<leftmost.y<<" "<<leftmost.size<<std::endl;
    std::cout<<rightmost.x<<" "<<rightmost.y<<" "<<rightmost.size<<std::endl;
    std::cout<<downmost.x<<" "<<downmost.y<<" "<<downmost.size<<std::endl;
    
    double tx;
    double ty;
	Detector::imageToAICoordinates(input, 
	              (int)leftmost.x,
	              (int)leftmost.y,
	              tx,
                  ty);
    leftHole = leftmost;
    leftHole.x = tx;
    leftHole.y = ty;
	Detector::imageToAICoordinates(input, 
	              (int)rightmost.x,
	              (int)rightmost.y,
	              tx,
                  ty);
    rightHole = rightmost;
    rightHole.x = tx;
    rightHole.y = ty;
	Detector::imageToAICoordinates(input, 
	              (int)downmost.x,
	              (int)downmost.y,
	              tx,
                  ty);
    downHole = downmost;
    downHole.x = tx;
    downHole.y = ty;

    TargetEventPtr event(new TargetEvent());

    //grab square contour info
    if(rectangleContours.size() > 0)
    {
        cv::RotatedRect r = minAreaRect(rectangleContours[0]);
        event->x = r.center.x;
        event->y = r.center.y;
        event->range = r.size.width;
    }
    if(leftmost.x < 1000)
    {
        event->leftx = leftHole.x;
        event->lefty = leftHole.y;
        event->leftsize = leftHole.size;
    }
    if(rightmost.x > -1)
    {
        event->rightx = rightHole.x;
        event->righty = rightHole.y;
        event->rightsize = rightHole.size;
    }
    if(downmost.y > -1)
    {
        event->downx = downHole.x;
        event->downy = downHole.y;
        event->downsize = downHole.size;
    }
    publish(EventType::TARGET_FOUND, event);




    //input->setData(img_whitebalance.data,false);
    if (output)
    {
        cv::Mat drawing = cv::Mat::zeros( morphEdges.size(), CV_8UC3 );
        for(unsigned int i = 0; i< contours.size(); i++ )
        {
            cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            cv::drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
        }
        //draw contours onto original image
        drawing = image;//cv::Mat::zeros( morphEdges.size(), CV_8UC3 );
        for(unsigned int i = 0; i< circleContours.size(); i++ )
        {
            cv::Scalar color = cv::Scalar( 255,255,255 );
            cv::drawContours( drawing, circleContours, i, color, 2, 8, hierarchy, 0, cv::Point() );
        }
        for(unsigned int i = 0; i< rectangleContours.size(); i++ )
        {
            cv::Scalar color = cv::Scalar( 0,0,0 );
            cv::drawContours( drawing, rectangleContours, i, color, 2, 8, hierarchy, 0, cv::Point() );
            //std::cout<<cv::arcLength(rectangleContours[i],true)<<std::endl;
        }
        cv::Scalar color = cv::Scalar( 0,0,255 );
        cv::circle(drawing,cv::Point2d(leftmost.x, leftmost.y), leftmost.size, color,-1);
        cv::circle(drawing,cv::Point2d(rightmost.x, rightmost.y), rightmost.size, color,-1);
        color = cv::Scalar( 0,255,0 );
        cv::circle(drawing,cv::Point2d(downmost.x, downmost.y), downmost.size, color,-1);
        input->setData(drawing.data,false);
        output->copyFrom(input);
	}



}

void TargetDetector::publishFoundEvent()
{/*
logger.infoStream() << "Publish Event: Center "<<m_targetCenterX <<", "<<m_targetCenterY<< 
				" large center: "<<m_targetLargeCenterX <<", "<<m_targetLargeCenterY<<
				" small center: "<<m_targetSmallCenterX <<", "<<m_targetSmallCenterY<<
				" Squareness: "<<m_squareNess <<", range: "<<m_range<<
				" Flags: Large: "<<m_largeflag <<", small: "<<m_smallflag<<
				" ranges: "<<m_rangelarge<<", small: "<<m_rangesmall<<
				" angle: "<<m_angle <<", Color"<<m_color<< " ";

        TargetEventPtr event(new TargetEvent(
                                 m_targetCenterX,
                                 m_targetCenterY,
				 m_targetLargeCenterX,
				m_targetLargeCenterY,
				m_targetSmallCenterX,
				m_targetSmallCenterY,
                                 m_squareNess,
                                 m_range,
				m_largeflag,
				m_smallflag,
				m_rangelarge,
				m_rangesmall,
				m_angle,
				  m_color));
				
        //printf("\n Publishing Target");
        publish(EventType::TARGET_FOUND, event);
 */
}

void TargetDetector::categorizeContours(    std::vector<std::vector<cv::Point> > &contours,
                            std::vector<std::vector<cv::Point> > &prunedContours,
                            std::vector<std::vector<cv::Point> > &circleContours,
                            std::vector<std::vector<cv::Point> > &rectangleContours)
{
    double maxRectArea = -1;
    for(unsigned int i = 0; i < contours.size(); i++)
    {
        bool numValid = true;
        bool shapeValid = false;
        //a circle has a circumfrence of 2*pi*r
        //its radius is r, the distance from the center
        //the mean distance to the center of the contours should be r
        //thus we will look at the circumfrence(distance between all contours) over r(mean distance to centroid)
        //this quantity should equal 2*pi, so we will threshold on a percentage of 2*pi
        //we will also threshold on how many points make up the contour
        //we also need a condition for the main square making up the task
        //for this we will need to think a bit more
        
        //# of points check
        //std::cout<<contours[i].size()<<std::endl;
        if(contours[i].size() < (unsigned int)numPointsThreshold)
        {
            numValid = false;
        }

        //shape checks
        if(numValid == true)
        {
            //circularity check
            //find centroid
            int cx = 0;
            int cy = 0;
            for(unsigned int j = 0; j < contours[i].size(); j++)
            {
                cx = cx + contours[i][j].x;
                cy = cy + contours[i][j].y;
            }
            cx = cx/contours[i].size();
            cy = cy/contours[i].size();

            double radius = 0;
            double circumfrence = 0;
            //now find mean radius
            for(unsigned int j = 0; j < contours[i].size(); j++)
            {

                cv::Point p = contours[i][j];
                cv::Point c(cx,cy);
                radius = radius + sqrt((p.x-c.x)*(p.x-c.x) + (p.y-c.y)*(p.y-c.y));   
            }
            radius = radius/contours[i].size();
            //compute circumfrence using min bounding circle
            //also gets difference in ideal center vs averaged center
            float idealRad;
            cv::Point2f center;
            cv::minEnclosingCircle(contours[i], center, idealRad);
            circumfrence = 2*PI*idealRad;

            //compute metric
            double metric = circumfrence/radius;
            //check metric
            if(shapeValid == false && (metric >= (1-circularityBound)*2*PI)&& (metric <= (1+circularityBound)*2*PI))
            {
                //only if in size bounds
                if(idealRad > minR && idealRad< maxR)
                {
                    shapeValid = true;
                    circleContours.push_back(contours[i]);
                }
            }
            //end circularity test

            //begin rectangularity test
            //I seriously might just change this to pick the largest contour that isn't a circle, its pretty much always the rectangle
            if(false == false)
            {
                cv::RotatedRect r = cv::minAreaRect(contours[i]);
                double area = r.size.width*r.size.height;//cv::contourArea(contours[i]);
                double metric = r.size.width/r.size.height;
                if((metric >= (1-rectangularityBound)) && (metric <= (1+rectangularityBound)))
                {
                    //make sure shape isn't the side of the whole frame
                    //numbers are 70% of frame
                    if(r.size.width < 448 && r.size.height < 336)
                    {
                        if((area) > maxRectArea)
                        {
                            maxRectArea = area;
                            shapeValid = true;
                            if(rectangleContours.size() > 0)
                            {
                                rectangleContours.pop_back();
                            }
                            rectangleContours.push_back(contours[i]);
                            //std::cout<<metric<<"  "<<area<<" "<<r.center<<std::endl;
                        }
                    }
                    //std::cout<<metric<<"  "<<area<<" "<<r.center<<std::endl;
                }
                //std::cout<<metric<<"  "<<area<<" "<<r.center<<std::endl;
 
            }

        }
        //add valid contour
        if(numValid && shapeValid)
        {
            prunedContours.push_back(contours[i]);
        }
    }
    //now prune the contours further by eliminating duplicate contours
    //only prune circles, there can only be one square
    //larger object always wins
    
    
}


void TargetDetector::setPublishData(targetPanel square, Image* input)
{
    using namespace cv;

logger.infoStream() << " Setting Data to be Publish";

	Point2f vertices[4];
	int minTargetSize = 25;
	square.outline.points(vertices);
	// Determine the corindates of the target
logger.infoStream() << " Center of main panel ("<<(int)square.outline.center.x<<" , "<<(int)square.outline.center.y <<") ";

	Detector::imageToAICoordinates(input, 
	              (int)square.outline.center.x,
	              (int)square.outline.center.y,
	              m_targetCenterX,
                      m_targetCenterY);

logger.infoStream() << "Center converted to ("<<m_targetCenterX <<" , "<<m_targetCenterY <<") ";

	// Determine range
	//m_range = 1.0 - (((double)square.outline.size.width) /
	//                 ((double)square.outline.size.height));
	
	//m_range = square.outline.size.width;
	//m_range is assigned before now
	// Determine the squareness
	
	double aspectRatio = ((double)square.outline.size.height)/((double)square.outline.size.width);
	if (aspectRatio < 1)
	    m_squareNess = 1.0;
	else
	    m_squareNess = 1.0/aspectRatio;

	if (abs(square.outline.angle) <45) 
		m_angle = abs(square.outline.angle);
	else
		m_angle = 90-abs(square.outline.angle);

	//printf("\n angle: %f",m_angle);
	//plot pretty results
	//for (int i = 0; i < 4; i++)
  	//	line(img_whitebalance, vertices[i], vertices[(i+1)%4], Scalar(0,255,0));
	if (square.foundLarge == true)
	{
	 	if (square.targetLarge.size.width > minTargetSize && square.targetLarge.size.height > minTargetSize)
		{
			m_largeflag = true;
			//valid
			Detector::imageToAICoordinates(input, 
		                 (int)square.targetLarge.center.x,
		                 (int)square.targetLarge.center.y,
		                  m_targetLargeCenterX,
		                  m_targetLargeCenterY);
			m_rangelarge = square.targetLarge.size.width; 

			//plot pretty results
			if (m_color == Color::RED)
			{
				ellipse(img_whitebalance, square.targetLarge, Scalar(0,0,255), 14, 8 );
			}
			else if (m_color == Color::GREEN)
			{
				ellipse(img_whitebalance, square.targetLarge, Scalar(0,255,0), 14, 8 );
			}
			else if (m_color == Color::BLUE)
			{
				ellipse(img_whitebalance, square.targetLarge, Scalar(255,0,0), 14, 8 );
			}
			else if (m_color == Color::YELLOW)
			{
				ellipse(img_whitebalance, square.targetLarge, Scalar(0,255,255), 14, 8 );
			}
			else 
			{
				ellipse(img_whitebalance, square.targetLarge, Scalar(255,255,0), 14, 8 );
			}

		}
	}
	else
	{
		m_largeflag = false;
		m_targetLargeCenterX= 0;
                m_targetLargeCenterY=0;
		m_rangelarge =0;
	}
	if (square.foundSmall == true)
	{
	  	if (square.targetSmall.size.width > minTargetSize && square.targetSmall.size.height > minTargetSize)
		{
			m_smallflag = true;
			//valid
			Detector::imageToAICoordinates(input, 
		                 (int)square.targetSmall.center.x,
		                 (int)square.targetSmall.center.y,
		                 m_targetSmallCenterX,
		                 m_targetSmallCenterY);
		
			m_rangesmall = square.targetSmall.size.width; 
			if (m_color == Color::RED)
			{
				ellipse(img_whitebalance, square.targetSmall, Scalar(0,0,150), 14, 8 );
			}
			else if (m_color == Color::GREEN)
			{
				ellipse(img_whitebalance, square.targetSmall, Scalar(0,150,0), 14, 8 );
			}
			else if (m_color == Color::BLUE)
			{
				ellipse(img_whitebalance, square.targetSmall, Scalar(150,0,0), 14, 8 );
			}
			else if (m_color == Color::YELLOW)
			{
				ellipse(img_whitebalance, square.targetSmall, Scalar(0,150,150), 14, 8 );
			}
			else 
			{
				ellipse(img_whitebalance, square.targetSmall, Scalar(150,150,0), 14, 8 );
			}
		}
	}
	else
	{
		m_smallflag = false;
		m_targetSmallCenterX= 0;
                m_targetSmallCenterY=0;
		m_rangesmall =0;
	}
}   

} //namespace vision2014    
} // namespace vision
} // namespace ram
