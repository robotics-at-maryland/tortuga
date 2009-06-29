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
//#include <cassert>

// Library Includes
#include "cv.h"
#include "highgui.h"
#include <log4cpp/Category.hh>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/BinDetector.h"
#include "vision/include/Camera.h"
#include "vision/include/Events.h"

#include "math/include/Vector2.h"

#include "core/include/Logging.h"
#include "core/include/PropertySet.h"

//static log4cpp::Category& LOGGER(log4cpp::Category::getInstance("Vision"));

static const int BIN_EXTRACT_BORDER = 16;

namespace ram {
namespace vision {

static bool binToCenterComparer(BinDetector::Bin b1, BinDetector::Bin b2)
{
    return b1.distanceTo(0,0) < b2.distanceTo(0,0);
}
    
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
    
void BinDetector::Bin::draw(Image* image)
{
    IplImage* out = image->asIplImage();
    // Draw green rectangle around the blob
    CvPoint tl,tr,bl,br;
    tl.x = bl.x = getMinX();
    tr.x = br.x = getMaxX();
    tl.y = tr.y = getMinY();
    bl.y = br.y = getMaxY();
    cvLine(out, tl, tr, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, tl, bl, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, tr, br, CV_RGB(0,255,0), 3, CV_AA, 0);
    cvLine(out, bl, br, CV_RGB(0,255,0), 3, CV_AA, 0);

    // Now draw my id
    std::stringstream ss;
    ss << getId();
    Image::writeText(image, ss.str(), tl.x, tl.y);

    std::stringstream ss2;
    ss2 << getAngle().valueDegrees();
    Image::writeText(image, ss2.str(), br.x-30, br.y-15);

    // Now do the symbol
    Image::writeText(image, Symbol::symbolToText(m_symbol), bl.x, bl.y - 15);
}
        
BinDetector::BinDetector(core::ConfigNode config,
                         core::EventHubPtr eventHub) :
    Detector(eventHub),
    m_blobDetector(config, eventHub),
    m_symbolDetector(config,eventHub),
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
    m_whiteMaskMinimumPercent(0),
    m_whiteMaskMinimumIntensity(0),
    m_blackMaskMinimumPercent(0),
    m_blackMaskMaxTotalIntensity(0),
    m_redMinPercent(0),
    m_redMinRValue(0),
    m_redMaxGValue(0),
    m_redMaxBValue(0),
    m_blobMinBlackPixels(0),
    m_blobMinWhitePixels(0),
    m_blobMinRedPercent(0),
    m_binMaxAspectRatio(0),
    m_binSameThreshold(0),
    m_binHoughPixelRes(0),
    m_binHoughThreshold(0),
    m_binHoughMinLineLength(0),
    m_binHoughMaxLineGap(0),
    m_binID(0)
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
    deleteImages();
}

void BinDetector::processImage(Image* input, Image* out)
{
    // Ensure all the images are the proper size
    if ((m_whiteMaskedFrame->getWidth() != input->getWidth()) || 
        (m_whiteMaskedFrame->getHeight() != input->getHeight()))
    {
        // We are the wrong size delete them and recreate
        deleteImages();
        allocateImages(input->getWidth(), input->getHeight());
    }

    // Make debug output look like input (will be marked up later)
    if (out)
        out->copyFrom(input);
    
    // Create the percents images for the white, black and red filters
    m_percents->copyFrom(input);
    to_ratios(m_percents->asIplImage());
    
    // Filter for white, black, and red
    filterForWhite(input, m_whiteMaskedFrame);
    filterForRed(input, m_redMaskedFrame);
    filterForBlack(input, m_blackMaskedFrame);

    // And the red and black filter into the black
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
    
    // Update debug image with black, white and red color info
    filterDebugOutput(out);

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
    findBinBlobs(whiteBlobs, blackBlobs, binBlobs);
    
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
            newBins.push_back(processBin(binBlob, m_runSymbolDetector,
                                         binNumber, out));
            binNumber++;
        }

        // Sort through our new bins and match them to the old ones
        TrackedBlob::updateIds(&m_bins, &newBins, m_binSameThreshold);

        // Anybody left we didn't find this iteration, so its been dropped
        BOOST_FOREACH(Bin bin, m_bins)
        {
            BinEventPtr event(new BinEvent(bin.getX(), bin.getY(), 
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
                BinEventPtr event(new BinEvent(getX(), getY(), getSymbol(),
                                               getAngle()));
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
            BinEventPtr event(new BinEvent(bin.getX(), bin.getY(), 
                                           bin.getSymbol(), bin.getAngle()));
            event->id = bin.getId();
            publish(EventType::BIN_FOUND, event);
        }

    }
    else if (m_found)
    {
        // Publish lost event
        m_found = false;
        m_centered = false;
        publish(EventType::BINS_LOST, core::EventPtr(new core::Event()));
    }
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
    // NOTE: The property set automatically loads the value from the given
    //       config if its present, if not it uses the default value presented.
    core::PropertySetPtr propSet(getPropertySet());

    // General properties
    propSet->addProperty(config, false, "centeredLimit",
        "Maximum distance for the bin to be considred \"centered\"",
        0.1, &m_centeredLimit, 0.0, 4.0/3.0);
    propSet->addProperty(config, false, "logSymbolsImages",
        "Log all the images passed to the symbols detector to disk",
        false, &m_logSymbolImages);
    
    // Black mask properties
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
    propSet->addProperty(config, false, "binSameThreshold",
       "The max distance between bins on different frames",
        0.2, &m_binSameThreshold, 0.0, 4.0/3.0);
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
}

void BinDetector::allocateImages(int width, int height)
{
    m_percents = new OpenCVImage(width, height);
    m_whiteMaskedFrame = new OpenCVImage(width, height);
    m_blackMaskedFrame = new OpenCVImage(width, height);
    m_redMaskedFrame = new OpenCVImage(width, height);
    
    int extra = BIN_EXTRACT_BORDER * 2;
    size_t size = (width + extra) * (height + extra) * 3;
    m_extractBuffer = new unsigned char[size];
    m_scratchBuffer1 = new unsigned char[size];
    m_scratchBuffer2 = new unsigned char[size];
}

void BinDetector::deleteImages()
{
    delete m_percents;
    delete m_whiteMaskedFrame;
    delete m_blackMaskedFrame;
    delete m_redMaskedFrame;
    delete [] m_extractBuffer;
    delete [] m_scratchBuffer1;
    delete [] m_scratchBuffer2;
}
    
void BinDetector::filterForWhite(Image* input, Image* output)
{
    white_mask(m_percents->asIplImage(), input->asIplImage(),
               output->asIplImage(),
               m_whiteMaskMinimumPercent, m_whiteMaskMinimumIntensity);
}

void BinDetector::filterForBlack(Image* input, Image* output)
{
    black_mask(m_percents->asIplImage(), input->asIplImage(),
               output->asIplImage(),
               m_blackMaskMinimumPercent, m_blackMaskMaxTotalIntensity);
}

void BinDetector::filterForRed(Image* input, Image* output)
{
    int size = m_percents->getWidth() * m_percents->getHeight() * 3;
    unsigned char* percentData = m_percents->getData();
    unsigned char* inputData = input->getData();
    unsigned char* outputData = output->getData();

    for (int count = 0; count < size; count += 3)
    {
        if ((percentData[count+2] > m_redMinPercent) && // min R Percent
            (inputData[count+2] > m_redMinRValue) && // min R
            (inputData[count] < m_redMaxGValue) && // max G
            (inputData[count+1] < m_redMaxBValue)) // max B
        {
            outputData[count]=outputData[count+1]=outputData[count+2]=255;
        }
        else
        {
            outputData[count]=outputData[count+1]=outputData[count+2]=0;
        }
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
                               BlobDetector::BlobList& binBlobs)
{
    // NOTE: all blobs sorted largest to smallest
    BOOST_FOREACH(BlobDetector::Blob blackBlob, blackBlobs)
    {
        BOOST_FOREACH(BlobDetector::Blob whiteBlob, whiteBlobs)
        {
            // Sadly, this totally won't work at the edges of the screen...
            // crap damn.
            if (whiteBlob.containsInclusive(blackBlob, 2) &&
                blackBlob.getAspectRatio() < m_binMaxAspectRatio)
            {
                // blackBlobs[blackBlobIndex] is the black rectangle of a bin
                binBlobs.push_back(blackBlob);
                
                // Don't add it once for each white blob containing it,
                // thatd be dumb.
                break;
            }
            else
            {
                //Not a bin.
            }
        }
    }

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
    
    int upperLeftX = bin.getCenterX() - width/2 - BIN_EXTRACT_BORDER;
    int upperLeftY = bin.getCenterY() - height/2 - BIN_EXTRACT_BORDER;
    int lowerRightX = bin.getCenterX() + width/2 + BIN_EXTRACT_BORDER;
    int lowerRightY = bin.getCenterY() + height/2 + BIN_EXTRACT_BORDER;

    // Make sure we are not outside the image
    upperLeftX = std::max(0, upperLeftX);
    upperLeftY = std::max(0, upperLeftY);
    lowerRightX = std::min((int)m_blackMaskedFrame->getWidth() - 1,
                           lowerRightX);
    lowerRightY = std::min((int)m_blackMaskedFrame->getWidth() - 1,
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
                                          false);
        vision::Image::transform(redBinImage, rotatedBinImage, binAngle);
        delete redBinImage; // m_scratchBuffer2 free to use
        
        // Crop down Image to square around bin symbol
        Image* cropped = cropBinImage(rotatedBinImage, m_scratchBuffer2);
        delete rotatedBinImage; // m_scratchBuffer1 free to use
        if (cropped)
        {
            symbol = determineSymbol(cropped, m_scratchBuffer1, output);

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
                logSymbolImage(cropped, symbol);
            
            delete cropped;// m_scratchBuffer2 free to use
        }
    }
    
    // Report our results
    return Bin(bin, m_percents, binAngle, m_binID++, symbol);
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
        
        if (output)
        {
            line[0].x += bin.getCenterX() - input->getWidth() / 2;
            line[0].y += bin.getCenterY() - input->getHeight() / 2;
            line[1].x += bin.getCenterX() - input->getWidth() / 2;
            line[1].y += bin.getCenterY() - input->getHeight() / 2;
            
            cvLine(output->asIplImage(), line[0], line[1], CV_RGB(255,255,0),
                   5, CV_AA, 0);
        }
//        printf("Line dimensions: %f, %f\n", lineX, lineY);

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
    onlyRedSymbolRows = onlyRedSymbolCols = (onlyRedSymbolRows > onlyRedSymbolCols ? onlyRedSymbolRows : onlyRedSymbolCols);


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
    // Get a proper size image for dans detector
    Image* image = Image::loadFromBuffer(scratchBuffer, 128, 128, false);
    cvResize(input->asIplImage(), image->asIplImage(), CV_INTER_LINEAR);
    
    m_symbolDetector.processImage(image ,output);
    delete image;
    
    // Filter symbol type
    Symbol::SymbolType symbolFound = m_symbolDetector.getSymbol(); 
    Symbol::SymbolType symbol = Symbol::NONEFOUND;

    if (symbolFound == Symbol::CLUB || symbolFound == Symbol::CLUBR90 || symbolFound == Symbol::CLUBR180 || symbolFound == Symbol::CLUBR270)
    {
        //seeClub = true;
        symbol = Symbol::CLUB;
//        std::cout<<"Found Club Bin"<<std::endl;
        
    }
    else if (symbolFound == Symbol::SPADE || symbolFound == Symbol::SPADER90 || symbolFound == Symbol::SPADER180 || symbolFound == Symbol::SPADER270)
    {
        //seeSpade = true;
        symbol = Symbol::SPADE;
//        std::cout<<"Found Spade Bin"<<std::endl;
        
    }
    else if (symbolFound == Symbol::HEART || symbolFound == Symbol::HEARTR90 || symbolFound == Symbol::HEARTR180 || symbolFound == Symbol::HEARTR270)
    {
        //seeHeart = true;
        symbol = Symbol::HEART;
//        std::cout<<"Found Heart Bin"<<std::endl;
    }
    else if (symbolFound == Symbol::DIAMOND || symbolFound == Symbol::DIAMONDR90 || symbolFound == Symbol::DIAMONDR180 || symbolFound == Symbol::DIAMONDR270)
    {
        //seeDiamond = true;
        symbol = Symbol::DIAMOND;
//        std::cout<<"Found Diamond Bin"<<std::endl;
    }
    else if (symbolFound == Symbol::UNKNOWN)
    {
        symbol = Symbol::UNKNOWN;
//        std::cout<<"Found an unknown bin, rotate above it until we figure out what it is!"<<std::endl;
    }
    else if (symbolFound == Symbol::NONEFOUND)
    {
        //seeEmpty = true;
        symbol = Symbol::NONEFOUND;
//        std::cout<<"Found empty Bin"<<std::endl;
    }
    return symbol;
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

} // namespace vision
} // namespace ram
