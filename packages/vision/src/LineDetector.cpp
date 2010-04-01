

// Project Includes
#include "core/include/PropertySet.h"
#include "vision/include/Image.h"
#include "vision/include/LineDetector.h"

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
                         .15, &m_highThreshold, 0.0, 1.0);

    propSet->addProperty(config, false, "lowThreshold",
                         "Low threshold for canny edge detetor",
                         .4*.15, &m_lowThreshold, 0.0, 1.0);

    propSet->addProperty(config, false, "maxLines",
                         "Maximum number of lines to find",
                         10, &m_maxLines);

    propSet->addProperty(config, false, "minLineSize",
                         "Minimum size of a line",
                         0, &m_minLineSize);

    propSet->verifyConfig(config, true);
}

void LineDetector::processImage(Image* input, Image* output)
{
    // Only support single channel image for now
    IplImage* image = input->asIplImage();
    assert(image->nChannels == 1 && "Only supports single channel images");
    //ensureDataSize(image->imageSize);
   
    cvCanny(image, data, m_highThreshold, m_lowThreshold);
    houghTransform();

    // Tag lines that are too short
    double minLength = m_minLineSize*m_minLineSize;
    std::vector<int> taggedLines;
    int i = 0;
    BOOST_FOREACH(LineDetector::Line line, m_lines)
    {
        if (line.squaredLength() < minLength) {
            taggedLines.push_back(i);
        }
        i += 1;
    }

    // Now remove them
    int offset = 0;
    BOOST_FOREACH(int iter, taggedLines)
    {
        m_lines.erase(m_lines.begin()+(iter - offset));
        offset += 1;
    }

    // Debug stuff
    if (0 != output)
    {
        output->copyFrom(input);

        CvPoint pt1, pt1BoundUR, pt1BoundLL;
        CvPoint pt2, pt2BoundUR, pt2BoundLL;
        int boxsize = 5;
        int linesize = 3;
        BOOST_FOREACH(LineDetector::Line line, m_lines)
        {
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
            cvLine(output->asIplImage(), pt1, pt2, CV_RGB(0,0,255), linesize);
        }
    }
}

int LineDetector::houghTransform()
{
    return 0;
}

void LineDetector::ensureDataSize(int pixels)
{
/**
    size_t bytes = (size_t)pixels * sizeof(*data);
    if (m_dataSize < (size_t)pixels)
    {
        m_dataSize = (int)pixels;
        if (data)
            data = (unsigned int*)realloc(data, bytes);
        else
            data = (unsigned int*)malloc(bytes);
    }
**/
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
