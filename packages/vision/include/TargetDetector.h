/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/TargetDetector.h
 */

#ifndef RAM_TARGET_DETECTOR_H_04_29_2009
#define RAM_TARGET_DETECTOR_H_04_29_2009

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/WhiteBalance.h"
#include "vision/include/BuoyDetectorKate.h"

#include "core/include/ConfigNode.h"

// Must be incldued last
#include "vision/include/Export.h"

namespace ram {
namespace vision {
    
class RAM_EXPORT TargetDetector : public Detector
{
  public:
    TargetDetector(core::ConfigNode config,
                   core::EventHubPtr eventHub = core::EventHubPtr());
    ~TargetDetector();

    void processImage(Image* input, Image* output= 0);
    void processColorImage(Image* input, Image* output=0);

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
	 RotatedRect outline;
	 RotatedRect targetSmall;
	 RotatedRect targetLarge;
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
    void filterForGreen(Image* input);

    /** Find target blob candidates: use the null blobs that are inside the
     * Green blobs and combine to fill most of the space the green blob as well
     * as have centroids pretty close together.
     * Second method is to use the horizontal and vertical runner techniques
     * to attempt to find the bounds of the internal space
     */
    
    /** Processes the list of all found blobs and finds all valid targets
     *
     *  This current only checks for aspect ratio constraints.
     */
    bool processGreenBlobs(const BlobDetector::BlobList& blobs,
                           BlobDetector::BlobList& outBlobs);

    /** Publish the found event with the latest target information */
    void publishFoundEvent();
    
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

	Mat img_whitebalance;
	targetPanel getSquareBlob(Mat img, Mat img_whitebalance,Color::ColorType color); //color type is purefly for display purposes
	void setPublishData(targetPanel square, Image* input);

	int m_minSize;
	int m_framenumber;
	 int m_minContourSize;
};
	
} // namespace vision
} // namespace ram

#endif // RAM_TARGET_DETECTOR_H_04_29_2009
