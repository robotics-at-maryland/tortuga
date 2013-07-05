/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/src/BuoyDetector.cpp
 */

// STD Includes
#include <math.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <unistd.h>


// Library Includes
#include "cv.h"
#include "highgui.h"
#include <log4cpp/Category.hh>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/math/distributions/students_t.hpp>

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"

#include "vision/include/BuoyDetector.h"
#include "vision/include/Camera.h"
#include "vision/include/Color.h"
#include "vision/include/Events.h"
#include "vision/include/Image.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/ImageFilter.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/RegionOfInterest.h"
#include "vision/include/Utility.h"
#include "vision/include/VisionSystem.h"
#include "vision/include/TableColorFilter.h"
#include "vision/include/WhiteBalance.h"
#include "vision/include/BuoyDetectorKate.h"

static boost::filesystem::path getImagesDir()
{
    boost::filesystem::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "vision" / "test" / "data" / "references";
}

static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("BuoyDetector"));


using namespace std;
using namespace cv;

namespace ram {
namespace vision {

BuoyDetector::BuoyDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_redFound(false),
    m_greenFound(false),
    m_yellowFound(false),
    m_redFilter(0),
    m_greenFilter(0),
    m_yellowFilter(0),
    m_blobDetector(config, eventHub),
    m_checkBlack(false),

    m_dilateIterations(0),
    m_erodeIterations(0),
    m_minBlackPercentage(0),
    m_maxTotalBlackCheckSize(0),
    m_physicalWidthMeters(0.26),
    m_useRedFilterLookup(false),
    m_useYellowFilterLookup(false),
    m_useGreenFilterLookup(false),
    m_redLookupTablePath(""),
    m_yellowLookupTablePath(""),
    m_greenLookupTablePath("")

{
    init(config);
}

BuoyDetector::BuoyDetector(Camera* camera) :
    cam(camera),
    m_redFound(false),
    m_greenFound(false),
    m_yellowFound(false),
    m_redFilter(0),
    m_greenFilter(0),
    m_yellowFilter(0),
    m_dilateIterations(0),
    m_erodeIterations(0),
    m_minBlackPercentage(0),
    m_maxTotalBlackCheckSize(0),
    m_physicalWidthMeters(0.26),
    m_useRedFilterLookup(false),
    m_useYellowFilterLookup(false),
    m_useGreenFilterLookup(false),
    m_redLookupTablePath(""),
    m_yellowLookupTablePath(""),
    m_greenLookupTablePath("")
{
    init(core::ConfigNode::fromString("{}"));
}

void BuoyDetector::init(core::ConfigNode config)
{
    LOGGER.info("Initializing Buoy Detector");
    // Detection variables
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    propSet->addProperty(config, false, "debug",
                         "Debug level", 2, &m_debug, 0, 2);

    propSet->addProperty(config, false, "maxAspectRatio",
                         "Maximum aspect ratio (width/height)",
                         1.1, &m_maxAspectRatio);

    propSet->addProperty(config, false, "minAspectRatio",
                         "Minimum aspect ratio (width/height)",
                         0.25, &m_minAspectRatio);

    propSet->addProperty(config, false, "minWidth",
                         "Minimum width for a blob",
                         50, &m_minWidth);

    propSet->addProperty(config, false, "minHeight",
                         "Minimum height for a blob",
                         50, &m_minHeight);

    propSet->addProperty(config, false, "minPixelPercentage",
                         "Minimum percentage of pixels / area",
                         0.1, &m_minPixelPercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "minPixels",
                         "Minimum number of pixels", 15, &m_minPixels);

    propSet->addProperty(config, false, "maxDistance",
                         "Maximum distance between two blobs from different frames",
                         15.0, &m_maxDistance);

    propSet->addProperty(config, false, "almostHitPercentage",
                         "Radius when the buoy is considered almost hit",
                         0.2, &m_almostHitPercentage, 0.0, 1.0);


    propSet->addProperty(config, false, "checkBlack",
                         "Whether or not to look for black beneath the buoy",
                         false, &m_checkBlack);

    propSet->addProperty(config, false, "dilateIterations",
                         "Number of dilate iterations to perform",
                         0, &m_dilateIterations);

    propSet->addProperty(config, false, "erodeIterations",
                         "Number of erode iterations to perform",
                         0, &m_erodeIterations);

    propSet->addProperty(config, false, "minBlackPercentage",
                         "The precentage of the subwindow that must be black",
                         0.25, &m_minBlackPercentage, 0.0, 1.0);

    propSet->addProperty(config, false, "maxTotalBlackCheckSize",
                         "The biggest fraction of the window for the black check",
                         0.1, &m_maxTotalBlackCheckSize, 0.0, 1.0);


    propSet->addProperty(config, false, "topIgnorePercentage",
        "% of the screen from the top to be blacked out",
        0.0, &m_topIgnorePercentage);

    propSet->addProperty(config, false, "bottomIgnorePercentage",
        "% of the screen from the bottom to be blacked out",
        0.0, &m_bottomIgnorePercentage);

    propSet->addProperty(config, false, "leftIgnorePercentage",
        "% of the screen from the left to be blacked out",
        0.0, &m_leftIgnorePercentage);

    propSet->addProperty(config, false, "rightIgnorePercentage",
        "% of the screen from the right to be blacked out",
        0.0, &m_rightIgnorePercentage);
    
    propSet->addProperty(config, false, "RedColorFilterLookupTable",
        "True uses red color filter lookup table", false, 
        boost::bind(&BuoyDetector::getRedLookupTable, this),
        boost::bind(&BuoyDetector::setRedLookupTable, this, _1));

    propSet->addProperty(config, false, "YellowColorFilterLookupTable",
        "True uses yellow color filter lookup table", false, 
        boost::bind(&BuoyDetector::getYellowLookupTable, this),
        boost::bind(&BuoyDetector::setYellowLookupTable, this, _1));
    
    propSet->addProperty(config, false, "GreenColorFilterLookupTable",
        "True uses green color filter lookup table", false, 
        boost::bind(&BuoyDetector::getGreenLookupTable, this),
        boost::bind(&BuoyDetector::setGreenLookupTable, this, _1));
    
    m_redFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_redFilter->addPropertiesToSet(propSet, &config,
                                    "RedL", "Red Luminance",
                                    "RedC", "Red Chrominance",
                                    "RedH", "Red Hue",
                                    0, 255, 0, 255, 0, 255);
    m_greenFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_greenFilter->addPropertiesToSet(propSet, &config,
                                    "GreenL", "Green Luminance",
                                    "GreenC", "Green Chrominance",
                                    "GreenH", "Green Hue",
                                    0, 255, 0, 255, 0, 255);
    m_yellowFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_yellowFilter->addPropertiesToSet(propSet, &config,
                                    "YellowL", "Yellow Luminance",
                                    "YellowC", "Yellow Chrominance",
                                    "YellowH", "Yellow Hue",
                                    0, 255, 0, 255, 0, 255);

    m_blackFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_blackFilter->addPropertiesToSet(propSet, &config,
                                    "BlackL", "Black Luminance",
                                    "BlackC", "Black Chrominance",
                                    "BlackH", "Black Hue",
                                    0, 255, 0, 255, 0, 255);
        
    
    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);
        
    // Working images
    frame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    
    redFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    greenFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    yellowFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    blackFrame = new OpenCVImage(640, 480, Image::PF_BGR_8);
    
    //buoy detection algorithm constants
    //starts from the build folder?
    //  initProcessBuoys(cv::imread(
    //          "/home/eliot/Python Tests/finalTemplates/ArtificialTemplateA3.png",0),
    //      cv::imread(
    //          "/home/eliot/Python Tests/finalTemplates/ArtificialTemplateB4.png",0));
    initProcessBuoys(cv::imread(
                         (getImagesDir() / "ArtificialTemplateA3.png").string(),0),
            cv::imread(
                (getImagesDir() / "ArtificialTemplateB4.png").string(),0));


    LOGGER.info("foundRed redX redY redRange redWidth "
                "redHeight redNumPixels redPixelPct" 
                "foundGreen greenX greenY greenRange greenWidth "
                "greenHeight greenNumPixels greenPixelPct"
                "foundYellow yellowX yellowY yellowRange yellowWidth "
                "yellowHeight yellowNumPixel yellowPixelPct");
 
    
        
}

BuoyDetector::~BuoyDetector()
{
    delete m_redFilter;
    delete m_greenFilter;
    delete m_yellowFilter;
    
    if ( m_useRedFilterLookup )
        delete m_redTableColorFilter;
    if ( m_useYellowFilterLookup )
        delete m_yellowTableColorFilter;
    if ( m_useGreenFilterLookup )
        delete m_greenTableColorFilter;

    delete frame;
    delete redFrame;
    delete greenFrame;
    delete yellowFrame;
    delete blackFrame;


}

void BuoyDetector::update()
{
    cam->getImage(frame);
    processImage(frame);
}

bool BuoyDetector::getRedLookupTable()
{
    return m_useRedFilterLookup;
}

void BuoyDetector::setRedLookupTable(bool lookupTable)
{
    if ( lookupTable) {
        m_useRedFilterLookup = true;

        // Initializing ColorFilterTable
        m_redLookupTablePath = 
            "/home/steven/TransdecImages/LookupTables/redBuoyBlend3.serial";
        m_redTableColorFilter = new TableColorFilter(m_redLookupTablePath);
    } else {
        m_useRedFilterLookup = false;
    }
}


bool BuoyDetector::getYellowLookupTable()
{
    return m_useYellowFilterLookup;
}

void BuoyDetector::setYellowLookupTable(bool lookupTable)
{
    if ( lookupTable) {
        m_useYellowFilterLookup = true;

        // Initializing ColorFilterTable
        m_yellowLookupTablePath = 
            "/home/steven/TransdecImages/LookupTables/yellowBuoyBlend3.serial";
        m_yellowTableColorFilter = new TableColorFilter(m_yellowLookupTablePath);
    } else {
        m_useYellowFilterLookup = false;
    }
}

bool BuoyDetector::getGreenLookupTable()
{
    return m_useGreenFilterLookup;
}

void BuoyDetector::setGreenLookupTable(bool lookupTable)
{
    if ( lookupTable) {
        m_useGreenFilterLookup = true;

        // Initializing ColorFilterTable
        m_greenLookupTablePath = 
            "/home/steven/TransdecImages/LookupTables/greenBuoyBlend3.serial";
        m_greenTableColorFilter = new TableColorFilter(m_greenLookupTablePath);
    } else {
        m_useGreenFilterLookup = false;
    }
}

bool BuoyDetector::processColor(Image* input, Image* output,
                                ImageFilter& filter, bool useLookupTable, 
                                BlobDetector::Blob& outBlob)
{
    const int imgWidth = input->getWidth();
    const int imgHeight = input->getHeight();
    const int imgPixels = imgWidth * imgHeight;

    output->copyFrom(input);
    output->setPixelFormat(Image::PF_RGB_8);
    output->setPixelFormat(Image::PF_LCHUV_8);

    if ( useLookupTable ) 
        filter.filterImage(input, output);
    else 
        filter.filterImage(output);
 
    // Dilate the image (only if necessary)
    if (m_dilateIterations > 0) 
    {
        cvDilate(output->asIplImage(), output->asIplImage(),
                 NULL, m_dilateIterations);
    }

    // Erode the image (only if necessary)
    if (m_erodeIterations > 0) 
    {
        cvErode(output->asIplImage(), output->asIplImage(),
                NULL, m_erodeIterations);
    }

    m_blobDetector.processImage(output);
    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();

    bool foundBlob = false;
    
    BOOST_FOREACH(BlobDetector::Blob blob, blobs)
    {
        // Sanity check blob
        double percent = (double) blob.getSize() /
            (blob.getWidth() * blob.getHeight());
        if (blob.getTrueAspectRatio() < m_maxAspectRatio &&
            blob.getHeight() > m_minHeight &&
            blob.getWidth() > m_minWidth &&
            blob.getSize() > m_minPixels &&
            percent > m_minPixelPercentage &&
            blob.getTrueAspectRatio() > m_minAspectRatio)
        {
            foundBlob = true;

            // Check for black below blob (if desired)
            if (m_checkBlack)
            {
                int height = blob.getHeight();
                double totalSize = blob.getHeight() * blob.getWidth();
        
                // Only check this when the buoy should be far away
                if ((totalSize / imgPixels) < m_maxTotalBlackCheckSize )
                {
                    double blackCount = Utility::countWhitePixels(blackFrame,
                                                                  blob.getMinX(), 
                                                                  blob.getMinY() + height,
                                                                  blob.getMaxX(), 
                                                                  blob.getMaxY() + height);
            
                    double blackPercentage = blackCount / totalSize;
                    if (blackPercentage < m_minBlackPercentage)
                        foundBlob = false;
                }
            }

            if(foundBlob)
            {
                outBlob = blob;
                break;
            }
        }
    }

 

    return foundBlob;
}


void BuoyDetector::processImage(Image* input, Image* output)
{

	//KATE Update 2013
	//convert from Image* to Mat
	Mat img = input->asIplImage();
	//imshow("input image", img);

        Mat output_blob= img;
	//IplImage finalBouys_iplimage;

	img_whitebalance = WhiteBalance(img);
	imshow("whitebalance",img_whitebalance);
	img_buoy = blob.DetectBuoys(img_whitebalance, m_redFilter, m_greenFilter,m_yellowFilter);
	
	cvtColor(img_buoy,img_buoy,CV_BGR2RGB);
	//copy data from Mat to an IMage*
	//note, that the src (in this case img_buoy) must be a member of the class and not just here
        input->setData(img_buoy.data,false);

	vector<KeyPoint>  Yellow_keypoints= blob.getYellow();
  	vector<KeyPoint>  Red_keypoints= blob.getRed();
  	vector<KeyPoint>  Green_keypoints= blob.getGreen();

	//Kate code
	//publish event for found buoys
	for (unsigned int i=0;i<Yellow_keypoints.size();i++)
	{	
		publishFoundEventKate(Yellow_keypoints[i], Color::YELLOW);
	}
	if (Yellow_keypoints.size() < 1)
	{
		publishLostEvent(Color::YELLOW);

       		LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                            << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
	}
	for (unsigned int i=0;i<Red_keypoints.size();i++)
	{	
		publishFoundEventKate(Red_keypoints[i], Color::RED);
	}
	if (Red_keypoints.size() < 1)
	{
		publishLostEvent(Color::RED);

       		LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                            << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
	}
	for (unsigned int i=0;i<Green_keypoints.size();i++)
	{	
		publishFoundEventKate(Green_keypoints[i], Color::GREEN);
	}
	if (Green_keypoints.size() < 1)
	{
		publishLostEvent(Color::GREEN);

       		LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                            << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
	}
        //In order to pass the vision test, we need an output
	//so thats why the rest of this code is still here
       frame->copyFrom(input);


/*
    int topRowsToIgnore = m_topIgnorePercentage * frame->getHeight();
    int bottomRowsToIgnore = m_bottomIgnorePercentage * frame->getHeight();
    int leftColsToIgnore = m_leftIgnorePercentage * frame->getWidth();
    int rightColsToIgnore = m_rightIgnorePercentage * frame->getWidth();

    int initialMinX = leftColsToIgnore;
    int initialMaxX = frame->getWidth() - rightColsToIgnore;
    int initialMinY = topRowsToIgnore;
    int initialMaxY = frame->getHeight() - bottomRowsToIgnore;

    RegionOfInterest initialROI = RegionOfInterest(initialMinX, initialMaxX,
                                                   initialMinY, initialMaxY);

	    int framePixels = initialROI.area();
	   int almostHitPixels = framePixels * m_almostHitPercentage; //kate comment
*/

/*
    // Filter for black if needed
    if (m_checkBlack)
    {
        blackFrame->copyFrom(frame);
        blackFrame->setPixelFormat(Image::PF_RGB_8);
        blackFrame->setPixelFormat(Image::PF_LCHUV_8);

        m_blackFilter->filterImage(blackFrame);
    }
*/

/*
    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();

    BlobDetector::Blob redBlob;

    bool redFound=1; //hardcoded by kate

    if ( m_useRedFilterLookup ) {
        //redFound = processColor(frame, redFrame, *m_redTableColorFilter, true, redBlob);
    } else {
        //redFound = processColor(frame, redFrame, *m_redFilter, false, redBlob);
    }
 
    if (redFound)
    {
        publishFoundEvent(redBlob, Color::RED);
        int blobPixels = redBlob.getSize();

        if(blobPixels > almostHitPixels)
            publish(EventType::BUOY_ALMOST_HIT,
                    core::EventPtr(new core::Event()));

        double fracWidth = static_cast<double>(redBlob.getWidth()) / xPixelWidth;
        double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));
        int width = redBlob.getWidth();
        int height = redBlob.getHeight();

        LOGGER.infoStream() << "1" << " "
                            << redBlob.getCenterX() << " "
                            << redBlob.getCenterY() << " "
                            << range << " "
                            << width << " "
                            << height << " "
                            << blobPixels << " "
                            << blobPixels / (width * height);
    } 
    else 
    {
        // Publish lost event if this was found previously
        if (m_redFound) 
            publishLostEvent(Color::RED);

        LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                            << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
        
    }
    m_redFound = redFound;
*/

/*
    BlobDetector::Blob greenBlob;
    bool greenFound;
    if ( m_useGreenFilterLookup ) {
        greenFound = processColor(frame, greenFrame, *m_greenTableColorFilter, 
                                  true, greenBlob);
    } else {
        greenFound = processColor(frame, greenFrame, *m_greenFilter, 
                                  false, greenBlob);
    }
    if (greenFound)
    {
        publishFoundEvent(greenBlob, Color::GREEN);
        int blobPixels = greenBlob.getSize();

        if(blobPixels > almostHitPixels)
            publish(EventType::BUOY_ALMOST_HIT,
                    core::EventPtr(new core::Event()));

        double fracWidth = static_cast<double>(greenBlob.getWidth()) / xPixelWidth;
        double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));
        int width = greenBlob.getWidth();
        int height = greenBlob.getHeight();
        
        LOGGER.infoStream() << "1" << " "
                            << greenBlob.getCenterX() << " "
                            << greenBlob.getCenterY() << " "
                            << range << " "
                            << width << " "
                            << height << " "
                            << blobPixels << " "
                            << blobPixels / (width * height);

    }
    else
    {
        // Publish lost event if this was found previously
        if (m_greenFound)
            publishLostEvent(Color::GREEN);

        LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                            << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
        
    }
    m_greenFound = greenFound;

    BlobDetector::Blob yellowBlob;
    bool yellowFound;
    if ( m_useYellowFilterLookup ) {
        yellowFound = processColor(frame, yellowFrame, *m_yellowTableColorFilter, 
                                   true, yellowBlob);
    } else {
        yellowFound = processColor(frame, yellowFrame, *m_yellowFilter, 
                                   false, yellowBlob);
    }
    if (yellowFound)
    {
        publishFoundEvent(yellowBlob, Color::YELLOW);
        int blobPixels = yellowBlob.getSize();

        if(blobPixels > almostHitPixels)
            publish(EventType::BUOY_ALMOST_HIT,
                    core::EventPtr(new core::Event()));

        double fracWidth = static_cast<double>(yellowBlob.getWidth()) / xPixelWidth;
        double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));
        int width = greenBlob.getWidth();
        int height = greenBlob.getHeight();

        LOGGER.infoStream() << "1" << " "
                            << greenBlob.getCenterX() << " "
                            << greenBlob.getCenterY() << " "
                            << range << " "
                            << width << " "
                            << height << " "
                            << blobPixels << " "
                            << blobPixels / (width * height);

    } 
    else
    {
        // Publish lost event if this was found previously
        if (m_yellowFound)
            publishLostEvent(Color::YELLOW);

        LOGGER.infoStream() << "0" << " " << "0" << " " << "0" << " " << "0" << " "
                            << "0" << " " << "0" << " " << "0" << " " << "0" << " ";
        
    }
    m_yellowFound = yellowFound;
*/
    if(output)
    {
        output->copyFrom(frame);
        if (m_debug >= 1) {
            output->copyFrom(frame);


            unsigned char *data = output->getData();
            int width = output->getWidth();
            int height = output->getHeight();
            int nch = output->getNumChannels();

            int topRowsToIgnore = height * m_topIgnorePercentage;
        //    int bottomRowsToIgnore = height * m_bottomIgnorePercentage;

            int valuesPerRow = width * output->getNumChannels();
            for(int r = 0; r < topRowsToIgnore; r++)
            {
                for(int c = 0; c < valuesPerRow; c++)
                {
                    *(data + r * width * nch + c) = 0;
                }
            }

       //     for(int r = height - bottomRowsToIgnore - 1; r < height - 1; r++)
       //     {
       //         for(int c = 0; c < valuesPerRow; c++)    output->setData(imgdone.data,false);
       //        {
       //             *(data + r * width * nch + c) = 0;
       //         }
       //     }


            //Image::fillMask(output, redFrame, 255, 0, 0);
            //Image::fillMask(output, greenFrame, 0, 255, 0);
            //Image::fillMask(output, yellowFrame, 255, 255, 0);
            //Image::fillMask(output, blackFrame, 147, 20, 255);

        } //endif mdebug==1

    //    if (m_debug == 2) {
    //       if (redFound)
    //            drawBuoyDebug(output, redBlob, 255, 0, 0);
    //        if (greenFound)
    //            drawBuoyDebug(output, greenBlob, 0, 255, 0);
    //        if (yellowFound)
    //            drawBuoyDebug(output, yellowBlob, 255, 255, 0);
    //    }

    } //end if output


}


void BuoyDetector::initProcessBuoys(cv::Mat temp1, cv::Mat temp2)
{
    //this is temporary until these are added to the config values
    //haven't done this currently because algorithm needs testing at
    //this step
    //cv::imshow("debug",temp1);
    temp1.copyTo(m_template1);
    temp2.copyTo(m_template2);
    m_rMin = 20;
    m_rMax = 100;
    m_gMin = 20;
    m_gMax = 100;
    m_bMin = 20;
    m_bMax = 130;
    m_vMin = 20;
    m_vMax = 75;
    m_YCMin = 20;
    m_YCMax = 75;
    //tempImage.create(640,480,CV_8UC(3));
    //combImage.create(640,480,CV_8UC(1));
    //cspMat.create(640,480,CV_8UC(3));
    //cannyMat =  cv::Mat::zeros(640,480,CV_8UC(1));
    //firstTemp.create(640 - m_template1.rows+1, 480 - m_template1.cols+1,CV_32FC(1));
    //secondTemp.create(firstTemp.rows - m_template2.rows+1, firstTemp.cols - 
    //                m_template2.cols+1, CV_32FC(1));
    //cutoffBuoys.create(firstTemp.rows - m_template2.rows+1, firstTemp.cols - 
    //                m_template2.cols+1, CV_8UC(1));
    m_cutoffLength = 5;
    m_cutoffZero = 10;
    //channels = std::vector<cv::Mat>(3);
    //for(int i = 0; i<4; i++)
    //{
    //  channels.push_back(cv::Mat::zeros(640,480, CV_8UC(1)));
    //}
    stDevFactor = .4;
    
    
}

void BuoyDetector::processBuoys(Image* input, Image* output)
{
    std::cout<<"starting"<<std::endl;
    Mat img = input->asIplImage();

    Mat imgdone = WhiteBalance(img); 
    //IplImage imgtemp = IplImage(imgdone);
    //output->setData(imgdone.data,false);



/* COMMENT oUT cause kate says so

    // RGB channels order is BGR //
    cv::split(tempImage, channels);
    cv::Canny(channels[0], combImage, m_bMin, m_bMax);
    combImage.copyTo(cannyMat);
    cv::Canny(channels[1], combImage, m_gMin, m_gMax);
    cannyMat = combImage | cannyMat;
    cv::Canny(channels[2], combImage, m_rMin, m_rMax);
    cannyMat = combImage | cannyMat;

    //next obtain the value component of HSV
    cv::cvtColor(tempImage, cspMat, CV_BGR2HSV);
    cv::split(cspMat, channels);


    cv::Canny(channels[2], combImage, m_vMin, m_vMax);
    cannyMat = combImage | cannyMat;

    //finally obtain the 1rst component of YCrCb
    cv::cvtColor(tempImage, cspMat, CV_BGR2YCrCb);
    cv::split(cspMat, channels);

    cv::Canny(channels[0], combImage, m_YCMin, m_YCMax);
    cannyMat = combImage | cannyMat;

    //next up is the first template match
    m_template1.convertTo(firstTempCast, CV_8UC(1), 1.0, 0);
    cv::matchTemplate(cannyMat, m_template1, firstTemp, CV_TM_CCORR);
    //type of m_template1 has to be changed because the templates
    //  have to be 8 bit images, which forces a change in type

    m_template2.convertTo(m_template2, CV_32FC(1), 1.0, 0);
    cv::matchTemplate(firstTemp, m_template2, secondTemp, CV_TM_SQDIFF);

    //data needs to be scaled because the results end up being
    //way too large
    secondTemp = secondTemp > (std::pow(2,64)*(2.5/10000));
    

    cutoffBuoys = cv::Mat::zeros(secondTemp.rows,secondTemp.cols,CV_8UC(1));
    

    //next a cutoff is applied to the images
    //searches for a blob
    int remove;
    int start;
    int maxWidth;
    for(int j = 0; j < secondTemp.cols; j=j+1)
    {
        for(int i = 0; i < secondTemp.rows; i=i+1)
        {
            //above loop conditions should be correct
            remove = 0;//whether to remove rows or not
            start = 0;//current line width
            maxWidth = -1;//max line width
            int x;
            int y;
            //if a blob is found
            if((secondTemp.at<unsigned char>(i,j) > m_cutoffZero) && (start == 0))
            {
                x = i;
                y = j;
                //begin finding the width of rows
                int a;
                while((secondTemp.at<unsigned char>(x,y) > m_cutoffZero) 
                      && ((x+1) < secondTemp.rows))
                {
                    //move to leftmost part of this row
                    a = 0;
                    while(a == 0)
                    {
                        if((y-1) < 0)
                        {
                            a = 1;
                        }
                        else
                        {
                            if(secondTemp.at<unsigned char>(x,y-1) < m_cutoffZero)
                            {
                                a = 1;
                            }
                            else 
                            {
                                y--;
                            }
                        }
                    }
                    j = y;
                    start = 0;
                    int temp;
                    temp = 0;
                    //begin counting the line length
                    while(y < secondTemp.cols)
                    {
                        start++;
                        //if not removing, copy over the value
                        if(remove == 0)
                        {
                            cutoffBuoys.at<unsigned char>(x,y) = 255;
                        }
                        else
                        {
                            cutoffBuoys.at<unsigned char>(x,y) = 0;
                        }
                        if(secondTemp.at<unsigned char>(x,y) <= m_cutoffZero)
                        {
                            secondTemp.at<unsigned char>(x,y) = 0;
                            temp = y;
                            y = secondTemp.cols;
                        }
                        else
                        {
                            secondTemp.at<unsigned char>(x,y) = 0;
                        }
                        y++;
                    }
                    x++;    
                    y = (j+temp)/2;
                    //update max width if needed
                    if(start > maxWidth)
                    {
                        maxWidth = start;
                    }
                    //switch to removing if cutoff length
                    //difference is passed
                    if((maxWidth - start) > m_cutoffLength)
                    {
                        remove = 1;
                    }
                }
            }
        }
    }
    //tempImage.copyTo(finalBuoys); 
    finalBuoys = cv::Mat::zeros(tempImage.rows,tempImage.cols,CV_8UC(3));
    for(int l = 0; l < cutoffBuoys.cols; l++)
    {
        for(int k = 0; k < cutoffBuoys.rows; k++)
        {
            
            if(cutoffBuoys.at<unsigned char>(k,l) == 255)
            {
                 finalBuoys.at<cv::Vec3b>(k+((finalBuoys.rows-cutoffBuoys.rows)/2+1)
                                         ,l+((finalBuoys.cols-cutoffBuoys.cols)
                                             /2+1)) = tempImage.at<cv::Vec3b>
                    (k+((finalBuoys.rows-cutoffBuoys.rows)/2+1)
                     ,l+((finalBuoys.cols-cutoffBuoys.cols)/2+1));
            }
            
        }
    }
       
    
  
    //std::cout<<"finished cutoff"<<std::endl;
    //std::cout<<"image shown"<<std::endl;
    //((IplImage*) *output) = (IplImage)finalBuoys;
    //output->setData(finalBuoys.data,false);
*/
    //std::cout<<"swordfish"<<std::endl;

}


void BuoyDetector::processBuoysImage(Image* input, Image* output)
{

    vision::OpenCVImage out(640, 480, vision::Image::PF_BGR_8);
    processBuoys(input,&out);
    if(output != 0)
    {
        processImage(&out,output);
    }
    else
    {
        processImage(&out);
    }

}




void BuoyDetector::processBuoysMask(cv::Mat* mask, Image* img, Image* output)
{
    mask = &finalBuoys;
    tempImage = img->asIplImage();
    double mean[3] = {0, 0, 0};
    double stDev[3] = {0, 0, 0};
    int n = 0;
    double delta;//a temp variable user to avoid overly complicated calculations
    //first compute mean and standard deviation
    //stDev stores the sum of squares and is made
    //into standard deviation at the end of this
    cv::split(tempImage, channels);
    for(int i = 0; i < 3; i++)
    {
        n = 0;
        for(int j = 0; j < tempImage.cols; j++)
        {
            for(int k = 0; k < tempImage.rows; k++)
            {
                n++;
                delta = (double)(channels[i].at<unsigned char>(k,j)) - mean[i];
                mean[i] = mean[i] + delta / n;
                stDev[i] = stDev[i] + delta * ((double)(channels[i].at<unsigned char>(k,j))
                                               - mean[i]);
            }
        }
        stDev[i] = sqrt(stDev[i] / (n - 1));
        stDev[i] = stDev[i] * stDevFactor;
    }
    //runs a hypothesis test on the elements denoted by the mask
    //the value of an element is set to 0 if it does not pass all tests
    //and is set to 254 if it passes all tests
    //95% confidence is used here
    boost::math::students_t dist(n-1);
    double t_value = -boost::math::quantile(dist,.025);
    //t-value obtained for sample size
    for(int p=0; p<3; p++)
        {std::cout<<mean[p]<<" "<<stDev[p]<<" "<<t_value<<std::endl;}
    //run tests on pixels in the mask
    std::cout<<"swordfish"<<std::endl;
    cv::imshow("initial mask",*mask);
    std::cout<<"fdsaf"<<std::endl;
    for(int j = 0; j < tempImage.cols; j=j+1)
    {
        for(int i = 0; i < tempImage.rows; i=i+1)
        {
            int x;
            int y;
            //if a blob is found
            if(mask->at<unsigned char>(i,j) == 255)
            {
                x = i;
                y = j;
                int a;
                while(mask->at<unsigned char>(x,y) == 255 && ((x+1) < tempImage.rows))
                {
                    //move to leftmost part of this row
                    a = 0;
                    while(a == 0)
                    {
                        if((y-1) < 0)
                        {
                            a = 1;
                        }
                        else
                        {
                            if(mask->at<unsigned char>(x,y) == 255)
                            {
                                a = 1;
                            }
                            else 
                            {
                                y--;
                            }
                        }
                    }
                    j = y;
                    int temp;
                    temp = 0;
                    //begin counting the line length
                    while(y < tempImage.cols)
                    {
                        //process pixels
                        if(mask->at<unsigned char>(x,y) == 255)
                        {
                            int val = 3;
                            for(int i = 0; i < 3; i++)
                            {
                                //the mean and standard deviation are not modified
                                //becuase the magnitude of the change at absoulte maximum
                                //is miniscule and can thus be ignored
                                double bound = t_value*stDev[i]*sqrt(1+(1/(n-1)));
                                if(abs(mean[i] - channels[i].at<unsigned char>(x,y))
                                   > bound)
                                {
                                    val--;
                                }
                            }
                            if(val == 0)
                            {
                                mask->at<unsigned char>(x,y) = 254;
                            }
                            else
                            {
                                mask->at<unsigned char>(x,y) = 0;
                            }
                        }
                        else
                        {
                            y = tempImage.cols;
                        }
                        y++;
                    }
                    x++;    
                    y = (j+temp)/2;
                    //end of processing line
                }
            }
        }
    }
    //loops finished
    //testing finished
    cv::imshow("test mask", *mask);
    cv::waitKey();


#if 0
    //currently this is just a skeleton of the loop to iterate through the points
    for(int j = 0; j < secondTemp.cols; j=j+1)
    {
        for(int i = 0; i < secondTemp.rows; i=i+1)
        {
            int x;
            int y;
            //if a blob is found
            if(/*pixel condition*/ && (start == 0))
            {
                x = i;
                y = j;
                int a;
                while(/*pixel condition*/ && ((x+1) < secondTemp.rows))
                {
                    //move to leftmost part of this row
                    a = 0;
                    while(a == 0)
                    {
                        if((y-1) < 0)
                        {
                            a = 1;
                        }
                        else
                        {
                            if(/*pixel condition*/)
                            {
                                a = 1;
                            }
                            else 
                            {
                                y--;
                            }
                        }
                    }
                    j = y;
                    start = 0;
                    int temp;
                    temp = 0;
                    //begin counting the line length
                    while(y < secondTemp.cols)
                    {
                        start++;
                        //process pixels
                        y++;
                    }
                    x++;    
                    y = (j+temp)/2;
                    //end of processing line
                }
            }
        }
    }
    //loops finished
#endif

}



void BuoyDetector::publishFoundEventKate(KeyPoint blob, Color::ColorType color)
{
    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();

    BuoyEventPtr event = BuoyEventPtr(new BuoyEvent());
    
    double centerX = 0, centerY = 0;
    Detector::imageToAICoordinates(frame, blob.pt.x, blob.pt.y,
                                   centerX, centerY);

	//blob.size (or any keypoint.size) returns the diameter of the meaningfull keypooint neighborhood
	//dont get it confused with keypoint.size() which will return the size of the keypoitn vector

   // double blobWidth = blob.getWidth();
   // double fracWidth = blobWidth / xPixelWidth;
   // double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));

    int minX = blob.pt.x-blob.size;
    int maxX = blob.pt.x+blob.size;
    int minY = blob.pt.y-blob.size;
    int maxY = blob.pt.y+blob.size;

    bool touchingEdge = false;
    if(minX == 0 || minY == 0 || maxX == xPixelWidth || maxY == yPixelHeight)
        touchingEdge = true;

    event->x = centerX;
    event->y = centerY;
    event->range = blob.size;
    event->azimuth = math::Degree((-1) * (xFOV / 2) * centerX);
    event->elevation = math::Degree((yFOV / 2) * centerY);
    event->color = color;
    event->touchingEdge = touchingEdge;

    publish(EventType::BUOY_FOUND, event);
}








void BuoyDetector::publishFoundEvent(BlobDetector::Blob& blob, Color::ColorType color)
{
    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();

    BuoyEventPtr event = BuoyEventPtr(new BuoyEvent());
    
    double centerX = 0, centerY = 0;
    Detector::imageToAICoordinates(frame, blob.getCenterX(), blob.getCenterY(),
                                   centerX, centerY);

    double blobWidth = blob.getWidth();
    double fracWidth = blobWidth / xPixelWidth;
    double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));

   
 int minX = blob.getMinX();
    int maxX = blob.getMaxX();
    int minY = blob.getMinY();
    int maxY = blob.getMaxY();

    bool touchingEdge = false;
    if(minX == 0 || minY == 0 || maxX == xPixelWidth || maxY == yPixelHeight)
        touchingEdge = true;

    event->x = centerX;
    event->y = centerY;
    event->range = range;
    event->azimuth = math::Degree((-1) * (xFOV / 2) * centerX);
    event->elevation = math::Degree((yFOV / 2) * centerY);
    event->color = color;
    event->touchingEdge = touchingEdge;

    publish(EventType::BUOY_FOUND, event);
}

void BuoyDetector::publishLostEvent(Color::ColorType color)
{
    BuoyEventPtr event(new BuoyEvent());
    event->color = color;
    
    publish(EventType::BUOY_LOST, event);
}

void BuoyDetector::drawBuoyDebug(Image* debugImage, BlobDetector::Blob &blob,
                   unsigned char red, unsigned char green,
                   unsigned char blue)
{
    CvPoint center;
    center.x = blob.getCenterX();
    center.y = blob.getCenterY();
    
    blob.drawStats(debugImage);
    blob.draw(debugImage);
    
    cvCircle(debugImage->asIplImage(), center, 3,
             cvScalar(blue, green, red), -1);
}

} // namespace vision
} // namespace ram
