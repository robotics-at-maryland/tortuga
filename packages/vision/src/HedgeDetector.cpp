
// STD Includes
#include <vector>

// Library Includes
#include "highgui.h"
#include <boost/foreach.hpp>

// Project Includes
#include "vision/include/main.h"
#include "vision/include/Camera.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/HedgeDetector.h"
#include "vision/include/Events.h"
#include "vision/include/ColorFilter.h"

#include "core/include/PropertySet.h"

namespace ram {
namespace vision {

HedgeDetector::HedgeDetector(core::ConfigNode config,
                             core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0),
    m_filter(0)
{
    init(config);
}

HedgeDetector::HedgeDetector(Camera* camera)
{
    init(core::ConfigNode::fromString("{}"));
}

void HedgeDetector::init(core::ConfigNode config)
{
    core::PropertySetPtr propSet(getPropertySet());
    propSet->addProperty(config, false, "initialMinPixels",
                         "Green pixel count required for blob to be a hedge",
                         400, &m_initialMinGreenPixels);
    minGreenPixels = m_initialMinGreenPixels;

    propSet->addProperty(config, false, "topRemovePercentage",
        "% of the screen from the top to be blacked out",
        0.0, &m_topRemovePercentage);

    propSet->addProperty(config, false, "bottomRemovePercentage",
        "% of the screen from the bottom to be blacked out",
        0.0, &m_bottomRemovePercentage);

    propSet->addProperty(config, false, "leftRemovePercentage",
        "% of the screen from the left to be blacked out",
        0.0, &m_leftRemovePercentage);

    propSet->addProperty(config, false, "rightRemovePercentage",
        "% of the screen from the right to be blacked out",
        0.0, &m_rightRemovePercentage);

    propSet->addProperty(config, false, "useLUVFilter",
                         "Use LUV based color filter", false, &m_useLUVFilter);

    m_filter = new ColorFilter(0, 255, 0, 255, 0, 255);
    m_filter->addPropertiesToSet(propSet, &config,
                                 "L", "L*",
                                 "U", "Blue Chrominance",
                                 "V", "Red Chrominance",
                                 0, 255,    // L defaults // 180, 255
                                 0, 200,    // U defaults // 76, 245
                                 200, 255); // V defaults // 200,255

    // Make sure the configuration is valid
    propSet->verifyConfig(config, true);

    // State machine variables
    found=false;
    m_hedgeCenterX=0;
    m_hedgeCenterY=0;
    m_hedgeWidth=0;
    hedgeCenter.x=0;
    hedgeCenter.y=0;

    // Working images
    frame = new ram::vision::OpenCVImage(640, 480);
    image=cvCreateImage(cvSize(640,480), 8, 3);
    raw=cvCreateImage(cvGetSize(image), 8, 3);
    flashFrame=cvCreateImage(cvGetSize(image), 8, 3);
    saveFrame=cvCreateImage(cvSize(640,480), 8, 3);
}

HedgeDetector::~HedgeDetector()
{
    delete frame;
    cvReleaseImage(&flashFrame);
    cvReleaseImage(&image);
    cvReleaseImage(&raw);
    cvReleaseImage(&saveFrame);
}

double HedgeDetector::getX()
{
    return m_hedgeCenterX;
}

double HedgeDetector::getY()
{
    return m_hedgeCenterY;
}

void HedgeDetector::setTopRemovePercentage(double percent)
{
    m_topRemovePercentage = percent;
}

void HedgeDetector::setBottomRemovePercentage(double percent)
{
    m_bottomRemovePercentage = percent;
}

void HedgeDetector::setUseLUVFilter(bool value)
{
    m_useLUVFilter = value;
}

void HedgeDetector::show(char* window)
{
    cvShowImage(window,((IplImage*)(raw)));
}

IplImage* HedgeDetector::getAnalyzedImage()
{
    rotate90DegClockwise(raw, saveFrame);
    return (IplImage*)(saveFrame);
}

void HedgeDetector::update()
{
    cam->getImage(frame);
    processImage(frame, 0);
}

void HedgeDetector::processImage(Image* input, Image* output)
{
    // Resize images if needed
    if ((image->width != (int)input->getWidth()) &&
        (image->height != (int)input->getHeight()))
    {
        cvReleaseImage(&image);
        image = cvCreateImage(cvSize(input->getWidth(),
                                     input->getHeight()), 8, 3);
        cvReleaseImage(&raw);
        raw=cvCreateImage(cvGetSize(image),8,3);
        cvReleaseImage(&flashFrame);
        flashFrame=cvCreateImage(cvGetSize(image), 8, 3);
    }

    cvCopyImage(input->asIplImage(), image);
    cvCopyImage(image, flashFrame);
    
    // Remove top chunk if desired
    if (m_topRemovePercentage != 0)
    {
        int linesToRemove = (int)(m_topRemovePercentage * image->height);
        size_t bytesToBlack = linesToRemove * image->width * 3;
        memset(image->imageData, 0, bytesToBlack);
        memset(flashFrame->imageData, 0, bytesToBlack);
    }
    

    if (m_bottomRemovePercentage != 0)
    {
//        printf("Removing \n");
        int linesToRemove = (int)(m_bottomRemovePercentage * image->height);
        size_t bytesToBlack = linesToRemove * image->width * 3;
        memset(&(image->imageData[image->width * image->height * 3 - bytesToBlack]), 0, bytesToBlack);
        memset(&(flashFrame->imageData[flashFrame->width * flashFrame->height * 3 - bytesToBlack]), 0, bytesToBlack);
    }

    if (m_rightRemovePercentage != 0)
    {
        size_t lineSize = image->width * 3;
        size_t bytesToBlack = (int)(m_rightRemovePercentage * lineSize);;
        for (int i = 0; i < image->height; ++i)
        {
            size_t offset = i * lineSize - bytesToBlack;
            memset(image->imageData + offset, 0, bytesToBlack);
            memset(flashFrame->imageData + offset, 0, bytesToBlack);
        }
    }

    if (m_leftRemovePercentage != 0)
    {
        size_t lineSize = image->width * 3;
        size_t bytesToBlack = (int)(m_leftRemovePercentage * lineSize);;
        for (int i = 0; i < image->height; ++i)
        {
            size_t offset = i * lineSize;
            memset(image->imageData + offset, 0, bytesToBlack);
            memset(flashFrame->imageData + offset, 0, bytesToBlack);
        }
    }
        
    // Process Image
    CvPoint boundUR = {0};
    CvPoint boundLL = {0};
    boundUR.x = 0;
    boundUR.y = 0;
    boundLL.x = 0;
    boundLL.y = 0;

    if (m_useLUVFilter)
        filterForGreenLUV(flashFrame);
    else
        filterForGreen(image, flashFrame);

    m_blobDetector.setMinimumBlobSize(minGreenPixels);
    OpenCVImage temp(flashFrame, false);
    m_blobDetector.processImage(&temp);

    BlobDetector::BlobList blobs = m_blobDetector.getBlobs();
    int greenPixelCount = 0;

    if (blobs.size() > 0)
    {
        BlobDetector::Blob greenBlob(0, 0, 0, 0, 0, 0, 0);

        // Attempt to find a valid blob
        if (processBlobs(blobs, greenBlob))
        {
            hedgeCenter.x = greenBlob.getCenterX();
            hedgeCenter.y = greenBlob.getCenterY();
            boundUR.x = greenBlob.getMaxX();
            boundUR.y = greenBlob.getMaxY();
            boundLL.x = greenBlob.getMinX();
            boundLL.y = greenBlob.getMinY();
            greenPixelCount = greenBlob.getSize();

            found=true;
            Detector::imageToAICoordinates(input, hedgeCenter.x, hedgeCenter.y,
                                           m_hedgeCenterX, m_hedgeCenterY);

            publishFoundEvent();
        }
    } else {
        if (found)
            publish(EventType::HEDGE_LOST, core::EventPtr(new core::Event()));

        found=false;
    }


    if (output)
    {
        cvCopyImage(flashFrame, raw);
        
        if (found)
        {
            // Debugging info
            CvPoint tl,tr,bl,br;
            tl.x = bl.x = std::max(hedgeCenter.x-4,0);
            tr.x = br.x = std::min(hedgeCenter.x+4,raw->width-1);
            tl.y = tr.y = std::min(hedgeCenter.y+4,raw->height-1);
            br.y = bl.y = std::max(hedgeCenter.y-4,0);
                        
            cvLine(raw, tl, tr, CV_RGB(0,0,255), 3, CV_AA, 0 );
            cvLine(raw, tl, bl, CV_RGB(0,0,255), 3, CV_AA, 0 );
            cvLine(raw, tr, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
            cvLine(raw, bl, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
            
            cvRectangle(raw, boundUR, boundLL, CV_RGB(0,255,0), 2, CV_AA, 0);

            // clamp values
            //lightCenter.x = std::min(lightCenter.x, raw->width-1);
            //lightCenter.x = std::max(lightCenter.x, 0);
            //lightCenter.y = std::min(lightCenter.y, raw->height-1);
            //lightCenter.y = std::max(lightCenter.y, 0);
            //int radius = std::max((int)sqrt((double)redPixelCount/M_PI), 1);
         
            //cvCircle(raw, hedgeCenter, radius, CV_RGB(0,255,0), 2, CV_AA, 0);
        }

        OpenCVImage temp(raw, false);
        output->copyFrom(&temp);
    }
}

void HedgeDetector::filterForGreen(IplImage* image, IplImage* flashFrame)
{
}

void HedgeDetector::filterForGreenLUV(IplImage* image)
{
    cvCvtColor(image, image, CV_BGR2Luv);
    Image* tmpImage = new OpenCVImage(image, false);
    m_filter->filterImage(tmpImage);
    delete tmpImage;
}

void HedgeDetector::publishFoundEvent()
{
    if (found)
    {
        HedgeEventPtr event(new HedgeEvent(0, 0, 0));

        event->x = m_hedgeCenterX;
        event->y = m_hedgeCenterY;
        event->width = m_hedgeWidth;

        publish(EventType::HEDGE_FOUND, event);
    }
}

bool HedgeDetector::processBlobs(const BlobDetector::BlobList& blobs,
                                 BlobDetector::Blob& outBlob)
{
    // TODO: Get aspect ratio working
    // Copy from RedLightDetector

    if (blobs.size() > 0)
    {
        outBlob = blobs[0];
        return true;
    }

    return false;
}

} // namespace vision
} // namespace ram
