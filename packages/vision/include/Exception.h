/*
 * Copyright (C) 2010 Robotics at Maryland
 * Copyright (C) 2010 Jonathan Sternberg
 * All rights reserved.
 *
 * Author: Jonathan Sternberg <jsternbe@umd.edu>
 * File:  packages/vision/include/Exception.h
 */

#ifndef RAM_VISION_EXCEPTION_04_04_2010
#define RAM_VISION_EXCEPTION_04_04_2010

// STD Includes
#include <string>
#include <sstream>
#include <exception>

namespace ram {
namespace vision {

class ImageConversionException : public std::exception
{
public:
    ImageConversionException(int startCode, int endCode)
    {
        std::stringstream sstream;
        sstream << "Invalid conversion from code " << startCode
                << " to " << endCode;
        m_msg = sstream.str();
    }

    virtual ~ImageConversionException() throw ()
    {
    }

    virtual const char* what() const throw()
    {
        return m_msg.c_str();
    }

private:
    std::string m_msg;
};

} // namespace vision
} // namespace ram

#endif // RAM_VISION_EXCEPTION_04_04_2010
