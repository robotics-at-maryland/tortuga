/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/src/CameraMakers.cpp
 */

// All registration are done in one file to keep the compilation dependency on 
// the factory system to a minimum

// STD Includes
#include <sstream>

// Library Includes
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

// Project Includes
#include "vision/include/CameraMaker.h"

#include "vision/include/ImageCamera.h"
#include "vision/include/NetworkCamera.h"
#include "vision/include/FFMPEGNetworkCamera.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/FFMPEGCamera.h"
#include "vision/include/RawFileCamera.h"
#include "vision/include/DC1394Camera.h"

namespace bfs = boost::filesystem;

namespace ram {
namespace vision {

static const boost::regex CAMNUM("\\d+");
static const boost::regex HOSTNAME_PORT("([a-zA-Z0-9.-_]+):(\\d{1,5})");
static const boost::regex IMAGE("[^\\.]+.(jpg|png)");

// ------------------------------------------------------------------------- //
//                        I M A G E   C A M E R A                            //
// ------------------------------------------------------------------------- //
    
template <>
CameraPtr CameraMakerTemplate<ImageCamera>::makeObject(
    CameraMakerParamType params)
{
    std::string input(params.get<0>());
    std::stringstream ss;
    ss <<"'" << input << "' image file";
	
    vision::Image* img = vision::Image::loadFromFile(input);
    vision::ImageCamera* c = new vision::ImageCamera(
        img->getWidth(), img->getHeight(), 30);
    c->newImage(img);
    //delete img;
    params.get<2>() = ss.str();
    return CameraPtr(c);
}
    
static CameraMakerTemplate<ImageCamera>
registerImageCamera("ImageCamera");


// ------------------------------------------------------------------------- //
//               F F M P E G   N E T W O R K   C A M E R A                   //
// ------------------------------------------------------------------------- //
    
template <>
CameraPtr CameraMakerTemplate<FFMPEGNetworkCamera>::makeObject(
    CameraMakerParamType params)
{
    boost::smatch what;
    bool result = boost::regex_match(params.get<0>(), what, HOSTNAME_PORT);
    assert(result && "Invalid input string");

    // Extract hostname & port out of the string
    std::string hostname = what.str(1);
    boost::uint16_t port =
        boost::lexical_cast<boost::uint16_t>(what.str(2));

    // Form message string
    std::stringstream ss;
    ss << "Streaming images from host: \"" << hostname
       << "\" on port " << port;
    params.get<2>() = ss.str();

    // Return our new camear
    return CameraPtr(new vision::FFMPEGNetworkCamera(hostname, port));
}
    
static CameraMakerTemplate<FFMPEGNetworkCamera>
registerFFMPEGNetworkCamera("FFMPEGNetworkCamera");


// ------------------------------------------------------------------------- //
//                       D C 1 3 9 4   C A M E R A                           //
// ------------------------------------------------------------------------- //

template <>
CameraPtr CameraMakerTemplate<DC1394Camera>::makeObject(
    CameraMakerParamType params)
{
    std::string input(params.get<0>());
    boost::smatch what;
    
    bool result = boost::regex_match(input, what, CAMNUM);
    assert(result && "Invalid input for DC1394Camera");

    int camnum = boost::lexical_cast<int>(input);
    assert(camnum >= 100 && "Camera number invalid for DC1394Camera");

    size_t num = (size_t)camnum - 100;

    // Form message string
    std::stringstream ss;
    ss << "DC1394 Camera num:" << num;
    params.get<2>() = ss.str();
    
    return CameraPtr(new vision::DC1394Camera(params.get<1>(), num));
}
  
static CameraMakerTemplate<DC1394Camera>
registerDC1394Camera("DC1394Camera");


// ------------------------------------------------------------------------- //
//                       O P E N C V   C A M E R A                           //
// ------------------------------------------------------------------------- //
    
template <>
CameraPtr CameraMakerTemplate<OpenCVCamera>::makeObject(
    CameraMakerParamType params)
{
    std::string input(params.get<0>());
    boost::smatch what;
    std::stringstream ss;
    
    if (boost::regex_match(input, what, CAMNUM))
    {
        int camnum = boost::lexical_cast<int>(input);
        assert(camnum < 100 && "Camera number invalid for OpenCVCamera");

        // Form message string
        std::stringstream ss;
        ss << "OpenCV Camera num:" << camnum;
        params.get<2>() = ss.str();
    
        return CameraPtr(new vision::OpenCVCamera(camnum, true));
    }
    else
    {
        ss << "'" << input << "' video file";
        params.get<2>() = ss.str();
        return CameraPtr(new vision::OpenCVCamera(input));
    }
}

static CameraMakerTemplate<OpenCVCamera>
registerOpenCVCamera("OpenCVCamera");


// ------------------------------------------------------------------------- //
//                      R A W   F I L E   C A M E R A                        //
// ------------------------------------------------------------------------- //
    
template <>
CameraPtr CameraMakerTemplate<RawFileCamera>::makeObject(
    CameraMakerParamType params)
{
    return CameraPtr(new vision::RawFileCamera(params.get<0>()));
}

static CameraMakerTemplate<RawFileCamera>
registerRawFileCamera("RawFileCamera");


// ------------------------------------------------------------------------- //
//                       F F M P E G   C A M E R A                           //
// ------------------------------------------------------------------------- //
    
template <>
CameraPtr CameraMakerTemplate<FFMPEGCamera>::makeObject(
    CameraMakerParamType params)
{
    return CameraPtr(new vision::FFMPEGCamera(params.get<0>()));
}

static CameraMakerTemplate<FFMPEGCamera>
registerFFMPEGCamera("FFMPEGCamera");

    
// ------------------------------------------------------------------------- //
//                       F F M P E G   C A M E R A                           //
// ------------------------------------------------------------------------- // 

std::string CameraKeyExtractor::extractKey(CameraMakerParamType& params)
{
    std::string input(params.get<0>());
    if (boost::regex_match(input, CAMNUM))
    {
        int camnum = boost::lexical_cast<int>(input);
        if (camnum >= 100)
        {
            return "DC1394Camera";
        }
        else
        {
            return "OpenCVCamera";
        }
    }

    boost::smatch what;
    if (boost::regex_match(input, what, HOSTNAME_PORT))
    {
        return "FFMPEGNetworkCamera";
    }
	
    if (boost::regex_match(input, IMAGE))
    {
        return "ImageCamera";
    }

    if (input.find("PLAYBACK:") != std::string::npos)
    {
        return "PlaybackCamera";
    }
    
    std::string extension = bfs::path(input).extension();

    if (".rmv" == extension)
        return "RawFileCamera";
    else
        return "FFMPEGCamera";
}
    
} // namespace vision
} // namespace ram
    
// In this case the static variable is safe
#if RAM_COMPILER == RAM_COMPILER_MSVC
#  pragma warning( disable : 4640 )
#endif

namespace ram {
namespace pattern {

template<>
vision::CameraMaker::MakerMap*
vision::CameraMaker::getRegistry()
{
    // This line is run only once, avoids static initialization order issue
    static vision::CameraMaker::MakerMap* reg =
        new vision::CameraMaker::MakerMap();
    return reg;
}

} // namespace pattern
} // namespace ram
