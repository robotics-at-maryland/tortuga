

// Project Includes
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
    free(data);
}

void LineDetector::init(core::ConfigNode config)
{

}

void LineDetector::processImage(Image* input, Image* output)
{
}

void LineDetector::edgeFilter(IplImage* img)
{

}

int LineDetector::houghTransform()
{
    return 0;
}

void LineDetector::ensureDataSize(int pixels)
{

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
