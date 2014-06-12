/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/BinDetector.cpp
 */

// STD Includes
#include <iostream>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
//#include <cassert>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include <log4cpp/Category.hh>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include "math/include/Events.h"

// Project Includes
#include "vision/include/main.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/BinDetector.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"
#include "vision/include/DetectorMaker.h"
#include "vision/include/SymbolDetector.h"
#include "vision/include/ColorFilter.h"
#include "vision/include/WhiteBalance.h"
#include "vision/include/BuoyDetectorKate.h"

#include "math/include/Vector2.h"

#include "core/include/Logging.h"
#include "core/include/PropertySet.h"

static log4cpp::Category& logger(log4cpp::Category::getInstance("BinDetector"));

// extra boarder pixels to include when extracting sub-image of the bin
static const int BIN_EXTRACT_BORDER = 16;

namespace ram {
namespace vision {

//commented out by Kate since we're not using it and it threw up an unused error
//static bool binToCenterComparer(BinDetector::Bin b1, BinDetector::Bin b2)
//{
//    return b1.distanceTo(0,0) < b2.distanceTo(0,0);
//}
    
BinDetector::Bin::Bin() :
    TrackedBlob(),
    m_symbol(Symbol::NONEFOUND)
{
}
    
BinDetector::Bin::Bin(BlobDetector::Blob blob, Image* source,
                      math::Degree angle, int id, Symbol::SymbolType symbol) :
    TrackedBlob(blob, source, angle, id),
    m_symbol(symbol)
{
}
    
void BinDetector::Bin::draw(Image* image, Image* redImage)
{
    IplImage* out = image->asIplImage();

    // Draw green rectangle around the blob
    CvPoint tl; // top left
    CvPoint tr; // top right
    CvPoint bl; // bottom left
    CvPoint br; // bottom right
    int minX, maxX, minY, maxY;

    tl.x = bl.x = minX = getMinX();
    tr.x = br.x = maxX = getMaxX();
    tl.y = tr.y = minY = getMinY();
    bl.y = br.y = maxY = getMaxY();

    cvLine(out, tl, tr, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, tl, bl, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, tr, br, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, bl, br, CV_RGB(0,255,0), 3, CV_AA, 0);

    // Now draw my id
    std::stringstream ssId;
    ssId << getId();
    Image::writeText(image, ssId.str(), minX, maxY);

    // Draw the fill percentage
    if (redImage)
    {
        std::stringstream ssFillPct;
        ssFillPct << "F%: " << BinDetector::getRedFillPercentage(*this, redImage);
        int fillPctXOffset = -30;
        int fillPctYOffset = 0;
        Image::writeText(image, ssFillPct.str(),
                         maxX + fillPctXOffset,
                         minY + fillPctYOffset);
    }

    // Draw the angle
    std::stringstream ssAngle;
    ssAngle << std::setprecision(1) << getAngle().valueDegrees();
    int angleXOffset = -30;
    int angleYOffset = -15;
    Image::writeText(image, ssAngle.str(),
                     maxX + angleXOffset,
                     minY + angleYOffset);

    // Now do the symbol
    Image::writeText(image, Symbol::symbolToText(m_symbol), bl.x, bl.y - 15);
}
        
BinDetector::BinDetector(core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_debug(0),
    m_blobDetector(config, eventHub),
    m_symbolDetector(SymbolDetectorPtr()),
    m_found(false),
    m_centered(false),
    m_runSymbolDetector(true),
    m_logSymbolImages(false),
    m_percents(0),
    m_whiteMaskedFrame(0),
    m_blackMaskedFrame(0),
    m_redMaskedFrame(0),
    m_extractBuffer(0),
    m_scratchBuffer1(0),
    m_scratchBuffer2(0),
    m_scratchBuffer3(0),
    m_whiteMaskMinimumPercent(0),
    m_whiteMaskMinimumIntensity(0),
    m_blackMaskMinimumPercent(0),
    m_blackMaskMaxTotalIntensity(0),
    m_redErodeIterations(0),
    m_redDilateIterations(0),
    m_redOpenIterations(0),
    m_redCloseIterations(0),
    m_redMinPercent(0),
    m_redMinRValue(0),
    m_redMaxGValue(0),
    m_redMaxBValue(0),
    m_blobMinBlackPixels(0),
    m_blobMinWhitePixels(0),
    m_blobMinRedPercent(0),
    m_binMaxAspectRatio(0),
    m_binMinFillPercentage(0),
    m_binMaxOverlaps(0),
    m_binSameThreshold(0),
    m_binLostFrames(0),
    m_binHoughPixelRes(0),
    m_binHoughThreshold(0),
    m_binHoughMinLineLength(0),
    m_binHoughMaxLineGap(0),
    m_binID(0),
    m_whiteFilter(new ColorFilter(0, 255, 0, 255, 0, 255)),
    m_blackFilter(new ColorFilter(0, 255, 0, 255, 0, 255)),
    m_redFilter(new ColorFilter(0, 255, 0, 255, 0, 255)),
    m_frame(0)

{
    // Load all config based settings
    init(config);

    // Allocate images at default size
    allocateImages(640, 480);
    
    // By default we turn symbol detection on
    setSymbolDetectionOn(true);
}
    
    
BinDetector::~BinDetector()
{
    delete m_redFilter;
    delete m_frame;
 //   deleteImages();
}


bool BinDetector::found()
{
    return m_found;
}
    
float BinDetector::getX()
{
    if (m_bins.size() > 0)
        return m_bins.front().getX();
    else
        return 0;
}
float BinDetector::getY()
{
    if (m_bins.size() > 0)
        return m_bins.front().getY();
    else
        return 0;
}
math::Degree BinDetector::getAngle()
{
    if (m_bins.size() > 0)
        return m_bins.front().getAngle();
    else
        return math::Degree(0);
}

Symbol::SymbolType BinDetector::getSymbol()
{
    if (m_bins.size() > 0)
        return m_bins.front().getSymbol();
    else
        return Symbol::NONEFOUND;
}

BinDetector::BinList BinDetector::getBins()
{
    return m_bins;
}

void BinDetector::setSymbolDetectionOn(bool on)
{
    m_runSymbolDetector = on;
}

void BinDetector::setSymbolImageLogging(bool value)
{
    m_logSymbolImages = value;
}

    
void BinDetector::init(core::ConfigNode config)
{

  logger.info("Starting Bin Detector");
/*
    // Look up type for the symbol detector and validate it
    std::string symbolDetectorType = "BasicGladiatorDetector";
    if (config.exists("symbolDetector"))
        symbolDetectorType = config["symbolDetector"].asString();
    assert(vision::DetectorMaker::isKeyRegistered(symbolDetectorType) &&
           "Invalid symbol detector type");

    // Create the detector
    config.set("type", symbolDetectorType);
    DetectorPtr detector = vision::DetectorMaker::newObject(
        DetectorMakerParamType(config, core::EventHubPtr()));

    // Convert it the proper type
    m_symbolDetector =
        boost::dynamic_pointer_cast<SymbolDetector>(detector);
    assert(m_symbolDetector && "Symbol detector not of SymbolDetector type");

  */  
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());
/*
    // Add properties of the symbolDetector
    propSet->addPropertiesFromSet(m_symbolDetector->getPropertySet().get());
*/
    // Debug parameter
    propSet->addProperty(config, false, "debug",
                         "Debug parameter", 0, &m_debug, 0, 1);
/*
    // General properties
    propSet->addProperty(config, false, "centeredLimit",
        "Maximum distance for the bin to be considred \"centered\"",
        0.1, &m_centeredLimit, 0.0, 4.0/3.0);
    propSet->addProperty(config, false, "logSymbolImages",
        "Log all the images passed to the symbols detector to disk",
        false, &m_logSymbolImages);
    
    // Black mask properties
    propSet->addProperty(config, false, "blackIsRed",
        "Treat red as black",
        true, &m_blackIsRed);
    propSet->addProperty(config, false, "blackMaskMinimumPercent",
        "% of for the black mask minimum",
        10, &m_blackMaskMinimumPercent, 0, 100);
    propSet->addProperty(config, false, "blackMaskMaxTotalIntensity",
        "Maximum value of RGB pixels added together for black",
        350, &m_blackMaskMaxTotalIntensity, 0, 765);

    // White mask properties
    propSet->addProperty(config, false, "whiteMaskMinimumPercent",
        "% of for the white mask minimum",
        30, &m_whiteMaskMinimumPercent, 0, 100);
    propSet->addProperty(config, false, "whiteMaskMinimumIntensity",
        "Minimum value of RGB pixels added together for white",
        190, &m_whiteMaskMinimumIntensity, 0, 765);

    // Red mask properties
    propSet->addProperty(config, false, "redMinPercent",
        "Minimum percent of the total pixel value for red",
        35, &m_redMinPercent, 0, 255);
    propSet->addProperty(config, false, "redMinRValue",
        "Minimum value of Red pixel value for for red",
        125, &m_redMinRValue, 0, 255);
    propSet->addProperty(config, false, "redMaxGValue",
        "Maximum value of Green pixel value for for red",
        170, &m_redMaxGValue, 0, 255);
    propSet->addProperty(config, false, "redMaxBValue",
        "Maximum value of Blue pixel value for for red",
        170, &m_redMaxBValue, 0, 255);
    propSet->addProperty(config, false, "redOpenIterations",
        "Opening iterations on the red filtered image",
         0, &m_redOpenIterations, 0, 10);
    propSet->addProperty(config, false, "redCloseIterations",
        "Closing iterations on the red filtered image",
         0, &m_redCloseIterations, 0, 10);

    // Blob detector properties
    propSet->addProperty(config, false, "blobMinBlackPixels",
       "Minimum pixel count of a black blob",
        2500, &m_blobMinBlackPixels, 0, 10000);
    propSet->addProperty(config, false, "blobMinWhitePixels",
       "Minimum pixel count of a white blob",
        1500, &m_blobMinWhitePixels, 0, 10000);
    propSet->addProperty(config, false, "blobMinRedPercent",
       "Size of min red blob as a percent of the smallest black bin blob",
        10.0, &m_blobMinRedPercent, 0.0, 100.0);

    // Bin Determination properties 
    propSet->addProperty(config, false, "binMaxAspectRatio",
       "The maximum aspect ratio the black blob of a bin can have",
        3.0, &m_binMaxAspectRatio, 0.0, 10.0);
    propSet->addProperty(config, false, "binMinFillPrecentage",
       "The minimum amount of the black bob that must be filled to be a bin",
        0.0, &m_binMinFillPercentage, 0.0, 1.0);
    propSet->addProperty(config, false, "redMinFillPrecentage",
       "The minimum amount of the black blob that must be red to be a bin",
        0.0, &m_minRedFillPercentage, 0.0, 1.0);
    propSet->addProperty(config, false, "binMaxOverlaps",
       "The minimum amount of the black bob that must be filled to be a bin",
        20, &m_binMaxOverlaps, 0, 20);
    propSet->addProperty(config, false, "binSameThreshold",
       "The max distance between bins on different frames",
        0.2, &m_binSameThreshold, 0.0, 4.0/3.0);
    propSet->addProperty(config, false, "binLostFrames",
       "How many frames a bin must be missing before reporting lost",
        0, &m_binLostFrames, 0, 30);

    propSet->addProperty(config, false, "binHoughPixelRes",
        "Pixel resolution for hough based bin angle detection",
        3, &m_binHoughPixelRes, 0, 100); // 3 in Dans version
    propSet->addProperty(config, false, "binHoughThresdhold",
        "Threshold for hough based bin angle detection",
        110, &m_binHoughThreshold, 0, 300); // 150 in Dans version
    propSet->addProperty(config, false, "binHoughMinLineLength",
        "Minimum length of lines found when detemining bin angle",
        20, &m_binHoughMinLineLength, 0, 100); // 5 in Dans version
    propSet->addProperty(config, false, "binHoughMaxLineGap",
        "Maximum gap between lines for them to be joined",
        50, &m_binHoughMaxLineGap, 0, 300); // 50 in Dans version


    m_whiteFilter->addPropertiesToSet(propSet, &config,
                                      "whiteL", "Luminance",
                                      "whiteC", "Chrominance",
                                      "whiteH", "Hue",
                                      125, 255,  // L defaults // 180,255
                                      0, 255,  // U defaults // 76, 245
                                      0, 255); // V defaults // 200,255

    m_blackFilter->addPropertiesToSet(propSet, &config,
                                      "blackL", "Luminance",
                                      "blackC", "Chrominance",
                                      "blackH", "Hue",
                                      0, 124,  // L defaults // 180,255
                                      0, 255,  // U defaults // 76, 245
                                      0, 255); // V defaults // 200,255
*/
    
    propSet->addProperty(config, false, "binSameThreshold",
       "The max distance between bins on different frames",
        0.2, &m_binSameThreshold, 0.0, 40.0);
    propSet->addProperty(config, false, "binLostFrames",
       "How many frames a bin must be missing before reporting lost",
        0, &m_binLostFrames, 0, 30);
    propSet->addProperty(config, false, "redErodeIterations",
        "Erosion iterations on the red filtered image",
	1, &m_redErodeIterations, 0, 10);
    propSet->addProperty(config, false, "redDilateIterations",
        "Dilation iterations on the red filtered image",
         5, &m_redDilateIterations, 0, 10);

/*
    propSet->addProperty(config, false, "maxDistanceX",
        "max allowed distance in X between frames",
         40, &m_minDistanceX, 0, 500);

    propSet->addProperty(config, false, "maxDistanceX",
        "max allowed distance in Y between frames",
         40, &m_maxDistanceY, 0, 500);
*/

   m_redFilter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_redFilter->addPropertiesToSet(propSet, &config,
                                    "redL", "Luminance",
                                    "redC", "Chrominance",
                                    "redH", "Hue",
                                    0, 22,  // L defaults // 180,255
                                    130, 255,  // U defaults // 76, 245
                                    0, 255); // V defaults // 200,255

    m_frame = new OpenCVImage(640, 480, Image::PF_BGR_8);
	m_framecount = 0;

     

    // Make sure the configuration is valid
    //propSet->verifyConfig(config, true);

	//m_numberofclasses = 4; //four different bins
	//m_numberoftrainingimages = 6; //number of training images PER CLASS - so a total of 40 images
	//m_filepath= "/home/kmcbryan/Documents/RAM/tortuga/images/training/cropped"; //should be an input
	m_filetype  =".png";
	m_underscore ="_";
	m_D = "Descriptors";
	m_trainingsuccess =0;
	//m_binyml = "VisionTrainingtest.yml";

      /** Create the property object for you, load from the config */
    //template <typename T>
    //void addProperty(core::ConfigNode config, bool requireInConfig,
     //                const std::string& name, const std::string& desc, 
     //                T defaultValue, T* valuePtr);

//	propSet->addProperty(config,false,"BinYMLPath","BinYMLPath","VisionTrainingTest.yml",&m_binyml);

//	string temp = "BinYMLPath";
//    m_binyml=  config((string)"BinYMLPath");


	if (config.exists("BinYMLPath"))
        {
		m_binyml = config["BinYMLPath"].asString();
	}	
	else 
	{
		m_binyml = "VisionTrainingTest.yml";
	}

	if (config.exists("TrainingImagesFilePath"))
        {
		m_filepath = config["TrainingImagesFilePath"].asString();
	}	
	else 
	{
		m_filepath = "/home/kmcbryan/Documents/RAM/tortuga/images/training/cropped";
	}

	if (config.exists("SavingImagesFilePath"))
        {
		m_trainingpath = config["SavingImagesFilePath"].asString();
	}	
	else 
	{
		m_trainingpath = "/home/kmcbryan/Documents/RAM/tortuga/images/cropped";
	}



   // assert(vision::DetectorMaker::isKeyRegistered(symbolDetectorType) &&
   //        "Invalid symbol detector type");


    propSet->addProperty(config, false, "CalcTraining",
        "CalcTraining",false, &m_calcTraining);
    propSet->addProperty(config, false, "CompareBins",
        "CompareBins",false, &m_comparebins);
    propSet->addProperty(config, false, "SaveImages",
        "SaveImages",false, &m_saveimages);
  propSet->addProperty(config, false, "minHessian",
        "minHessian",
        250, &m_minHessian, 0, 5000); // 50 in Dans version
 propSet->addProperty(config, false, "NumberofImages",
        "NumberofImages",
        20, &m_numberoftrainingimages, 0, 30); // 50 in Dans version
 propSet->addProperty(config, false, "NumberofBinTypes",
        "NumberofBinTypes",
        8, &m_numberofclasses, 0, 20); // 50 in Dans version

 propSet->addProperty(config, false, "UpperLimitforBin",
       "UpperLimitforBin",
        2.0, &m_upperlimit, 0.0, 5.0); // 50 in Dans version

 propSet->addProperty(config, false, "MaxdistanceX",
        "maximum distance in X to be considered same bin as previous frame",
        90, &m_maxdistanceX, 0, 200); // 50 in Dans version

 propSet->addProperty(config, false, "maxdistsanceY",
       "maximum distance in Y to be considered same bin as previous frame",
        90, &m_maxdistanceY, 0, 200); // 50 in Dans version

    propSet->addProperty(config, false, "MinimumFrames",
        "Minimum Number of frames the bin id canbe assumed",
         5, &m_minAssumeFrame, 0, 20);


}

void BinDetector::allocateImages(int width, int height)
{
   // m_percents = new OpenCVImage(width, height);
   // m_whiteMaskedFrame = new OpenCVImage(width, height, Image::PF_BGR_8);
   // m_blackMaskedFrame = new OpenCVImage(width, height, Image::PF_BGR_8);
    //m_redMaskedFrame = new OpenCVImage(width, height, Image::PF_BGR_8);
    
    //int extra = BIN_EXTRACT_BORDER * 2;
    //size_t size = (width + extra) * (height + extra) * 3;
    //m_extractBuffer = new unsigned char[size];
    //m_scratchBuffer1 = new unsigned char[size];
    //m_scratchBuffer2 = new unsigned char[size];
    //m_scratchBuffer3 = new unsigned char[size];
}

void BinDetector::deleteImages()
{
    //delete m_percents;
    //delete m_whiteMaskedFrame;
    //delete m_blackMaskedFrame;
    //delete m_redMaskedFrame;
    //delete [] m_extractBuffer;
    //delete [] m_scratchBuffer1;
    //delete [] m_scratchBuffer2;
    //delete [] m_scratchBuffer3;
    delete m_redFilter;
    delete m_frame;
}
    
void BinDetector::filterForWhite(Image* input, Image* output)
{
    m_whiteFilter->filterImage(input, output);
}

void BinDetector::filterForBlack(Image* input, Image* output)
{
    m_blackFilter->filterImage(input, output);
  
    // And the red and black filter into the black
    if (m_blackIsRed)
    {
        unsigned char* blackData = m_blackMaskedFrame->getData();
        unsigned char* redData = m_redMaskedFrame->getData();
        int size = input->getWidth() * input->getHeight() * 3;
        for (int i = 0; i < size; ++i)
        {
            if (*redData)
                *blackData = 255;
            redData++;
            blackData++;
        }
    }
}

void BinDetector::filterForRed(Image* input, Image* output)
{
    m_redFilter->filterImage(input, output);

    cvSmooth(output->asIplImage(), output->asIplImage(), CV_MEDIAN, 5);

    if (m_redOpenIterations > 0)
    {
        IplImage *img = output->asIplImage();
        cvErode(img, img, NULL, m_redOpenIterations);
        cvDilate(img, img, NULL, m_redOpenIterations);
    }

    if (m_redCloseIterations > 0)
    {
        IplImage *img = output->asIplImage();
        cvDilate(img, img, NULL, m_redCloseIterations);
        cvErode(img, img, NULL, m_redCloseIterations);
    }

    if (m_redErodeIterations)
    {
      cvErode(output->asIplImage(), output->asIplImage(), 0, 
	      m_redErodeIterations);
    }
    if (m_redDilateIterations)
    {
      cvDilate(output->asIplImage(), output->asIplImage(), 0, 
	       m_redDilateIterations);
    }

}

void BinDetector::filterDebugOutput(Image* out)
{
    if (out)
    {
        int size = out->getWidth() * out->getHeight() * 3;
        unsigned char* outData = out->getData();
        unsigned char* whiteData = m_whiteMaskedFrame->getData();
        unsigned char* blackData = m_blackMaskedFrame->getData();
        unsigned char* redData = m_redMaskedFrame->getData();


        for (int count = 0; count < size; count += 3)
        {
            bool setColor = false;
            unsigned char R = 147;
            unsigned char G = 20;
            unsigned char B = 255;

            if ((whiteData[count] != 0) && (blackData[count] == 0))
            {
                // Make all white black
                R = G = B = 0;
                setColor = true;
            }
            else if ((blackData[count] != 0) && (whiteData[count] == 0))
            {
                // Make all black white
                R = G = B = 255;
                setColor = true;
            }
            else if  ((blackData[count] != 0) || (whiteData[count] != 0))
            {
                // else defaults to pink
                setColor = true;
            }

            if (redData[count] != 0)
            {
                // we have red
                R = 255;
                G = B = 0;
                setColor = true;
            }
            
            if (setColor)
            {
                outData[count] = B;
                outData[count+1] = G;
                outData[count+2] = R;
            }
        }
    }
}

void BinDetector::findBinBlobs(const BlobDetector::BlobList& whiteBlobs,
                               const BlobDetector::BlobList& blackBlobs,
                               BlobDetector::BlobList& binBlobs,
                               Image* output)
{
    BlobDetector::BlobList nonBinBlobs;
    BlobDetector::BlobList candidateBins;
    double averageHeight = 0;
    int foundBins = 0;

    // TODO: Add fill percenter, full height percentage, border width
    //       and aspect ratio to the configurable parameters


    // NOTE: all blobs sorted largest to smallest
    BOOST_FOREACH(BlobDetector::Blob blackBlob, blackBlobs)
    {
        bool matchedBlob = false;
        
        BOOST_FOREACH(BlobDetector::Blob whiteBlob, whiteBlobs)
        {
            // Sadly, this totally won't work at the edges of the screen...
            // crap damn.
            if (whiteBlob.containsInclusive(blackBlob, 2) &&
                (blackBlob.getAspectRatio() < m_binMaxAspectRatio) &&
                (blackBlob.getFillPercentage() > m_binMinFillPercentage) &&
		(getRedFillPercentage(blackBlob, m_redMaskedFrame)
		 >= m_minRedFillPercentage))
            {
                // blackBlobs[blackBlobIndex] is the black rectangle of a bin
                candidateBins.push_back(blackBlob);
                matchedBlob = true;
		averageHeight += blackBlob.getHeight();
                // Don't add it once for each white blob containing it,
                // thatd be dumb.
                break;
            }
        }

	if (!matchedBlob)
	    nonBinBlobs.push_back(blackBlob);
    }
    
    // Determine the average height
    foundBins = binBlobs.size();
    averageHeight /= foundBins;

    // Now lets go through the non bin blobs, and see if we can figure
    // out if they are black blobs that have just grown so large they only
    // have white on the sides
/*    BOOST_FOREACH(BlobDetector::Blob blackBlob, nonBinBlobs)
    {
        double blobHeight = blackBlob.getHeight();
        double imageHeight = m_whiteMaskedFrame->getHeight();
        if (((blobHeight / imageHeight) > 0.9) && 
            (blackBlob.getFillPercentage() > 0.8))
        {
            // We are a full height blob, which just might have white blobs on 
            // the side

            bool whiteLeft = false;
            bool whiteRight = false;

            // Attempt to find white blobs on the side
            BOOST_FOREACH(BlobDetector::Blob whiteBlob, whiteBlobs)
            {
                double whiteblobHeight = blackBlob.getHeight();
                if ((whiteblobHeight / imageHeight) > 0.9)
                {
                    // The white blob is full height as well
                    if (blackBlob.boundsIntersect(whiteBlob, 12))
                    {
                        if (whiteBlob.getCenterX() < blackBlob.getCenterX())
                            whiteLeft = true;
                        else
                           whiteRight = true;
                    }
                }
            } // foreach white blob

            // Determine if the black blob is on the edge
            bool onLeftEdge = blackBlob.getMinX() <= 12;
            bool onRightEdge = 
              blackBlob.getMaxX() >= (int)(m_whiteMaskedFrame->getWidth() - 12);
            
            // If we are the edge and have a white on the other side or
            // we have white on the both sides, we are a bin! If both sides
	    // are on the edge, we are not a bin
            if ((onLeftEdge || whiteLeft) && (onRightEdge || whiteRight) && 
                !(onLeftEdge && onRightEdge))
            {
       	        if (whiteLeft && whiteRight)
		{
		    // If both edges are white we need to be a proper aspect
		    // ratio
		    if (blackBlob.getAspectRatio() < 3)
		        candidateBins.push_back(blackBlob);
		}
		else 
		{
		    if (foundBins > 0)
		    {
		        // If we found a "normal" bin make sure this bins
		        // height is similar to that one
		        double heightError = 
			    fabs(blackBlob.getHeight() - averageHeight);
			if ((heightError / averageHeight) < 0.2)
			    candidateBins.push_back(blackBlob);
		    }
		    else
		    {
		        candidateBins.push_back(blackBlob);
		    }
		}
            }
        } // if (blob of full height)
    }
*/

    bool removed = true;
    while (removed)
    {
        removed = false;
        
        // Filter all the candidates and remove ones that overlap
        BlobDetector::BlobList::iterator mainIter = candidateBins.begin();
        BlobDetector::BlobList::iterator iterToRemove =
            candidateBins.end();
        int removeOverlaps = 0;
        while (mainIter != candidateBins.end())
        {
            // Count the number of overlaps
            int binOverlaps = 0;
            BlobDetector::BlobList::const_iterator iter = candidateBins.begin();
            while (iter != candidateBins.end())
            {
                if (mainIter->boundsIntersect(*iter) &&
                    (mainIter->getCenterX() != iter->getCenterX()) &&
                    (mainIter->getCenterY() != iter->getCenterY()))
                {
                    binOverlaps++;
                }
                iter++;
            }

            // Only add if its under the overlap count
            if (binOverlaps > m_binMaxOverlaps && binOverlaps > removeOverlaps)
            {
                removeOverlaps = binOverlaps;
                iterToRemove = mainIter;
                // We found a bad bin remove and start over again
                removed = true;
            }
            
            // Advance through
            mainIter++;
        }

        // Remove the bad bin
        if (removed)
        {
            candidateBins.erase(iterToRemove);
            if (output)
                iterToRemove->draw(output);
        }
    }

    // Anybody left is really a bin
    binBlobs = candidateBins;
}

bool BinDetector::findArrayAngle(const BinList& bins, math::Degree& finalAngle,
                                 Image* output)
{
    // Determine the angle of the bin array
    if (bins.size() > 1 && bins.size() <= 4)
    {
        int curX = -1;
        int curY = -1;
        int prevX = -1;
        int prevY = -1;
        int binsCenterX = 0;
        int binsCenterY = 0;
        //If you change this from a 3, also change the loops below
        double innerAngles[3];
        
        int angleCounter = 0;
        BOOST_FOREACH(Bin bin, bins)
        {
            binsCenterX += bin.getCenterX();
            binsCenterY += bin.getCenterY();
            prevX = curX;
            prevY = curY;
            curX = bin.getCenterX();
            curY = bin.getCenterY();
            
            if (prevX == -1 && prevY == -1)
            {
                // the first one
            }
            else
            {
                CvPoint prev;
                CvPoint cur;
                prev.x = prevX;
                prev.y = prevY;
                cur.x = curX;
                cur.y = curY;
                
                //Swap so we always get answers mod 180.
                if (prev.x > cur.x || (prev.x == cur.x && prev.y > cur.y))
                {
                    CvPoint swap = prev;
                    prev = cur;
                    cur = swap;
                }
                
                double innerAng = atan2(cur.y - prev.y,cur.x - prev.x);
                
                innerAngles[angleCounter] = innerAng;
                angleCounter++;

                // Draw line between bins
                if (output)
                {
                    cvLine(output->asIplImage(), prev, cur, CV_RGB(255,0,0), 5,
                           CV_AA, 0);
                }
            }
        }
        
        double sinTotal = 0;
        double cosTotal = 0;
        for (int i = 0; i < angleCounter && i < 3; i++)
        {
            sinTotal+=sin(innerAngles[i]);
            cosTotal+=cos(innerAngles[i]);
        }
        
        double finalAngleAcrossBins = atan2(sinTotal,cosTotal);

        // Draw line across all the bins
        if (output)
        {
            CvPoint drawStart, drawEnd;
            drawStart.x = binsCenterX/(angleCounter+1);
            drawStart.y = binsCenterY/(angleCounter+1);
            
            drawEnd.x = (int)(drawStart.x + cosTotal / (angleCounter) * 250);
            drawEnd.y = (int)(drawStart.y + sinTotal / (angleCounter) * 250);

            cvLine(output->asIplImage(), drawStart, drawEnd, CV_RGB(255,255,0),
                   5, CV_AA,0); 
        }
        //printf("final angle across bins %f:\n", finalAngleAcrossBins);
        
        math::Radian angleAcrossBins(finalAngleAcrossBins);
        
        finalAngle = math::Degree(-angleAcrossBins.valueDegrees());
        //printf("Final Inner Angle For Joe: %f\n", finalAngle.valueDegrees());
        return true;
    } // angle of bin array

    return false;
}

BinDetector::Bin BinDetector::processBin(BlobDetector::Blob bin,
                                         bool detectSymbol,
                                         int binNum, Image* output)
{
    // Get corners of area to extract (must be multiple of 4)
    int width = bin.getWidth()/4 * 4;
    int height = bin.getHeight()/4 * 4;
    
    int upperLeftX = bin.getCenterX() - width/2;
    int upperLeftY = bin.getCenterY() - height/2;
    int lowerRightX = bin.getCenterX() + width/2;
    int lowerRightY = bin.getCenterY() + height/2;

    // Make sure we are not outside the image
    upperLeftX = std::max(0, upperLeftX);
    upperLeftX = std::min((int)m_blackMaskedFrame->getWidth() - 1, upperLeftX);

    upperLeftY = std::max(0, upperLeftY);
    upperLeftY = std::min((int)m_blackMaskedFrame->getHeight() - 1, upperLeftY);

    lowerRightX = std::max(0, lowerRightX);
    lowerRightX = std::min((int)m_blackMaskedFrame->getWidth() - 1,
                           lowerRightX);

    lowerRightY = std::max(0, lowerRightY);
    lowerRightY = std::min((int)m_blackMaskedFrame->getHeight() - 1,
                           lowerRightY);
    
    Image* binImage = Image::extractSubImage(
        m_blackMaskedFrame, m_extractBuffer, upperLeftX, upperLeftY,
        lowerRightX, lowerRightY);

    // Determine angle of the bin
    math::Degree binAngle(0);
    calculateAngleOfBin(bin, binImage, binAngle, output);
    delete binImage; // m_extractBuffer free to use
    
    // Determine bin symbol if desired
    Symbol::SymbolType symbol = Symbol::NONEFOUND;
         
    if (detectSymbol)
    {
        // Extract red masked image
        Image* redBinImage = Image::extractSubImage(
            m_redMaskedFrame, m_extractBuffer, upperLeftX, upperLeftY,
            lowerRightX, lowerRightY);
        
        // Rotate to upright
        Image* rotatedBinImage =
            vision::Image::loadFromBuffer(m_scratchBuffer1,
                                          redBinImage->getWidth(),
                                          redBinImage->getHeight(),
                                          false,
                                          m_redMaskedFrame->getPixelFormat());
        
        vision::Image::transform(redBinImage, rotatedBinImage, binAngle);
        
        // Crop down Image to square around bin symbol
        Image* cropped = cropBinImage(rotatedBinImage, m_scratchBuffer3);

        if (cropped)
        {
            symbol = determineSymbol(redBinImage, m_scratchBuffer1, output);

            if (output && (binNum < 4))
            {
                // Scale the image to 128x128
                Image* scaledBin =
                    Image::loadFromBuffer(m_scratchBuffer1, 128, 128, false);
                cvResize(cropped->asIplImage(), scaledBin->asIplImage(),
                         CV_INTER_LINEAR);
                Image::drawImage(scaledBin, binNum * 128, 0, output, output);
                
                delete scaledBin; // m_scratchBuffer1 free to use
            }

            // Log the images if desired
            if (m_logSymbolImages)
                logSymbolImage(redBinImage, symbol);
            
            delete cropped;// m_scratchBuffer2 free to use
        }
        delete rotatedBinImage; // m_scratchBuffer1 free to use
        delete redBinImage;
    }
    
    // Report our results
    return Bin(bin, m_percents, binAngle, m_binID++, symbol);
}

double BinDetector::getRedFillPercentage(BlobDetector::Blob bin,
					 Image* redImage)
{
    // Get corners of area to extract (must be multiple of 4)
    int width = bin.getWidth()/4 * 4;
    int height = bin.getHeight()/4 * 4;
    
    int upperLeftX = bin.getCenterX() - width/2 - BIN_EXTRACT_BORDER;
    int upperLeftY = bin.getCenterY() - height/2 - BIN_EXTRACT_BORDER;
    int lowerRightX = bin.getCenterX() + width/2 + BIN_EXTRACT_BORDER;
    int lowerRightY = bin.getCenterY() + height/2 + BIN_EXTRACT_BORDER;

    // Make sure we are not outside the image
    upperLeftX = std::max(0, upperLeftX);
    upperLeftX = std::min((int)redImage->getWidth() - 1, upperLeftX);

    upperLeftY = std::max(0, upperLeftY);
    upperLeftY = std::min((int)redImage->getHeight() - 1, upperLeftY);

    lowerRightX = std::max(0, lowerRightX);
    lowerRightX = std::min((int)redImage->getWidth() - 1,
                           lowerRightX);

    lowerRightY = std::max(0, lowerRightY);
    lowerRightY = std::min((int)redImage->getHeight() - 1,
                           lowerRightY);


    // Get the red pixels
    int redPixels = Image::countWhitePixels(redImage,
					    upperLeftX, upperLeftY,
					    lowerRightX, lowerRightY);

    return ((double)redPixels) / ((double)(bin.getWidth() * bin.getHeight()));
					      //  return 0;
}

bool BinDetector::calculateAngleOfBin(BlobDetector::Blob bin, Image* input,
                                      math::Degree& foundAngle, Image* output)
{
    // Grab a gray scale version of the input image
    CvSize size = cvGetSize(input->asIplImage());
    IplImage* grayScale = cvCreateImageHeader(size, IPL_DEPTH_8U, 1);
    cvSetData(grayScale, m_scratchBuffer1, input->getWidth());
    cvCvtColor(input->asIplImage(), grayScale, CV_BGR2GRAY);

    // Grab a cannied version of our image
    IplImage* cannied = cvCreateImageHeader(size, IPL_DEPTH_8U, 1);
    cvSetData(cannied, m_scratchBuffer2, input->getWidth());
    cvCanny(grayScale, cannied, 50, 200, 3 );

    // Run the hough transform on the cannied image
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    
    lines = cvHoughLines2( cannied, storage, CV_HOUGH_PROBABILISTIC,
                           m_binHoughPixelRes,
                           CV_PI/180, m_binHoughThreshold,
                           m_binHoughMinLineLength, m_binHoughMaxLineGap);

    // Determine angle from hough transform
    float longestLineLength = -1;
    float angle = 0;
    for(int i = 0; i < lines->total; i++ )
    {
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
        float lineX = line[1].x - line[0].x; 
        float lineY = line[1].y - line[0].y;

        cvLine(input->asIplImage(), line[0], line[1], CV_RGB(255,255,0),
               5, CV_AA, 0);
        
        if (output && m_debug == 1)
        {
            line[0].x += bin.getCenterX() - input->getWidth() / 2;
            line[0].y += bin.getCenterY() - input->getHeight() / 2;
            line[1].x += bin.getCenterX() - input->getWidth() / 2;
            line[1].y += bin.getCenterY() - input->getHeight() / 2;
            
            cvLine(output->asIplImage(), line[0], line[1], CV_RGB(255,255,0),
                   5, CV_AA, 0);
        }
	//printf("Line dimensions: %f, %f\n", lineX, lineY);

        if (longestLineLength < (lineX * lineX + lineY * lineY))
        {
            angle = atan2(lineY,lineX);
            longestLineLength = lineX * lineX + lineY * lineY;
        }
    }

    bool success = false;
    if (lines->total != 0)
    {
        // We can only computer and angle if we found the lines
        double angInDegrees = math::Radian(angle).valueDegrees();
        double angleToReturn = 90-angInDegrees;
        
        if (angleToReturn >= 90)
            angleToReturn -= 180;
        else if (angleToReturn < -90)
            angleToReturn += 180;
        
        math::Degree finalJoeAngle(angleToReturn);
        foundAngle = finalJoeAngle;
        success = true;
    }
    
    cvReleaseImageHeader(&cannied);
    cvReleaseMemStorage(&storage);
    cvReleaseImageHeader(&grayScale);
    
    return success;
}


//Returns false on failure, puts symbol into scaledRedSymbol.
Image* BinDetector::cropBinImage(Image* redBinImage,
                                 unsigned char* storageBuffer)
{   
    int minSymbolX = redBinImage->getWidth() + 1;
    int minSymbolY = redBinImage->getHeight() + 1;
    int maxSymbolX = 0;
    int maxSymbolY = 0;
    
    // TODO: consider dilation of red image
    //cvDilate(redBinImage->asIplImage(), redBinImage->asIplImage(), NULL, 1);

    //   int size = 0;
    m_blobDetector.setMinimumBlobSize(100);
    m_blobDetector.processImage(redBinImage);
    if (!m_blobDetector.found())
    {
        // No symbol found, don't make a histogram
        return 0;
    }
    else
    {
        // TODO: General blob combine and apply it here to join broken up
        //       symbols
        
        //find biggest two blobs (hopefully should be just one, but if spade or club split..)
        std::vector<ram::vision::BlobDetector::Blob> blobs =
            m_blobDetector.getBlobs();
        ram::vision::BlobDetector::Blob biggest(-1,0,0,0,0,0,0);
        ram::vision::BlobDetector::Blob secondBiggest(0,0,0,0,0,0,0);
        ram::vision::BlobDetector::Blob swapper(-1,0,0,0,0,0,0);
        for (unsigned int blobIndex = 0; blobIndex < blobs.size(); blobIndex++)
        {
            if (blobs[blobIndex].getSize() > secondBiggest.getSize())
            {
                secondBiggest = blobs[blobIndex];
                if (secondBiggest.getSize() > biggest.getSize())
                {
                    swapper = secondBiggest;
                    secondBiggest = biggest;
                    biggest = swapper;
                }
            }
        }
        minSymbolX = biggest.getMinX();
        minSymbolY = biggest.getMinY();
        maxSymbolX = biggest.getMaxX();
        maxSymbolY = biggest.getMaxY();

/*        if (!m_incrediblyWashedOutImages)//A fancy way to say that at transdec, the symbols don't get split.
        {
        if (blobs.size() > 1)
        {
            if (minSymbolX > secondBiggest.getMinX())
                minSymbolX = secondBiggest.getMinX();
            if (minSymbolY > secondBiggest.getMinY())
                minSymbolY = secondBiggest.getMinY();
            if (maxSymbolX < secondBiggest.getMaxX())
                maxSymbolX = secondBiggest.getMaxX();
            if (maxSymbolY < secondBiggest.getMaxY())
                maxSymbolY = secondBiggest.getMaxY();

        }
        }*/
    }

    // Find the rows/cols of the croped image
    int onlyRedSymbolRows = (maxSymbolX - minSymbolX + 1);// / 4 * 4;
    int onlyRedSymbolCols = (maxSymbolY - minSymbolY + 1);// / 4 * 4;
    if (onlyRedSymbolRows == 0 || onlyRedSymbolCols == 0)
    {
        return 0;
    }

    // Make the image sqaure on its biggest dimension
    if (m_symbolDetector->needSquareCropped())
    {
        onlyRedSymbolRows = onlyRedSymbolCols = (onlyRedSymbolRows > onlyRedSymbolCols ? onlyRedSymbolRows : onlyRedSymbolCols);

    }

    // Sanity check on the sizes
    if (onlyRedSymbolRows >= (int)redBinImage->getWidth() ||
        onlyRedSymbolCols >= (int)redBinImage->getHeight())
    {
        return 0;
    }

    // Extract the symbol porition of the image
    int centerX = (maxSymbolX+minSymbolX)/2;
    int centerY = (maxSymbolY+minSymbolY)/2;
    int upperLeftX = centerX - onlyRedSymbolRows/2;
    int upperLeftY = centerY - onlyRedSymbolCols/2;
    int lowerRightX = centerX + onlyRedSymbolRows/2;
    int lowerRightY = centerY + onlyRedSymbolCols/2;

    // Make sure we are not outside the image
//    upperLeftX = std::max(0, upperLeftX);
//    upperLeftY = std::max(0, upperLeftY);
//    lowerRightX = std::min((int)redBinImage->getWidth() - 1, lowerRightX);
//    lowerRightY = std::min((int)redBinImage->getWidth() - 1, lowerRightY);

    Image* croppedImage = Image::extractSubImage(
        redBinImage, storageBuffer, upperLeftX, upperLeftY,
        lowerRightX, lowerRightY);

    return croppedImage;
}

Symbol::SymbolType BinDetector::determineSymbol(Image* input,
                                                unsigned char* scratchBuffer,
                                                Image* output)
{
    m_symbolDetector->processImage(input, output);
    // Filter symbol type
    Symbol::SymbolType symbolFound = m_symbolDetector->getSymbol(); 
    return symbolFound;
}

void BinDetector::logSymbolImage(Image* image, Symbol::SymbolType symbol)
{
    static int saveCount = 1;
    
    if (saveCount == 1)
    {
        // First run, make sure all the directories are created
        boost::filesystem::path base = core::Logging::getLogDir();

        BOOST_FOREACH(std::string name, Symbol::getSymbolNames())
        {
            boost::filesystem::path symbolDir = base / name;
            if (!boost::filesystem::exists(symbolDir))
                boost::filesystem::create_directories(symbolDir);
        }
    }

    // Determine the directory to place the image based on symbol
    boost::filesystem::path base = core::Logging::getLogDir();
    boost::filesystem::path baseDir = base / Symbol::symbolToText(symbol);

    // Determine the final path
    std::stringstream ss;
    ss << saveCount << ".png";
    boost::filesystem::path fullPath =  baseDir / ss.str();
    
    // Save the image and increment our counter
    Image::saveToFile(image, fullPath.string());
    saveCount++;
}




































void BinDetector::DetectorContours(Image* input)
{
/*
Kate Note:
This function is used to find the contour of the buoys
A color filter, in HSV space, is first used on a whitebalanced image
Then an erode function is used to clean up the results
The contours are found, and the maximum one with the correct aspect ratio is dubbed the buoy

makes use of find getsquareBlob() this is the part that takes the contours and pulls out the
buoy

*/
	
	cv::Mat img(640,480,CV_8UC3);
	img = input->asIplImage();
	img_whitebalance.create(img.size(),img.type());

	img_whitebalance = WhiteBalance(img);	
	//logger.infoStream() << " Whitebalance done";
	cv::Mat img_blur(img_whitebalance.size(),img_whitebalance.type());
	int ksize = 15;

	medianBlur(img_whitebalance, img_blur, ksize);
	//logger.infoStream() << " Median Blur done";

	int red_minH= m_redFilter->getChannel3Low();
	int red_maxH= m_redFilter->getChannel3High();
	//int green_minH= m_greenFilter->getChannel3Low();
	//int green_maxH= m_greenFilter->getChannel3High();
	int minS = m_redFilter->getChannel1Low();
	int maxS = m_redFilter->getChannel1High();

	int minV = m_redFilter->getChannel2Low(); 
	int maxV = m_redFilter->getChannel2High();

	int erosion_size = m_redErodeIterations;
	int dilate_size = m_redDilateIterations;

	//Initializae things
	blobfinder blob;
	cv::Mat img_hsv(img_whitebalance.size(),img_whitebalance.type());
	cv::Mat img_saturation(img_whitebalance.size(),CV_8UC1);
	cv::Mat img_red(img_whitebalance.size(),CV_8UC1);

	cv::Mat erode_dst_red(img_whitebalance.size(),CV_8UC1);
	cv::Mat erode_dst_redL(img_whitebalance.size(),CV_8UC1);
	cv::Mat erode_dst_redS(img_whitebalance.size(),CV_8UC1);

	cv::Mat dilate_dst_redS(img_whitebalance.size(),CV_8UC1);
	cv::Mat dilate_dst_redL(img_whitebalance.size(),CV_8UC1);
	cv::Mat dilate_dst_red(img_whitebalance.size(),CV_8UC1);

	cv::Mat img_red_final(img_whitebalance.size(),CV_8UC1);
	cv::Mat dilate_dst_redHL(img_whitebalance.size(),CV_8UC1);

	//get image into useable format and apply whitebalance;
	cvtColor(img_blur,img_hsv,CV_BGR2HSV);
	cv::vector<Mat> hsv_planes;
	cv::split(img_hsv,hsv_planes);

	//Color Filter
	Mat img_Luminance = blob.LuminanceFilter(hsv_planes,minV,maxV);
	img_saturation = blob.SaturationFilter(hsv_planes,minS,maxS);
	img_red =blob.OtherColorFilter(hsv_planes,red_minH,red_maxH);

	int erosion_type = 0; //morph rectangle type of erosion

	cv::Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );


	cv::Mat dilate_element = getStructuringElement( erosion_type,
                                       Size( 2*dilate_size + 1, 2*dilate_size+1 ),
                                       Point( dilate_size, dilate_size ) );

  	/// Apply the erosion operation 
	erode(img_red, erode_dst_red, element );
	erode(img_saturation, erode_dst_redS, element );
	erode(img_Luminance, erode_dst_redL, element );

	dilate(erode_dst_red, dilate_dst_red, dilate_element );
	dilate(erode_dst_redS, dilate_dst_redS, dilate_element );
	dilate(erode_dst_redL, dilate_dst_redL, dilate_element );
	//imshow("dilate S",dilate_dst_redS);
	//imshow("dilate L",dilate_dst_redL);

	//merge the dilated V & H and then and S	
	bitwise_and(dilate_dst_red,dilate_dst_redL, dilate_dst_redHL,noArray());
	bitwise_and(dilate_dst_redHL,dilate_dst_redS, img_red_final,noArray());

	//get Contours
	//logger.infoStream() << " Starting getSquareBlob, done with all color filters";
	int numberoftrackedcontours = 6;
	bincontours bins[numberoftrackedcontours];
	//printf("\n entering getSquareBlob");
	getSquareBlob(img_red_final, bins,numberoftrackedcontours);

	//allocate data
	m_Bin37Found =false;
	m_Bin98Found =false;
	m_Bin10Found =false;
	m_Bin16Found = false;
	m_BinoutlineFound=false;
//	for (int i=0;i<numberoftrackedcontours;i++)
//	{
//		if (bins[i].found == true);
//		{/
//			publishFoundEventSURF(bins[i]);
//		}//
//	}
//
publishFoundEventSURFAll();
	 if (m_Bin37Found == false && m_Bin37FoundBefore == true)
	{
		publishLostEvent(Symbol::BIN37);
	}
	 if (m_Bin98Found == false && m_Bin98FoundBefore == true)
	{
		publishLostEvent(Symbol::BIN98);
	}
	 if (m_Bin16Found == false && m_Bin16FoundBefore == true)
	{
		publishLostEvent(Symbol::BIN16);
	}
	 if (m_Bin10Found == false && m_Bin10FoundBefore == true)
	{
		publishLostEvent(Symbol::BIN10);
	}
	 if (m_BinoutlineFound == false && m_BinoutlineFoundBefore == true)
	{
		publishLostEvent(Symbol::BINOUTLINE);
	}
	m_Bin37FoundBefore= m_Bin37Found;
	m_Bin98FoundBefore= m_Bin98Found;
	m_Bin10FoundBefore= m_Bin10Found;
	m_Bin16FoundBefore= m_Bin16Found;
	m_BinoutlineFoundBefore= m_BinoutlineFound;
	
};


void BinDetector::getSquareBlob(Mat src, bincontours* bins, int numberoftrackedcontours)
{
	//finds the maximum contour that meets aspectratio
	double aspectratio = 1.0;
	double aspectratio_limit = 2.0;
	double aspectratio_diff=0;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	cv::Mat rotated(img_whitebalance.size(),img_whitebalance.type());
	

	  /// Find contours
	findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	//find contour with the largest area- by area I mean number of pixels
	RotatedRect temp,maxtemp;
	double area=0;

	//find larget area where it should be about a square
	//outline of the main box
	//actually want the top 9 outlines
	//largest = all bins
	//next 4 largest - contour between black and white inside bins
	//next 4 largest - contour of the yellow bottom

	Mat Mmap;
	Mat cropped[numberoftrackedcontours];
	Mat finalcropped[numberoftrackedcontours];
	Mat finalresize[numberoftrackedcontours];
	//bincontours bins[numberoftrackedcontours];

	//initialize to zero, just to verify everything is at zero
	for (int k=0;k<numberoftrackedcontours;k++)
	{
		bins[k].contournumber = 0;
		bins[k].area= 0;
		bins[k].found = false;
		bins[k].aspectratio_diff = 10;
		bins[k].maxX = 0;
		bins[k].minX = 0;
		bins[k].maxY = 0;
		bins[k].minY = 0;
		bins[k].vertices[0].x=0;
		bins[k].vertices[0].y=0;
		bins[k].vertices[1].x=0;
		bins[k].vertices[1].y=0;
		bins[k].vertices[2].x=0;
		bins[k].vertices[2].y=0;
		bins[k].vertices[3].x=0;
		bins[k].vertices[3].y=0;
		bins[k].angle = 0;
		bins[k].height=0;
		bins[k].width=0;
		

	}
	bool used; //used later on to when trying to find the top areas


	double minX=0,maxX=0,minY=0,maxY=0;
	int imageheight=0, imagewidth=0;
	imagewidth = img_whitebalance.cols;
	imageheight = img_whitebalance.rows;

	Point2f vertices[4];
//find the largest rectangular contour
//find the contours inside that one

	//printf("\n about to find largest");
	for(unsigned int j=0; j<contours.size(); j++)
	{
		if (contours[j].size()>5)
		{
			temp = minAreaRect(contours[j]); //finds the rectangle that will encompass all the points
			area = temp.size.width*temp.size.height;
			aspectratio_diff = abs((float(temp.size.height)/float(temp.size.width))- aspectratio);
			//printf("\n j = %d, countoursize = %d, area = %f, aspectratio_diff =%f, height = %f, width = %f",j,contours[j].size(),area,aspectratio_diff,temp.size.height,temp.size.width);
			//drawContours(img_whitebalance, contours, j, Scalar(255,0,0), 2, 8, hierarchy, 0, Point() );
			if (area > bins[0].area && aspectratio_diff < aspectratio_limit)
			{
				//printf(" beating max");
				bins[0].contournumber = j;
				bins[0].area = area;
				bins[0].aspectratio_diff = aspectratio_diff;
				//get the min and max points
				temp.points(vertices);
				minX= 90000;
				maxX = 0;
				minY= 90000;
				maxY=0;	
				for (int i = 0; i < 4; i++)
				{
					//printf("\n verticle = (%f, %f)",vertices[i].x, vertices[i].y);
					if (vertices[i].x < minX)
						minX = vertices[i].x;
					if (vertices[i].x > maxX)
						maxX = vertices[i].x;
					if (vertices[i].y < minY)
						minY = vertices[i].y;
					if (vertices[i].y > maxY)
						maxY = vertices[i].y;
					bins[0].vertices[i]= vertices[i];
				};
				if (maxX > imagewidth)
				{
					maxX = imagewidth;
				}
				if (maxY > imageheight)
				{
					maxY = imageheight;
				}
				if (minX < 0)
				{
					minX = 0;
				}
				if (minY < 0)
				{
					 minY = 0;
				}
				bins[0].maxX = maxX;
				bins[0].minX = minX;
				bins[0].maxY = maxY;
				bins[0].minY = minY;
				bins[0].found=true;
				bins[0].centerx = temp.center.x;
				bins[0].centery = temp.center.y;
				bins[0].angle = temp.angle;
				bins[0].identified = true;
				bins[0].type = 0;
				bins[0].width = temp.size.width;
				bins[0].height = temp.size.height;
			}
		}//end if size
	};
	//printf("\n have final");
	//printf("\n FINAL j = %d, countoursize = %d, area = %d, aspectratio_diff =%f",bins[0].contournumber,contours[bins[0].contournumber].size(),bins[0].area,bins[0].aspectratio_diff);
	//logger.infoStream() << " Largest Contour: Size = "<< contours[bins[0].contournumber].size() <<" ";
	for (int i = 0; i < 4; i++)
	{
		line(img_whitebalance, bins[0].vertices[i], bins[0].vertices[(i+1)%4], Scalar(255,0,255),8);
	}
	//given the vertices find the min and max X and min and maxY


	//have the largest one
	//now to find which ones are inside
	double minX2=0,minY2=0,maxX2=0,maxY2=0;
	int contourcounter =1;
	int k2=0,tick=0;
	Mat mapMatrix(2,3,CV_32FC1);
	//printf("\n allowable angle range = %f to %f",maxallowangle, minallowangle);
	for(unsigned int j=0; j<contours.size(); j++)
	{
		if ((contours[j].size() > 5)&&((int)j!=bins[0].contournumber) && (bins[0].found == true))
		{
			temp = minAreaRect(contours[j]); //finds the rectangle that will encompass all the points
			area = temp.size.width*temp.size.height;
			aspectratio_diff = abs((float(temp.size.height)/float(temp.size.width))- aspectratio);
			temp.points(vertices);
			minX2= 90000;
			maxX2 = 0;
			minY2= 90000;
			maxY2=0;	
			for (int i = 0; i < 4; i++)
			{
				//printf("\n verticle = (%f, %f)",vertices[i].x, vertices[i].y);
				if (vertices[i].x < minX2)
					minX2 = vertices[i].x;
				if (vertices[i].x > maxX2)
					maxX2 = vertices[i].x;
				if (vertices[i].y < minY2)
					minY2 = vertices[i].y;
				if (vertices[i].y > maxY2)
					maxY2 = vertices[i].y;
				
			};
			if (maxX2 > imagewidth)
					{
				maxX2 = imagewidth;
			}
			if (maxY2 > imageheight)
			{
				maxY2 = imageheight;
			}
			if (minX2 < 0)
			{
				minX2 = 0;
			}
			if (minY2 < 0)
			{
				 minY2 = 0;
			}
			if ((aspectratio_diff <aspectratio_limit)&& (minX2 >=bins[0].minX) && (maxX2<=bins[0].maxX) && (minY2>=bins[0].minY) &&(maxY2<=bins[0].maxY) )
			{
				used = false;
				for (int i =0;i<numberoftrackedcontours;i++)
				{
				 //put insize, in order from largest to smallest
					if (area > bins[i].area && used == false)
					{
						//take the one that its greater than, i and move it down
						//need to move all ones down first
						tick = 1;
						for (k2 =i+1;k2<numberoftrackedcontours;k2++)
						{
							//k3 = numberoftrackedcontours-tick;
							tick = tick+1;
							if ((numberoftrackedcontours-tick) > i && (numberoftrackedcontours-tick)<numberoftrackedcontours)
							{
								bins[numberoftrackedcontours-tick] = bins[numberoftrackedcontours-tick-1];
							}

							//i=2
							//k2 = 3, tick = 1 k3 = 5 bin[5]=bin[4]
							//k2 = 4, tick = 2 k3 = 4 bin[4]=bin[3]
						}
						//if area > bin[2] then i = 2
						//so then k2 = 
						bins[i].contournumber = j;
						bins[i].area = area;
						bins[i].aspectratio_diff = aspectratio_diff;
						bins[i].maxX = maxX2;
						bins[i].minX = minX2;
						bins[i].maxY = maxY2;
						bins[i].minY = minY2;
						bins[i].found = true;
						bins[i].centerx = temp.center.x;
						bins[i].centery = temp.center.y;
						bins[i].identified = false;
						bins[i].angle = temp.angle;
						bins[i].width = temp.size.width;
						bins[i].height = temp.size.height;
						bins[i].type =0;
						used = true;
						for (int i2 = 0; i2 < 4; i2++)
						{
						   bins[i].vertices[i2]= vertices[i2];
						}
						contourcounter = 1;
					}//end if
				}
			}
		}//end if size
	};
	//printf("\n should have all of them");

	//m_saveimages = false;
	Mat img_whitebalance_gray(img_whitebalance.size(),CV_8UC1);
	cvtColor(img_whitebalance,img_whitebalance_gray,CV_RGB2GRAY);
	for (int k=0;k<numberoftrackedcontours;k++)
	{
		
		if (bins[k].found==true )
		{
			//printf("\n angle = %f",bins[k].angle);
			temp = minAreaRect(contours[bins[k].contournumber]); //finds the rectangle that will encompass all the points
			//printf(" got temp");
			Mmap = getRotationMatrix2D(temp.center,temp.angle,1.0);
			////printf(" mmap center = %f, %f", temp.center.x, temp.center.y);
			warpAffine(img_whitebalance_gray, rotated, Mmap, img_whitebalance.size(), INTER_CUBIC);
			//printf(" warpAffine");
			//printf(" about to resize. size  %f,%f", temp.size.height, temp.size.width);
			if (temp.size.height < 10 || temp.size.width < 10)
			{	
				printf("\n unable to rescale bin"); 
				//make empty- just unable to get rotated image

			}
			else
			{
				getRectSubPix(rotated, temp.size, temp.center, cropped[k]);
						

				if (temp.size.width > temp.size.height)
				{
					//printf(" flipping");
					//need to transpose image
					//dont want to tranpose becase that... tranposes want to rotate, so I'll just flip it at the end
					transpose(cropped[k], finalcropped[k]);
					flip(finalcropped[k], finalcropped[k], 0); //0  flips vertical

				}
				else
				{
					//printf(" else");
					finalcropped[k] = cropped[k];	
				}

				resize(finalcropped[k], finalresize[k], Size(50,150), 0, 0, INTER_LINEAR );
			}
			//printf("\n angle = %f, width=%f, height = %f",temp.angle,temp.size.width,temp.size.height);

	/*
			if (k == 0)
				imshow("cropped",finalresize[k]);
			else if (k==1)
				imshow("cropped1",finalresize[k]);
			else if (k==2)
				imshow("cropped2",finalresize[k]);
			else if (k==3)s
				imshow("cropped3",finalresize[k]);
			else if (k==4)
				imshow("cropped4",finalresize[k]);
			else if (k==5)
				imshow("cropped5",finalresize[k]);
			else if (k==6)
				imshow("cropped",finalresize[k]);
	*/

			//need the angle - so take the vertices and find the one where the Y increases
			for (int i = 0; i < 4; i++)
			{		
				line(img_whitebalance, bins[k].vertices[i], bins[k].vertices[(i+1)%4], Scalar(255,255,0),5);
			}

		}
	}

	//feature detector	//save images for training purposes


//logger.infoStream() << "Have all the bins extracted now to start comparing";
//Have all the images now
	int j1;
	double avgDistance[m_numberofclasses];
	if (m_saveimages == true)
	{
		printf("\n\n Saving Training Images");
		//logger.infoStream() << "Saving Training Images";
		saveTrainingImages(finalresize);
	}
	if (m_calcTraining== true)
	{
		printf("\n\nTraining Bin Data, for %d classes with %d images each class",m_numberofclasses,m_numberoftrainingimages);
		calcTrainingData(); //gets the keypoints from the images which have previously been saved
				   //saves to a yml file
				//would like to do KNN of the keypoints		
		printf("\n Saved Training data!");
		m_trainingsuccess = 0;
	}

	printf("\n numberof classes = %d",m_numberofclasses);
	int foundBinType = 0;
	double foundBinValue = 100;
	int Bin37,Bin10,Bin16,Bin98;
	double Bin37value,Bin10value,Bin16value,Bin98value;
	Bin37value=900;
	Bin10value=900;
	Bin16value=900;
	Bin98value=900;
	Bin37 =0;
	Bin10=0;
	Bin16=0;
	Bin98=0;
	double minvalue;
	int findmatchesworked = 0;
	

	if (m_comparebins == true)
	{
		//do bin matching
		//step 1: load data
		if (m_trainingsuccess < 1)
		{
			m_trainingsuccess = getTrainingData(m_descriptors_object);
			if (m_trainingsuccess == 1)
			{
				//m_descriptors_object = descriptors_object; //so this line works
			}
 		}
	
	
		if (m_trainingsuccess == 1)
		{//can now do comparision
			//printf("\n  Have Training data!");
			for (int k=1;k<numberoftrackedcontours;k++)
			{
				if (bins[k].found==true)
				{	
					foundBinValue = 999;
					minvalue = 100;
					findmatchesworked = FindMatches(finalresize[k], avgDistance, m_descriptors_object);
					
					if (findmatchesworked == 1)
					{
						//printf("\n Image %d: ",k);
						for (j1 = 1;j1<=m_numberofclasses;j1++)
						{	
							if (avgDistance[j1-1]< foundBinValue)
							{
								foundBinValue = avgDistance[j1-1];
								foundBinType = j1;
							}
							if (avgDistance[j1-1]<minvalue)
								minvalue = avgDistance[j1-1];

		//logger.infoStream() << "Image Number" <<k <<" class: "<<j1 <<" avgDistance "<< avgDistance[j1-1] <<" ";
						//	printf("k=%d,class=%d, %f ",k,j1,avgDistance[j1-1]);
						}//end for j1
						printf("\n Image: =%d, BinType =%d, value =%f",k,foundBinType, foundBinValue);
		//logger.infoStream() << "Image Number" <<k <<" BinType: "<<foundBinType <<" Value "<< foundBinValue <<" ";

						//find the lowest value for each BinType
						//not just a matter of finding the lowest value for each one, because image k cannot be assigned to multiple bins

						//will I used the minimum value of this? and if so, only assign it to that bin - if it works
						if (avgDistance[0]<Bin37value)
						{
							Bin37value = avgDistance[0];
							Bin37=k;
						}
						if (avgDistance[1]<Bin10value)
						{
							Bin10value = avgDistance[1];
							Bin10=k;
						}
						if (avgDistance[2]<Bin98value)
						{
							Bin98value = avgDistance[2];
							Bin98=k;
						}
						if (avgDistance[3]<Bin16value)
						{
							Bin16value = avgDistance[3];
							Bin16=k;
						}

						if(m_numberofclasses>4)
						{
							if (avgDistance[4]<Bin37value)
							{
								Bin37value = avgDistance[4];
								Bin37=k;
							}
							if (avgDistance[5]<Bin10value)
							{
								Bin10value = avgDistance[5];
								Bin10=k;
							}
							if (avgDistance[6]<Bin98value)
							{
								Bin98value = avgDistance[6];
								Bin98=k;
							}
							if (avgDistance[7]<Bin16value)
							{
								Bin16value = avgDistance[7];
								Bin16=k;
							}
						}
					}
					else
					{
						//printf("\n Unable to retrieve Keypoints for image %d \n",k);
						//logger.infoStream() << "Unable to retrieve Keypoints for image" <<k ;

					}

				} //end if bins found
			}//end for k
		}//end if training data loaded
		else
		{
			printf("\n\n ERROR- UNABLE TO LOAD TRAINING YML");
			//logger.infoStream() << "Error UNABLE TO LOAD TRAINIGN YML ";

		}

	}

	//check to see if we have valid results
	if (Bin37 == Bin10  &&(Bin37value < Bin10value))
	{
		//Bin10 is not found
		Bin10 = 0;	
	}
	if (Bin37 == Bin16  &&(Bin37value < Bin16value))
	{
		Bin16 = 0;	
	}
	if (Bin37 == Bin98  &&(Bin37value < Bin98value))
	{
		Bin98 = 0;	
	}
	if (Bin10 == Bin37  &&(Bin10value < Bin37value))
	{
		Bin37 = 0;	
	}
	if (Bin10 == Bin16  &&(Bin10value < Bin16value))
	{
		Bin37 = 0;	
	}
	if (Bin10 == Bin98  &&(Bin10value < Bin98value))
	{
		Bin98 = 0;	
	}
	if (Bin98 == Bin10  &&(Bin98value < Bin10value))
	{
		Bin10 = 0;	
	}
	if (Bin98 == Bin16  &&(Bin98value < Bin16value))
	{
		Bin16 = 0;	
	}
	if (Bin98 == Bin37  &&(Bin98value < Bin37value))
	{
		Bin37 = 0;	
	}
	if (Bin16 == Bin10  &&(Bin16value < Bin10value))
	{
		Bin10 = 0;	
	}
	if (Bin16 == Bin37  &&(Bin16value < Bin37value))
	{
		Bin37 = 0;	
	}
	if (Bin16 == Bin98  &&(Bin16value < Bin98value))
	{
		Bin98 = 0;	
	}	
	
	//printf("\n Bin37 = %d at %f, Bin10=%d at %f, Bin16=%d at %f,Bin98=%d at %f",Bin37,Bin37value,Bin10,Bin10value,Bin16,Bin16value,Bin98,Bin98value);
	//logger.infoStream() << "Bin37" <<Bin37 <<" Value "<<Bin37value <<" ";
	//logger.infoStream() << "Bin98" <<Bin98 <<" Value "<<Bin98value <<" ";
	//logger.infoStream() << "Bin16" <<Bin16 <<" Value "<<Bin16value <<" ";
	//logger.infoStream() << "Bin10" <<Bin10 <<" Value "<<Bin10value <<" ";


	if (Bin37 > 0 && Bin37value <m_upperlimit)
	{
		drawContours(img_whitebalance, contours, bins[Bin37].contournumber, Scalar(0,0,255), 10, 8, hierarchy, 0, Point() );
		bins[Bin37].type = 37;
		bins[Bin37].identified = true;
	}
	if (Bin98 > 0 && Bin98value <m_upperlimit)
	{
		drawContours(img_whitebalance, contours, bins[Bin98].contournumber, Scalar(0,255,0), 10, 8, hierarchy, 0, Point() );
		bins[Bin98].type = 98;
		bins[Bin98].identified = true;
	}
	if (Bin10 > 0 && Bin10value <m_upperlimit)
	{
		drawContours(img_whitebalance, contours, bins[Bin10].contournumber, Scalar(255,0,0), 10, 8, hierarchy, 0, Point() );
		bins[Bin10].type = 10;
		bins[Bin10].identified = true;
	}
	if (Bin16 > 0 && Bin16value <m_upperlimit)
	{
		drawContours(img_whitebalance, contours, bins[Bin16].contournumber, Scalar(0,255,255), 10, 8, hierarchy, 0, Point() );
		bins[Bin16].type = 16;
		bins[Bin16].identified = true;
	}


	//have bins - so Now I need to publish the event

	//imshow("final",img_whitebalance); 
	//printf("\n done comparing\n");

	//logger.infoStream() << "Donen Comparing";


	//set them all into a single event
	for (int i=0;i<numberoftrackedcontours;i++)	
	{

		if (i == 0 && bins[0].found == true)
		{
			printf("\n setting m_allbins.MainBox_found = true");
			m_allbins.MainBox_found = true;
			m_allbins.MainBox_x = bins[i].centerx;	
			m_allbins.MainBox_y = bins[i].centery;	
			m_allbins.MainBox_angle = bins[i].angle;	
			m_allbins.MainBox_height = bins[i].height;
			m_allbins.MainBox_width = bins[i].width;
			m_allbins.MainBox_type = 1;
		}
		else if (bins[0].found == false)
		{	
			//unable to find main bin
			m_allbins.MainBox_found = false;
			m_allbins.MainBox_x = 0;	
			m_allbins.MainBox_y = 0;	
			m_allbins.MainBox_angle = 0;	
			m_allbins.MainBox_height =0;
			m_allbins.MainBox_width = 0;
			m_allbins.MainBox_type = 0;
		}
		else if (bins[i].found == true && bins[0].found == true && i>0 && i<5)
		{
			m_allbins.Box[i-1].Box_found = true;	
			m_allbins.Box[i-1].Box_x = bins[i].centerx;
			m_allbins.Box[i-1].Box_y = bins[i].centery;
			m_allbins.Box[i-1].Box_angle =bins[i].angle;
			if (bins[i].height > bins[i].width)
			{
				m_allbins.Box[i-1].Box_width =bins[i].width;
				m_allbins.Box[i-1].Box_height =bins[i].height;
			}
			else
			{
				m_allbins.Box[i-1].Box_width =bins[i].height;
				m_allbins.Box[i-1].Box_height =bins[i].width;
			}
			m_allbins.Box[i-1].Box_identified = bins[i].identified;	
			
			//m_allbins.Box[i-1].Box_numberofframes =0;
			if (bins[i].identified == true && (bins[i].type == 16 ||bins[i].type == 10 ||bins[i].type == 98|| bins[i].type == 37))
				m_allbins.Box[i-1].Box_type =bins[i].type;
			else
			{
				m_allbins.Box[i-1].Box_type = 0;	
				m_allbins.Box[i-1].Box_identified = false;
			}	

		}//end else
		else if(i>0 && i<5)
		{
			m_allbins.Box[i-1].Box_found = false;	
			m_allbins.Box[i-1].Box_x = 0;
			m_allbins.Box[i-1].Box_y = 0;
			m_allbins.Box[i-1].Box_angle =0;
			m_allbins.Box[i-1].Box_height =0;
			m_allbins.Box[i-1].Box_width =0;
			m_allbins.Box[i-1].Box_identified = false;	
			m_allbins.Box[i-1].Box_numberofframes =0;
			m_allbins.Box[i-1].Box_type =0;
		}
	}//for i
		
if (m_framecount > 2)		
	checkPreviousFrames();

for (int i=0;i<numberoftrackedcontours;i++)	
{
	printf("\n Allbins = boxfound = %d, Type %d, numberofframes = %d",m_allbins.Box[i].Box_found, m_allbins.Box[i].Box_type,m_allbins.Box[i].Box_numberofframes);
}

//Draw boxes
if (m_allbins.MainBox_found== true)
{		
	for (int i=0;i<numberoftrackedcontours;i++)
	{
		if (m_allbins.Box[i].Box_found == true && m_allbins.Box[i].Box_type == 16)
		{
			cv::circle(img_whitebalance, cvPoint(m_allbins.Box[i].Box_x,m_allbins.Box[i].Box_y),m_allbins.Box[i].Box_width, Scalar(0,255,255), 10, 8);
		}
		if (m_allbins.Box[i].Box_found == true && m_allbins.Box[i].Box_type == 10)
		{
			cv::circle(img_whitebalance, cvPoint(m_allbins.Box[i].Box_x,m_allbins.Box[i].Box_y),m_allbins.Box[i].Box_width, Scalar(255,0,0), 10, 8);
		}
		if (m_allbins.Box[i].Box_found == true && m_allbins.Box[i].Box_type == 37)
		{
			cv::circle(img_whitebalance, cvPoint(m_allbins.Box[i].Box_x,m_allbins.Box[i].Box_y),m_allbins.Box[i].Box_width, Scalar(0,0,255), 10, 8);
		}
		if (m_allbins.Box[i].Box_found == true && m_allbins.Box[i].Box_type == 98)
		{
			cv::circle(img_whitebalance, cvPoint(m_allbins.Box[i].Box_x,m_allbins.Box[i].Box_y),m_allbins.Box[i].Box_width, Scalar(0,255,0), 10, 8);
		}
	}


}

	return;
}

void BinDetector::calcTrainingData()
	{
	//load training images
	int numberofclasses = m_numberofclasses; //four different bins
	int numberoftrainingimages = m_numberoftrainingimages; //number of training images PER CLASS - so a total of 40 images

	//all training images are the same size - they better be
	string filepath= m_filepath; //should be an input
	string classnumber;
	string imagenumber;
	string filename;
	stringstream ss_class, ss_number;

	string filetype  =m_filetype;
	string underscore =m_underscore;
	string D = m_D;
	
	//m_binyml = "VisionTrainingtest.yml";
	m_filepath = filepath;

	int rownumber;
	string DescriptorName;
	Mat row; 
        std::vector<KeyPoint> keypoints_object[numberofclasses*numberoftrainingimages];
	SurfFeatureDetector detector( m_minHessian );
	SurfDescriptorExtractor extractor;
        Mat descriptors_object[numberofclasses*numberoftrainingimages];
	
	//m_minHessian = 300;
	printf("\n minHessian = %d",m_minHessian);
	FileStorage fs(m_binyml, FileStorage::WRITE);
	for (int k=1;k<numberofclasses+1;k++)
	{

		ss_class.str("");
		ss_class<< k;
		classnumber=ss_class.str();
	
		for (int i=1;i<numberoftrainingimages+1;i++)
		{	
			ss_number.str(""); // clear

			filename ="";
			imagenumber="";
			ss_number<< i;
			imagenumber = ss_number.str();
			filename = filepath+classnumber+underscore+imagenumber+filetype;
			Mat training = imread(filename, CV_LOAD_IMAGE_GRAYSCALE); //should probably preload Mat
			if (!training.data)
			{
				printf("\n ERROR UNABLE TO LOAD IMAGE class %d, # %d, ",k,i);
				char* a = new char[filename.size()+1];
						a[filename.size()] = 0;
				memcpy(a,filename.c_str(),filename.size());
				printf("%s",a);	
			}
			else
			{
				imshow("training",training);
				cvWaitKey(100);
				//Can I get the detector points?
				 //-- Step 1: Detect the keypoints using SURF Detector
				rownumber = (k-1)*numberoftrainingimages+(i-1);
				  detector.detect(training, keypoints_object[rownumber] );
				if (keypoints_object[rownumber].size() > 0 && keypoints_object[rownumber].size()<99999)
				{
					//printf("\n number of keypoints = %d in %d,%d",keypoints_object[rownumber].size(),k,i);
					  //-- Step 2: Calculate descriptors (feature vectors)
					  extractor.compute(training, keypoints_object[rownumber], descriptors_object[rownumber] );
					//take the top N? data points, max feature points is the max size of the image
					//but I only want Mat_descriptors_object, thanI want to save it... and then load it

					DescriptorName = D+classnumber+underscore+imagenumber;
					write(fs,DescriptorName, descriptors_object[rownumber]);  
				}
				else
				{
					 printf(" \n ERROR IN FINDING KEYPOINTS IN TRAINING DATA %d, %d",k,i);
				}
			}
		} //end for i
	}//end for k
 	fs.release();
return;
};

int BinDetector::getTrainingData(Mat* descriptors_object)
{
	 
   	//read in training data saved in m_filename;
      	//load file
	
char* a = new char[m_binyml.size()+1];
	a[m_binyml.size()] = 0;
	memcpy(a,m_binyml.c_str(),m_binyml.size());
	//printf("\n bin YML file: ");
	//printf("%s",a);	

	FileStorage fs(m_binyml, FileStorage::READ);
	fs.open(m_binyml, FileStorage::READ);
	printf("\n opening binyml file");
	stringstream ss_class, ss_number;
	string classnumber;
	string imagenumber;
	string DescriptorName;
	int rownumber;
        if(fs.isOpened())
        {
		for (int k=1;k<m_numberofclasses+1;k++)
		{
			ss_class.str(""); // clear
			ss_class<< k;
			classnumber = ss_class.str();
			for (int i=1;i<m_numberoftrainingimages+1;i++)
			{
				ss_number.str(""); // clear
				ss_number<< i;
				imagenumber = ss_number.str();
				DescriptorName = m_D+classnumber+m_underscore+imagenumber;
				rownumber = (k-1)*m_numberoftrainingimages+(i-1);
			        read(fs[DescriptorName], descriptors_object[rownumber]);  
			} //end for i
		} ///end for k 
		fs.release();
		return(1);
	}  //end if 
	else
	{
		fs.release();
		return(0);
	}
};

void BinDetector::saveTrainingImages(Mat* finalresize)
{
	string filepath;
	string imagenumber;
	string filetype  =m_filetype;
	string filename;
	stringstream ss;
	ss.str(""); // clear
	ss<< m_framecount;
	imagenumber = ss.str();

filepath = m_trainingpath;
	
	filename = filepath+"0_"+imagenumber+filetype;
       	imwrite(filename,finalresize[0]);

	filename = filepath+"1_"+imagenumber+filetype;
       	imwrite(filename,finalresize[1]);

	filename = filepath+"2_"+imagenumber+filetype;
       	imwrite(filename,finalresize[2]);

	filename = filepath+"3_"+imagenumber+filetype;
       	imwrite(filename,finalresize[3]);

	filename = filepath+"4_"+imagenumber+filetype;
       	imwrite(filename,finalresize[4]);
	
	printf("\n Training Data Collected for this Frame");
	return;
}


int BinDetector::FindMatches(Mat image, double* avgDistance, Mat* descriptors_object)
{	

	//printf("\n finding matches");
	//m_minHessian = 300;
	 FlannBasedMatcher matcher;
	std::vector< DMatch > matches0;
	//printf("\n m_minHessian= %d",m_minHessian);
	SurfFeatureDetector detector( m_minHessian );
	SurfDescriptorExtractor extractor;
	int j1,j2,i2;
	double max_dist,min_dist,total_dist;
	Mat descriptors_bin_test;

	std::vector<KeyPoint> keypoints_bin_test0;
	int rownumber;

	//Get keypoints of potential bin
	detector.detect(image, keypoints_bin_test0);
	extractor.compute(image, keypoints_bin_test0, descriptors_bin_test);
	double dist;
	double total_mindist;
	int trainingimagesused = 0;
	if (keypoints_bin_test0.size() > 0)
	{
		for (j1 = 1;j1<m_numberofclasses+1;j1++)
		{
		  	  max_dist = 0;
			  min_dist = 10000;
			  total_dist= 0;
			  avgDistance[j1-1] = 0;
			 trainingimagesused = 0;
			total_mindist = 0;
	    		  for (j2 = 1;j2<m_numberoftrainingimages+1;j2++)
			  {
				trainingimagesused = trainingimagesused+1;
				rownumber = (j1-1)*m_numberoftrainingimages+(j2-1);
				//match with training images
			       // printf("\n Descriptors= %d",(descriptors_object[rownumber].rows));

				matcher.match(descriptors_object[rownumber],descriptors_bin_test, matches0 );
				//printf(" \n matched0 =%d",matches0.size());
				
								
				  for( i2= 0; i2<(int) matches0.size(); i2++ )
				  { dist = matches0[i2].distance;
				   if( dist < min_dist ) min_dist = dist;
				   if( dist > max_dist ) max_dist = dist;
				   total_dist = dist+total_dist;
				  }
				total_mindist = min_dist+total_mindist;
				if (matches0.size() == 0)
				{
					total_dist = total_dist+1000;
					total_mindist = total_mindist + 100;
				}
			}//end for j2
			//avgDistance[j1-1] = ((float)total_dist/(float)trainingimagesused);
			avgDistance[j1-1] = ((float)total_mindist)/((float)trainingimagesused);
			//printf("\n class= %d, avgDistance %f, minDist=%f",j1, avgDistance[j1-1],total_mindist);
	     }//end for j1
             return(1);
	}//end if keypoints good
	else
	{
		//printf("\n ERROR- Unable to find Keypoints from the test image");
		return(0);
	}
}; //end function



void BinDetector::publishFoundEventSURF(bincontours bin)
{
/*
    BinEventPtr event(new BinEvent());  
    double centerX = 0, centerY = 0;
    Detector::imageToAICoordinates(m_frame, bin.centerx, bin.centery,
                                   centerX, centerY);

    event->id = bin.identified;
    event->x = centerX;
    event->y = centerY;
    event->range = bin.width;
	if (bin.type == 0)
	{
		  event->symbol =Symbol::BINOUTLINE;
		m_BinoutlineFound = true;
	}
	else if (bin.type == 1)
	{
	    event->symbol = Symbol::BIN37;
		m_Bin37Found = true;
	}
	else if (bin.type == 2)
	{
	    event->type = Symbol::BIN98;
		m_Bin98Found = true;
	}
	else if (bin.type == 3)
	{
	    event->type = Symbol::BIN10;
		m_Bin10Found = true;
	}
	else if (bin.type == 4)
	{
	    event->type = Symbol::BIN16;
		m_Bin16Found = true;
	}
    event->angle = bin.angle;
    publish(EventType::BIN_FOUND, event);
*/
}

void BinDetector::publishLostEvent(Symbol::SymbolType color)
{
    BinEventPtr event(new BinEvent());
    event->id = color;
    
    publish(EventType::BINS_LOST, event);
}


void BinDetector::publishFoundEventSURFAll()
{

	BinEventPtr event(new BinEvent());  

	double centerMainX = 0, centerMainY = 0, centerBinX=0,centerBinY=0;

	if (m_allbins.MainBox_found == true && (int)m_allbins.MainBox_x>0 &&(int)m_allbins.MainBox_y>0 &&(int)m_allbins.MainBox_x <640 && 
		(int)m_allbins.MainBox_y<640)
	{
		//valid main box    		
		Detector::imageToAICoordinates(m_frame, (int)m_allbins.MainBox_x,(int)m_allbins.MainBox_y,
                                   centerMainX, centerMainY);
	}
	else
	{
		centerMainX = 0;
		centerMainY=0;
		m_allbins.MainBox_found = false;
	}

	//event->x = centerMainX;
	//event->y = centerMainY;
	//event->range = m_allbins.MainBox_width;
	//event->angle = m_allbins.MainBox_angle;
	math::Vector3 vectorbin0;
	vectorbin0.x = centerMainX;
	vectorbin0.y = centerMainY;
	vectorbin0.z = m_allbins.MainBox_width;	
	event->angle = m_allbins.MainBox_angle;
	//send rest of the bins now
	if (m_allbins.Box[0].Box_found == true && (int)m_allbins.Box[0].Box_x > 0 && (int)m_allbins.Box[0].Box_x<640 && (int)m_allbins.Box[0].Box_y>0 
		&& (int)m_allbins.Box[0].Box_y<640)
	{
		Detector::imageToAICoordinates(m_frame, (int)m_allbins.Box[0].Box_x,(int)m_allbins.Box[0].Box_y,
                                   centerBinX, centerBinY);
	}
	else
	{	
	  m_allbins.Box[0].Box_found = false;	
	  centerBinX=0;
	  centerBinY=0;
	}
 	
	math::Vector3 vectorbin1;
	vectorbin1.x = centerBinX;
	vectorbin1.y = centerBinY;
	vectorbin1.z = m_allbins.Box[0].Box_width;
	if (m_allbins.Box[0].Box_found == false)
	{	
		event->type1 = -2;
	}	
	else if (m_allbins.Box[0].Box_identified == false)
	{	
		event->type1=-2;
	}	
	else
	{
		event->type1= m_allbins.Box[0].Box_type;
	}
	
	 //send rest of the bins now
	if (m_allbins.Box[1].Box_found == true &&(int)m_allbins.Box[1].Box_x > 0 &&(int)m_allbins.Box[1].Box_x<640 &&(int)m_allbins.Box[1].Box_y>0
		 &&(int)m_allbins.Box[1].Box_y<640)
	{
		Detector::imageToAICoordinates(m_frame, (int)m_allbins.Box[1].Box_x,(int)m_allbins.Box[1].Box_y,
                                   centerBinX, centerBinY);
	}
	else
	{	
	  m_allbins.Box[1].Box_found = false;	
	  centerBinX=0;
	  centerBinY=0;
	}
	math::Vector3 vectorbin2;
	vectorbin2.x = centerBinX;
	vectorbin2.y = centerBinY;
	vectorbin2.z = m_allbins.Box[1].Box_width;;
	if (m_allbins.Box[1].Box_found == false)
	{	
		event->type2 = -2;
	}	
	else if (m_allbins.Box[1].Box_identified == false)
	{	
		event->type2=-2;
	}	
	else
	{
		event->type2= m_allbins.Box[1].Box_type;
	}

	 //send rest of the bins now
	if (m_allbins.Box[2].Box_found == true && (int)m_allbins.Box[2].Box_x > 0 && (int)m_allbins.Box[2].Box_x<640 && (int)m_allbins.Box[2].Box_y>0 && 			(int)m_allbins.Box[2].Box_y<640)
	{
		Detector::imageToAICoordinates(m_frame, (int)m_allbins.Box[2].Box_x,(int)m_allbins.Box[2].Box_y,
                                   centerBinX, centerBinY);
	}
	else
	{	
	  m_allbins.Box[2].Box_found = false;	
	  centerBinX=0;
	  centerBinY=0;
	}
	math::Vector3 vectorbin3;
	vectorbin3.x = centerBinX;
	vectorbin3.y = centerBinY;
	vectorbin3.z = m_allbins.Box[2].Box_width;
	if (m_allbins.Box[2].Box_found == false)
	{	
		event->type3 = -2;
	}	
	else if (m_allbins.Box[2].Box_identified == false)
	{	
		event->type3=-2;
	}	
	else
	{
		event->type3= m_allbins.Box[2].Box_type;
	}

	 //send rest of the bins now
	if (m_allbins.Box[3].Box_found == true && (int)m_allbins.Box[3].Box_x > 0 && (int)m_allbins.Box[3].Box_x<640 && (int)m_allbins.Box[3].Box_y>0 && 			(int)m_allbins.Box[3].Box_y<640)
	{
		Detector::imageToAICoordinates(m_frame, (int)m_allbins.Box[3].Box_x,(int)m_allbins.Box[3].Box_y,
                                   centerBinX, centerBinY);
	}
	else
	{	
	  m_allbins.Box[3].Box_found = false;	
	  centerBinX=0;
	  centerBinY=0;
	}
	math::Vector3 vectorbin4;
	vectorbin4.x = centerBinX;
	vectorbin4.y = centerBinY;
	vectorbin4.z = m_allbins.Box[3].Box_width;

	if (m_allbins.Box[3].Box_found == false)
	{	
		event->type4 = -2;
	}	
	else if (m_allbins.Box[3].Box_identified == false)
	{	
		event->type4=-2;
	}	
	else
	{
		event->type4= m_allbins.Box[3].Box_type;
	}
 
	event->vectorbin0 = vectorbin0;
	event->vectorbin1 = vectorbin1;
	event->vectorbin2 = vectorbin2;
	event->vectorbin3 = vectorbin3;
	event->vectorbin4 = vectorbin4;

    publish(EventType::BIN_FOUND, event);
}









void BinDetector::checkPreviousFrames()
{
	printf("\n nchecking previous frames");

	//Have the previous frames bins and also have the new frames' bin
	//now lets compare them!

	int maxdistanceX = m_maxdistanceX;
	int maxdistanceY= m_maxdistanceY;
	int distanceX=0,distanceY=0;
	//check main bin first
	int minDistance[4];
	minDistance[0]=1000;
	minDistance[1]=1000;
	minDistance[2]=1000;
	minDistance[3]=1000;

	int minDistanceBin[4];
	minDistanceBin[0]=0;
	minDistanceBin[1]=0;
	minDistanceBin[2]=0;
	minDistanceBin[3]=0;

	int tempDistance;
	printf(" previous = %d, all = %d", m_previousbins.MainBox_found, m_allbins.MainBox_found);
	if (m_previousbins.MainBox_found == true && m_allbins.MainBox_found == true)
	{
		printf("\n both boxes are found");
		distanceX= abs(m_allbins.MainBox_x-m_previousbins.MainBox_x);
		distanceY= abs(m_allbins.MainBox_y-m_previousbins.MainBox_y);
		if ((distanceX < maxdistanceX) &&(distanceY<maxdistanceY))
		{
			//same main bin as previoius
			//good. Now check the rest
			for (int i= 0;i<5;i++)
			{
				if (m_allbins.Box[i].Box_found == true)
				{
					for (int j= 0;j<5;j++)
					{
						if (m_previousbins.Box[j].Box_found == true)
						{
						//printf("\n found previous bin and found current bin");
							tempDistance = abs(m_allbins.Box[i].Box_x-m_previousbins.Box[j].Box_x)+
									abs(m_allbins.Box[i].Box_y-m_previousbins.Box[j].Box_y);
							if ((abs(m_allbins.Box[i].Box_x-m_previousbins.Box[j].Box_x) < maxdistanceX) && 
								(abs(m_allbins.Box[i].Box_y-m_previousbins.Box[j].Box_y) <maxdistanceY) && (tempDistance < minDistance[i]))
						 	{
								minDistance[i] = tempDistance;
								minDistanceBin[i] = j;
							}
						}
					}//end int j
					//printf(" min distance = %d", minDistance[i]);
					if (minDistance[i] < (maxdistanceX+maxdistanceY)/2)
					{
						//printf(" same bin");
						//same bin as seen before
						//check the ID 
						if (m_allbins.Box[i].Box_identified== true && m_previousbins.Box[minDistanceBin[i]].Box_identified == true)
						{
							//do they match? 
							if (m_allbins.Box[i].Box_type== m_previousbins.Box[minDistanceBin[i]].Box_type)
							{
								//good, increase the numberof frame value
								if (m_allbins.Box[i].Box_type == 16)
								{
									if (m_bin16frames < 1 || m_bin16frames > m_framecount)
										m_bin16frames = 0;

									m_bin16frames = m_bin16frames +1;
									m_allbins.Box[i].Box_numberofframes = m_bin16frames;
								}
								else if (m_allbins.Box[i].Box_type == 10)
								{
									if (m_bin10frames < 1 || m_bin10frames > m_framecount)
										m_bin10frames = 0;

									m_bin10frames = m_bin10frames +1;
									m_allbins.Box[i].Box_numberofframes = m_bin10frames;
								}
							
								if (m_allbins.Box[i].Box_type == 37)
								{
									if (m_bin37frames < 1 || m_bin37frames > m_framecount)
										m_bin37frames = 0;

									m_bin37frames = m_bin37frames +1;
									m_allbins.Box[i].Box_numberofframes = m_bin37frames;
								}
							
								if (m_allbins.Box[i].Box_type == 98)
								{
									if (m_bin98frames < 1 || m_bin98frames > m_framecount)
										m_bin98frames = 0;

									m_bin98frames = m_bin98frames +1;
									m_allbins.Box[i].Box_numberofframes = m_bin98frames;
								}

								

								//printf(" matched id as before = %d", m_allbins.Box[i].Box_numberofframes);
							}
							else
							{ 
								//they dont match
								if (m_allbins.Box[i].Box_numberofframes >m_minAssumeFrame)
								{ 
									//need to make sure I dont have two of the same bin types now
									bool idusedbefore = false;
									for (int i2= 0;i2<5;i2++)
									{
										if (i2 != i && m_allbins.Box[i2].Box_found == true && m_allbins.Box[i2].Box_type ==  m_previousbins.Box[minDistanceBin[i]].Box_type)
										{
											idusedbefore = true;
										}
									}
									if (idusedbefore == false)
									{
										m_allbins.Box[i].Box_type = m_previousbins.Box[minDistanceBin[i]].Box_type;
										if (m_allbins.Box[i].Box_type == 16)
										{
											if (m_bin16frames < 1 || m_bin16frames > m_framecount)
												m_bin16frames = 0;

											m_bin16frames = m_bin16frames -1;
											m_allbins.Box[i].Box_numberofframes = m_bin16frames;
										}
										else if (m_allbins.Box[i].Box_type == 10)
										{
											if (m_bin10frames < 1 || m_bin10frames > m_framecount)
												m_bin10frames = 0;

											m_bin10frames = m_bin10frames -1;
											m_allbins.Box[i].Box_numberofframes = m_bin10frames;
										}
							
										if (m_allbins.Box[i].Box_type == 37)
										{
											if (m_bin37frames < 1 || m_bin37frames > m_framecount)
												m_bin37frames = 0;

											m_bin37frames = m_bin37frames -1;
											m_allbins.Box[i].Box_numberofframes = m_bin37frames;
										}
							
										if (m_allbins.Box[i].Box_type == 98)
										{
											if (m_bin98frames < 1 || m_bin98frames > m_framecount)
												m_bin98frames = 0;

											m_bin98frames = m_bin98frames -1;
											m_allbins.Box[i].Box_numberofframes = m_bin98frames;
										}



									}
									else
									{
										//reset counter can't assume the type- so keep as what was found
										if (m_allbins.Box[i].Box_type == 16)
										{
											m_bin16frames = 0;
											m_allbins.Box[i].Box_numberofframes = m_bin16frames;
										}
										else if (m_allbins.Box[i].Box_type == 10)
										{
											m_bin10frames = 0;
											m_allbins.Box[i].Box_numberofframes = m_bin10frames;
										}
							
										if (m_allbins.Box[i].Box_type == 37)
										{
											m_bin37frames = 0;
											m_allbins.Box[i].Box_numberofframes = m_bin37frames;
										}
							
										if (m_allbins.Box[i].Box_type == 98)
										{
											m_bin98frames = 0;
											m_allbins.Box[i].Box_numberofframes = m_bin98frames;
										}

									}
									
								}
								else
								{
									//reset counter can't assume the type- so keep as what was found
									if (m_allbins.Box[i].Box_type == 16)
									{
										m_bin16frames = 0;
										m_allbins.Box[i].Box_numberofframes = m_bin16frames;
									}
									else if (m_allbins.Box[i].Box_type == 10)
									{
										m_bin10frames = 0;
										m_allbins.Box[i].Box_numberofframes = m_bin10frames;
									}
							
									if (m_allbins.Box[i].Box_type == 37)
									{
										m_bin37frames = 0;
										m_allbins.Box[i].Box_numberofframes = m_bin37frames;
									}
							
									if (m_allbins.Box[i].Box_type == 98)
									{
										m_bin98frames = 0;
										m_allbins.Box[i].Box_numberofframes = m_bin98frames;
									}

								}
								//printf(" No match = %d", m_allbins.Box[i].Box_numberofframes);
							}
						}//end if have IDs for both of them
						else if (m_allbins.Box[i].Box_identified== true)
						{
							//only have current bin
							if (m_allbins.Box[i].Box_type == 16)
							{
								m_bin16frames = 0;
								m_allbins.Box[i].Box_numberofframes = m_bin16frames;
							}
							else if (m_allbins.Box[i].Box_type == 10)
							{
								m_bin10frames = 0;
								m_allbins.Box[i].Box_numberofframes = m_bin10frames;
							}
							if (m_allbins.Box[i].Box_type == 37)
							{
								m_bin37frames = 0;
								m_allbins.Box[i].Box_numberofframes = m_bin37frames;
							}
							if (m_allbins.Box[i].Box_type == 98)
							{
								m_bin98frames = 0;
								m_allbins.Box[i].Box_numberofframes = m_bin98frames;
							}


						}
						else if (m_previousbins.Box[minDistanceBin[i]].Box_identified == true && m_allbins.Box[i].Box_numberofframes >m_minAssumeFrame)
						{
							//can assume the old one was correct- if its not alredy found
							bool idusedbefore = false;
							for (int i2= 0;i2<5;i2++)
							{
								if (i2 != i && m_allbins.Box[i2].Box_found == true && m_allbins.Box[i2].Box_type ==  m_previousbins.Box[minDistanceBin[i]].Box_type)			{
									idusedbefore = true;
								}
							}
							if (idusedbefore == false)
							{
								m_allbins.Box[i].Box_type = m_previousbins.Box[minDistanceBin[i]].Box_type;	
								if (m_allbins.Box[i].Box_type == 16)
								{
									m_bin16frames = m_bin16frames -1;
									m_allbins.Box[i].Box_numberofframes = m_bin16frames;
								}
								else if (m_allbins.Box[i].Box_type == 10)
								{
									m_bin10frames = m_bin10frames -1;
									m_allbins.Box[i].Box_numberofframes = m_bin10frames;
								}
							
								if (m_allbins.Box[i].Box_type == 37)
								{
									m_bin37frames = m_bin37frames -1;
									m_allbins.Box[i].Box_numberofframes = m_bin37frames;
								}
							
								if (m_allbins.Box[i].Box_type == 98)
								{
									m_bin98frames = m_bin98frames -1;
									m_allbins.Box[i].Box_numberofframes = m_bin98frames;
								}
							
							
								m_allbins.Box[i].Box_identified= true;
							}
							else
							{
								m_allbins.Box[i].Box_type = 0;
							}


							
							//again, need to make sure there aren't more than one of these bin types
						}
					}//end if (minDistance[i] < (minDistanceX+minDistanceY)/2)
					else
					{
					  	//unable to tell if they are the same bin
						//so do nothing
						//m_allbins.Box[i].Box_numberofframes = 0;
			
					}
				} //end if m_allbins = true
			}//end int i

		} //end same main bin
	} //end previous bins exist

	//if bin isn't found, make sure its counter is reset
	if ( m_allbins.MainBox_found == true)
	{
		m_Bin37Found = false;
		m_Bin98Found=false;
		m_Bin10Found=false;
		m_Bin16Found=false;
		for (int i= 0;i<5;i++)
		{
			if (m_allbins.Box[i].Box_found == true)
			{

				if (m_allbins.Box[i].Box_type == 16)
				{
					m_Bin16Found = true;
				}
				if (m_allbins.Box[i].Box_type == 10)
				{
					m_Bin10Found = true;
				}
				if (m_allbins.Box[i].Box_type == 37)
				{
					m_Bin37Found = true;
				}
				if (m_allbins.Box[i].Box_type == 98)
				{
					m_Bin98Found = true;
				}

			} //end if inner bin found

		} //end for i
		if (m_Bin37Found == false)
			m_bin37frames =0;
		if (m_Bin16Found == false)
			m_bin16frames =0;
		if (m_Bin10Found == false)
			m_bin10frames =0;
		if (m_Bin98Found == false)
			m_bin98frames =0;
	}

}


void BinDetector::processImage(Image* input, Image* out)
{
    m_frame->copyFrom(input);
  	if(m_framecount < 2)
	{
		m_trainingsuccess =0;
		m_bin16frames=0;
		m_bin37frames=0;
		m_bin10frames=0;
		m_bin98frames=0;
	}

   	m_framecount =  m_framecount+1;
	//printf("\n framecount = %d", m_framecount);

	if (m_framecount > 1 && m_allbins.MainBox_found==true)
	{
		m_previousbins= m_allbins;
		//printf("\n saving allbins as previousbins: found = %d", m_previousbins.MainBox_found);
	}

    DetectorContours(input); //find bins

	//Have the previous frames bins and also have the new frames' bin
	//now lets compare them!


	cvtColor(img_whitebalance,img_whitebalance,CV_RGB2BGR);

	//imshow("greenAND",erode_dst_green);
	//imshow("sat",img_saturation);
	//imshow("yellowerosion",erode_dst_yellow);
	//imshow("rederosion",erode_dst_red);

       input->setData(img_whitebalance.data,false);
       m_frame->copyFrom(input);
 /*
    // Ensure all the images are the proper size
    if ((m_whiteMaskedFrame->getWidth() != m_frame->getWidth()) || 
        (m_whiteMaskedFrame->getHeight() != m_frame->getHeight()))
    {
        // We are the wrong size delete them and recreate
        deleteImages();
        allocateImages(m_frame->getWidth(), m_frame->getHeight());
    }
*/
    // Make debug output look like m_frame (will be marked up later)
    if (out)
        out->copyFrom(m_frame);
/*    
    // Convert the image to LCh

   // m_frame->setPixelFormat(Image::PF_RGB_8);
   // m_frame->setPixelFormat(Image::PF_LCHUV_8);
    
    // Filter for white, black, and red
   // filterForWhite(m_frame, m_whiteMaskedFrame);
   // filterForRed(m_frame, m_redMaskedFrame);
   // filterForBlack(m_frame, m_blackMaskedFrame);
    
    // Update debug image with black, white and red color info
   // filterDebugOutput(out);


    // Find all the white blobs
    m_blobDetector.setMinimumBlobSize(m_blobMinWhitePixels);
    m_blobDetector.processImage(m_whiteMaskedFrame);
    BlobDetector::BlobList whiteBlobs = m_blobDetector.getBlobs();
    
    // Find all the black blobs
    m_blobDetector.setMinimumBlobSize(m_blobMinBlackPixels);
    m_blobDetector.processImage(m_blackMaskedFrame);
    BlobDetector::BlobList blackBlobs = m_blobDetector.getBlobs();

    // Find bins
    BlobDetector::BlobList binBlobs;
    findBinBlobs(whiteBlobs, blackBlobs, binBlobs, out);
    
    if (out)
    {
        std::stringstream ss;
        ss << "Bin#: " << binBlobs.size();
        Image::writeText(out, ss.str(), out->getWidth() / 2,
                         out->getHeight() - 15); 
    }

    // Process the individual bins if we have any
    if (binBlobs.size() > 0)
    {
        // We found bins
        m_found = true;
        
        // Process bins to determine there angle and symbol
        BinList newBins;

        int binNumber = 0;
        BOOST_FOREACH(BlobDetector::Blob binBlob, binBlobs)
        {
            Bin newBin = processBin(binBlob, m_runSymbolDetector,
                                    binNumber, out);
            newBins.push_back(newBin);
            binNumber++;
        }

        // Sort through our new bins and match them to the old ones
        TrackedBlob::updateIds(&m_bins, &newBins, &m_lostBins,
                               m_binSameThreshold, m_binLostFrames);

        // Anybody left we didn't find this iteration, so its been dropped
        BOOST_FOREACH(Bin bin, m_bins)
        {
            BinEventPtr event(new BinEvent(bin.getX(), bin.getY(), 0,
                                           bin.getSymbol(), bin.getAngle()));
            event->id = bin.getId();
            publish(EventType::BIN_DROPPED, event);
        }

        // Our new bins are now "the bins", and sort then in relation to the
        // center of the image
        m_bins = newBins;
        m_bins.sort(binToCenterComparer);

        // Determine angle of the array of bins and publish the event
        math::Degree arrayAngle;
        if (findArrayAngle(m_bins, arrayAngle, out))
        {
            // It was a valid angle, send it out
            BinEventPtr event(new BinEvent(arrayAngle));
            publish(EventType::MULTI_BIN_ANGLE, event);
        }

        // Now publish the centered events
        math::Vector2 toCenter(getX(), getY());
        if (toCenter.normalise() < m_centeredLimit)
        {
            if(!m_centered)
            {
                m_centered = true;
                BinEventPtr event(new BinEvent(getX(), getY(), 0,
                                               getSymbol(), getAngle()));
                publish(EventType::BIN_CENTERED, event);
            }
        }
        else
        {
            m_centered = false;
        }

        
        // Send bin events and draw debug update 
        BOOST_FOREACH(Bin bin, m_bins)
        {
            // Draw the debug bin output
            if (out)
                bin.draw(out);

            // Send out the bin event
            BinEventPtr event(new BinEvent(bin.getX(), bin.getY(), 0,
                                           bin.getSymbol(), bin.getAngle()));
            event->id = bin.getId();
            publish(EventType::BIN_FOUND, event);
        }

    }
    else if (m_found)
    {
        // Lets update the ids with no new bins
        BinList emptyBins;
        TrackedBlob::updateIds(&m_bins, &emptyBins, &m_lostBins,
                               m_binSameThreshold, m_binLostFrames);

        // Anybody left has run out of lost frames so its been dropped
        BOOST_FOREACH(Bin bin, m_bins)
        {
            BinEventPtr event(new BinEvent(bin.getX(), bin.getY(), 0,
                                           bin.getSymbol(), bin.getAngle()));
            event->id = bin.getId();
            publish(EventType::BIN_DROPPED, event);
        }

        // Our new bins are now "the bins"
        m_bins = emptyBins;

        if (0 == m_lostBins.size())
        {
            // Publish lost event
            m_found = false;
            m_centered = false;
            publish(EventType::BINS_LOST, core::EventPtr(new core::Event()));
        }
    }
*/
}





























} // namespace vision
} // namespace ram
