/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/TargetDetector.h
 */

#ifndef RAM_TARGET_DETECTOR_H_04_29_2014
#define RAM_TARGET_DETECTOR_H_04_29_2014

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/Color.h"

#include "core/include/ConfigNode.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
namespace vision2014 {
    
class RAM_EXPORT TargetDetector : public Detector
{
  public:
    TargetDetector(core::ConfigNode config,
                   core::EventHubPtr eventHub = core::EventHubPtr());
    ~TargetDetector();

    void processImage(Image* input, Image* output= 0);

    bool found();

    /** Center of the red light in the local horizontal, -4/3 to 4/3 */
    double getX();
    
    /** Center of the red light in the local vertical, -1 to 1 */
    double getY();

    /** The percent of the top of the image blacked out */
    void setTopRemovePercentage(double percent);

    /** The percent of the bottom of the image blacked out */
    void setBottomRemovePercentage(double percent);

  struct targetPanel
	{
        cv::RotatedRect outline;
        cv::RotatedRect targetSmall;
        cv::RotatedRect targetLarge;
        bool foundLarge;
        bool foundSmall;
        bool foundOutline;
	};
        
  private:
    void init(core::ConfigNode config);

    /** Filters the image so that all green is white
     *
     *  Note the applies the top and bottom remove steps as well as the desired
     *  erosion and dilation steps.  It produces an image ready to be processed
     *  by the other portions of the algorithm.
     */


    /** Publish the found event with the latest target information */
    void publishFoundEvent();
    
    //performs categorization on the contours
    void categorizeContours(    std::vector<std::vector<cv::Point> > &contours,
                                                std::vector<std::vector<cv::Point> > &prunedContours,
                                                std::vector<std::vector<cv::Point> > &circleContours,
                                                std::vector<std::vector<cv::Point> > &rectangleContours);

    //define fixed parameters
    int bSize;//block size for adaptive threshold
    int filterSize;//filter size for filtering(kernal, is d for bilateral)
    double colorSigma;
    double spaceSigma;
    int cH;
    int cS;
    int cV;
    int threshType;
    int cannyT1;
    int cannyT2;
    int morphIterations;//iterations for closing operation
    int morph_elem;//0 rect, 1 circle, 2 ellipse
    int morph_size;
    //this bound is a bidirectional threshold
    double circularityBound;
    double rectangularityBound;
    int numPointsThreshold;
    int dilateCount;
    int erodeCount;
    int debugOut;
    cv::RNG rng;

    //outdated params
 /** Filters for the green in the water */
    ColorFilter* m_filter;
   ColorFilter *m_blueFilter; //for finding the blue target
   ColorFilter *m_redFilter;
   ColorFilter *m_yellowFilter;
   ColorFilter *m_greenFilter;


/**Filter for Red value for VisionToolV2*/
	int m_redminH;
	int m_redmaxH;
    /** Working scratch image */
    Image* m_image;

    /** Wether or not the target is found */
    bool m_found;
    
    /** The location for the X center of the target */
    double m_targetCenterX;

    /** The location for the Y center of the target */
    double m_targetCenterY;

    /** 1 when the area inside the target is perfectly square, < 1 otherwise */
    double m_squareNess;

     /** location for the X center of the large hole target*/
     double m_targetLargeCenterX;
     /** location for the Y center of the large hole target*/
     double m_targetLargeCenterY;
     /** location for the X center of the small hole target*/
     double m_targetSmallCenterX;
     /** location for the Y center of the small hole target*/
     double m_targetSmallCenterY;
	/**color of target*/    
	Color::ColorType m_color;
    /** 0 When the target fills the screen, goes to 1 as it shrinks */
    double m_range;
    double m_rangelarge;
    double m_rangesmall;

    bool m_largeflag;
    bool m_smallflag;

    /** The minimum pixel count of the green target blob */
    int m_minGreenPixels;

    /** How many times to erode the green filtered elements */
    int m_erodeIterations;

    /** How many times to dilate the green filtered elements */
    int m_dilateIterations;

    /** Finds the red light */
    BlobDetector m_blobDetector;

    /** How "skinny" a blob can be and still be considered a target */
    double m_maxAspectRatio;

    /** How "fat" a blob can be and still be considered a target */
    double m_minAspectRatio;
    
    /** Percentage of the image to remove from the top */
    double m_topRemovePercentage;

    /** Percentage of the image to remove from the bottom */
    double m_bottomRemovePercentage;
	bool m_greenFound;
	bool m_redFound;
	bool m_blueFound;
	bool m_yellowFound;

	double m_angle; //angle of RotatedRect

    cv::Mat img_whitebalance;
	targetPanel getSquareBlob(cv::Mat img, cv::Mat img_whitebalance,Color::ColorType color); //color type is purefly for display purposes
	void setPublishData(targetPanel square, Image* input);

	int m_minSize;
	int m_framenumber;
	 int m_minContourSize;
};

}//namespace vision2014	
} // namespace vision
} // namespace ram

#endif // RAM_TARGET_DETECTOR_H_04_29_2009
