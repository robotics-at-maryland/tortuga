/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/vision/src/Image.cpp
 */


// Library Includes
#include <boost/python.hpp>

// Project Includes
#include "vision/include/Image.h"

namespace bp = boost::python;

class ImageWrapper : public ram::vision::Image,
                     public bp::wrapper< ram::vision::Image >
{
public:
    ImageWrapper() :
        ram::vision::Image(),
        bp::wrapper< ram::vision::Image >()
    {
    }
    
    virtual void copyFrom (const Image* src)
    {
        assert(false && "Not Implemented");
    };
    
    /** The raw image data */
    virtual unsigned char* getData() const
    {
        assert(false && "Not Implemented");
        return 0;
    };

    /** Width of image in pixels */
    virtual size_t getWidth() const
    {
        assert(false && "Not Implemented");
        return 0;
    };

    /** Height of image in pixels */
    virtual size_t getHeight() const
    {
        assert(false && "Not Implemented");
        return 0;
    };

    /** Depth of the image */
    virtual size_t getDepth() const
    {
        assert(false && "Not Implemented");
        return 0;
    }

    /** Number of channels in the image */
    virtual size_t getNumChannels() const
    {
        assert(false && "Not Implemented");
        return 0;
    }

    virtual Image::PixelFormat getPixelFormat() const
    {
        assert(false && "Not Implemented");
        return PF_START;
    };
    

    virtual bool getOwnership() const
    {
        assert(false && "Not Implemented");
		return false;
    };
    
    virtual unsigned char* setData(unsigned char*, bool)
    {
        assert(false && "Not Implemented");
        return 0;
    };

    virtual void setSize(int, int)
    {
        assert(false && "Not Implemented");
    };
  
    virtual void setPixelFormat(Image::PixelFormat)
    {
        assert(false && "Not Implemented");
    };

    virtual operator IplImage*()
    {
        assert(false && "Not Implemented");
        return 0;
    };

    virtual IplImage* asIplImage() const
    {
        assert(false && "Not Implemented");
        return 0;
    };
};

/** Wraps to allow usage from python */
ram::vision::Image* loadFromBuffer(unsigned long address, int width, int height,
                                   bool ownership,
                                   ram::vision::Image::PixelFormat fmt =
                                   ram::vision::Image::PF_START)
{
//    unsigned long addr = bp::extract<unsigned long>(address);
    return ram::vision::Image::loadFromBuffer((unsigned char*)address, width,
                                              height, ownership, fmt);
}

void registerImageClass()
{
    typedef bp::class_< ImageWrapper, boost::noncopyable > ImageExposer;
    ImageExposer imageExposer("Image");
    bp::scope imageScope(imageExposer);

    bp::enum_< ram::vision::Image::PixelFormat>("PixelFormat")
        .value("PF_START", ram::vision::Image::PF_START)
        .value("PF_RGB_8", ram::vision::Image::PF_RGB_8)
        .value("PF_BGR_8", ram::vision::Image::PF_BGR_8)
        .value("PF_YUV444_8", ram::vision::Image::PF_YUV444_8)
        .value("PF_GRAY_8", ram::vision::Image::PF_GRAY_8)
        .value("PF_HSV_8", ram::vision::Image::PF_HSV_8)
        .value("PF_LUV_8", ram::vision::Image::PF_LUV_8)
        .value("PF_LCHUV_8", ram::vision::Image::PF_LCHUV_8)
        .value("PF_LAB_8", ram::vision::Image::PF_LAB_8)
        .value("PF_LCHAB_8", ram::vision::Image::PF_LCHAB_8)
        .value("PF_END", ram::vision::Image::PF_END)
        .export_values();

    imageExposer
        .def("loadFromFile", &ram::vision::Image::loadFromFile,
             bp::return_value_policy<bp::manage_new_object>())
        .staticmethod("loadFromFile")
        .def("saveToFile", &ram::vision::Image::saveToFile,
             bp::return_value_policy<bp::manage_new_object>())
        .staticmethod("saveToFile")
        .def("loadFromBuffer", &::loadFromBuffer,
             bp::return_value_policy<bp::manage_new_object>())
        .staticmethod("loadFromBuffer");
}
