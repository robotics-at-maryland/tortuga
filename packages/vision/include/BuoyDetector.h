/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/BuoyDetector.h
 */

#ifndef RAM_BUOY_DETECTOR_H_05_23_2010
#define RAM_BUOY_DETECTOR_H_05_23_2010

// STD Includes
#include <set>
#include <vector>
#include <string>
#include <cmath>


// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Color.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/TableColorFilter.h"
#include "core/include/ConfigNode.h"
#include "vision/include/BuoyDetectorKate.h"

// Must be included last
#include "vision/include/Export.h"

using namespace std;
using namespace cv;

namespace ram {
namespace vision {

class RAM_EXPORT BuoyDetector : public Detector
{

		struct foundblob
		{
			double centerx;
			double centery;
			double range;
			int minX, minY, maxX,maxY;
			int color;
			int position;
			CvPoint vertex1;
			CvPoint vertex2;
			CvPoint vertex3;
			CvPoint vertex4;
			
		};

  public:
    BuoyDetector(core::ConfigNode config,
                 core::EventHubPtr eventHub = core::EventHubPtr());
    BuoyDetector(Camera* camera);
    ~BuoyDetector();

    void update();
    void processImage(Image* input, Image* output = 0);
    void processBuoys(Image* input, Image* output = 0);
    void processBuoysImage(Image* input, Image* output = 0);
    void processBuoysMask(cv::Mat* mask, Image* img, Image* output);
    void initProcessBuoys(cv::Mat temp1, cv::Mat temp2);
      IplImage* getAnalyzedImage();

    // Setter and Getter for lookup table color filter
    bool getRedLookupTable();
    void setRedLookupTable(bool lookupTable); 

    bool getYellowLookupTable();
    void setYellowLookupTable(bool lookupTable); 
    
    bool getGreenLookupTable();
    void setGreenLookupTable(bool lookupTable); 
	
    void DetectorContours(Image* input);
    foundblob getSquareBlob(Mat erosion_dst);
    void processImageSimpleBlob(Image* input, Image* output);
  private:

	Mat erode_dst_red;
	Mat erode_dst_green;
	Mat erode_dst_yellow;
	Mat erode_dst;

    void init(core::ConfigNode config);

    /* Normal processing to find one blob/color */
    bool processColor(Image* input, Image* output, ImageFilter& filter,
                      bool useLookupTable, BlobDetector::Blob& outBlob);
    
    void drawBuoyDebug(Image* debugImage, BlobDetector::Blob &blob,
                       unsigned char red, unsigned char green,
                       unsigned char blue);

    // Process current state, and publishes LIGHT_FOUND event
    void publishFoundEvent(BlobDetector::Blob& blob, Color::ColorType color);
    void publishFoundEventKate(cv::KeyPoint blob, Color::ColorType color);
    void publishLostEvent(Color::ColorType color);
	bool foundyellowbefore;
	bool foundgreenbefore;
	bool foundredbefore;

    Camera *cam;

    /** State variables */
    bool m_redFound;
    bool m_greenFound;
    bool m_yellowFound;

    /** Color Filters */
    ColorFilter *m_redFilter;
    ColorFilter *m_greenFilter;
    ColorFilter *m_yellowFilter;
    ColorFilter *m_blackFilter;

    /** Color Filter Lookup Table */
    TableColorFilter *m_redTableColorFilter;
    TableColorFilter *m_yellowTableColorFilter;
    TableColorFilter *m_greenTableColorFilter;
    
    /** Blob Detector */
    BlobDetector m_blobDetector;
    
    /** Threshold for almost hitting the red light */
    double m_almostHitPercentage;

    /** Whether or not to check for black below the buoy */
    bool m_checkBlack;

    /** Number of dilate iterations */
    int m_dilateIterations;

    /** Number of erode iterations */
    int m_erodeIterations;

    /** The precentage of the subwindow that must be black */
    double m_minBlackPercentage;

    /** The biggest fraction of the window for the black check */
    double m_maxTotalBlackCheckSize;

    /** Percentage of the image to remove from the top */
    double m_topIgnorePercentage;

    /** Percentage of the image to remove from the bottom */
    double m_bottomIgnorePercentage;

    /** Percentage of the image to remove from the left */
    double m_leftIgnorePercentage;

    /** Percentage of the image to remove from the right */
    double m_rightIgnorePercentage;

    /** Working Images */
    Image *frame;
    Image *redFrame;
    Image *greenFrame;
    Image *yellowFrame;
    Image *blackFrame;

    /**Working images for detection algorithm*/
    cv::Mat tempImage;
    cv::Mat combImage;
    /*Channels*/
    std::vector<cv::Mat> channels;
    /*temporary image for colorspaces*/
    cv::Mat cspMat;
    /*temporary images for templates*/
    cv::Mat cannyMat;
    cv::Mat firstTemp;
    cv::Mat firstTempCast;
    cv::Mat secondTempCast;
    cv::Mat secondTemp;
    
   /*Kate edit - same whitebalance image and final output*/
	cv::Mat img_whitebalance;
	cv::Mat img_buoy;

    /* Configuration variables */
    double m_maxAspectRatio;
    double m_minAspectRatio;

    int m_minWidth;
    int m_minHeight;

    double m_minPixelPercentage;
    int m_minPixels;
    double m_maxDistance;

    double m_physicalWidthMeters;

    int m_debug;

    bool m_useRedFilterLookup;
    bool m_useYellowFilterLookup;
    bool m_useGreenFilterLookup;

    std::string m_redLookupTablePath;
    std::string m_yellowLookupTablePath;
    std::string m_greenLookupTablePath;

    /*first and second templates for the template match*/
    cv::Mat m_template1;
    cv::Mat m_template2;
    //cutoff of the image
    cv::Mat cutoffBuoys;
    //final image
    cv::Mat finalBuoys;
    IplImage finalBuoys_iplimage;
    Image* tempoutput;
    //vision::OpenCVImage finalBuoys_image(&finalBuoys_iplimage);

    /*values for canny algorithm*/
    /*red, green, blue, value and 1rst component
      of the YCrCb colorspace*/
    double m_rMin;
    double m_rMax;
    double m_gMin;
    double m_gMax;
    double m_bMax;
    double m_bMin;
    double m_vMin;
    double m_vMax;
    double m_YCMax;
    double m_YCMin;
    /*cutoffs for the cutoff algorithm*/
    double m_cutoffZero;
    double m_cutoffLength;
    /*scaling factor for overlall image standard deviation*/
    double stDevFactor;
    
    //added for Kate's buoy detector
	//want blobfinder class to be added
	blobfinder blob;
  foundblob m_yellowbuoy,m_greenbuoy,m_redbuoy;
  void publishFoundEventContour(foundblob buoy, Color::ColorType color);

	int m_framenumber; //frame number since start of class
};
	
} // namespace vision
} // namespace ram

#endif // RAM_BUOY_DETECTOR_H_05_23_2010
