/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/vision/include/Image.h
 */

#ifndef RAM_VISION_IMAGE_H_05_29_2007
#define RAM_VISION_IMAGE_H_05_29_2007

// STD Includes
#include <cstddef>
#include <string>

// Library Includes
#include <boost/utility.hpp>

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Export.h"

#include "math/include/Math.h"
#include "math/include/Vector2.h"

namespace ram {
namespace vision {

/** Simple Abstract Image Class.
 *
 * This class provides a uniform interface to images comming from a variety of
 * places.
 */
class RAM_EXPORT Image : public boost::noncopyable
{
public:
    virtual ~Image() {};
    
    /** Describes the pixels format of our images */
    enum PixelFormat
    {
        PF_START, /** Sentinal Value */
        PF_RGB_8, /** Red Green Blue, 8 bits per channel */
        PF_BGR_8, /** Blue Green Red, 8 bits*/
        PF_YUV444_8, /** YUV, 8 bits for each channel */
        PF_END,   /** Sentinal Value */
    };

    /* Open image from given file, returns 0 on error */
    static Image* loadFromFile(std::string fileName);

    /** Saves the given image to the desired file */
    static void saveToFile(Image* image, std::string fileName);

    /** Shows the image on screen (THIS BLOCKS UNTIL CLOSED w/ESC Key) */
    static void showImage(Image* image, std::string name = "");

    /** Rotates, scales, and translates a given image
     *
     *  @param src
     *      The image to be transformed.
     *  @param dest
     *      The image where the result of the transformation goes.
     *  @param rotation
     *      How much to rotate the image.
     *  @param scaleFactor
     *      Changes image size, > 1 makes the image bigger.
     *  @param xTrans
     *      How many pixels to translate the images in the x dimension
     *  @param yTrans
     *      How many pixels to translate the images in the y dimension
     *  @param fillR
     *      The R component of the color to fill in around the rotate image.
     *  @param fillG
     *      The G component of the color to fill in around the rotate image.
     *  @param fillB
     *      The B component of the color to fill in around the rotate image.
     */
    static void transform(Image* src, Image* dest, math::Degree rotation,
                          double scaleFactor = 1.0, int xTrans = 0,
                          int yTrans = 0,
                          unsigned char fillR = 0,
                          unsigned char fillG = 0,
                          unsigned char fillB = 0);

    /** Gives you an image that is a sub of the current one
     *
     *  @param source
     *      The image to do the extract from
     *  @param border
     *      The border around the blob you want
     *  @param upperLeftX
     *      Upper left X coordinate of square to extract
     *  @param upperLeftY
     *      Upper left Y coordinate of square to extract
     *  @param lowerRightX
     *      Lower right X coordinate of square to extract
     *  @param lowerRightY
     *      Lower right Y coordinate of square to extract
     *
     *  @return
     *      An image class using (but not owning) your buffer, you must delete
     *      the image.
     */
    static Image* extractSubImage(Image* source, unsigned char* buffer,
                                  int upperLeftX, int upperLeftY,
                                  int lowerRightX, int lowerRightY);

    
    /** Blits the images using the clear color to the destination image */
    static void blitImage(Image* toBlit, Image* src, Image* dest,
                          unsigned char R = 255,
                          unsigned char G = 255,
                          unsigned char B = 255,
                          int xOffset = 0, int yOffset = 0);

    /** To write must be smaller then source */
    static void drawImage(Image* toWrite, int x, int y, Image* src,
                          Image* dest);

    /** Writs the desried text at the given X, Y cordinates */
    static void writeText(Image* image, std::string, int x, int y,
                          int height = 10);
    
    /** Creates and image given the data buffer (PF_BGR_8 is assumed) */
    static Image* loadFromBuffer(unsigned char* buffer, int width,
                                 int height, bool ownership);
    
    /** Makes the current image an exact copy of the source */
    virtual void copyFrom (const Image* src) = 0;
    
    /** The raw image data */
    virtual unsigned char* getData() const = 0;

    /** Width of image in pixels */
    virtual size_t getWidth() const = 0;

    /** Height of image in pixels */
    virtual size_t getHeight() const = 0;

    /** Pixel format of the image */
    virtual Image::PixelFormat getPixelFormat() const = 0;

    /** Determines whether or not to release the image buffer */
    virtual bool getOwnership() const = 0;
    
    /** Change Image data, old data returned if not owned
     *  
     *  
     */
    virtual unsigned char* setData(unsigned char* data,
                                   bool ownership = true) = 0;

    /** Set width and height of image in pixels */
    virtual void setSize(int width, int height) = 0;
  
    /** Set pixel format of the image, will crash if conversion is impossible*/
    virtual void setPixelFormat(Image::PixelFormat format) = 0;

    /** All images should be castable to IplImage for OpenCV compatibility */
    virtual operator IplImage*() = 0;

    /** Provieded for OpenCV Compatibiltiy */
    virtual IplImage* asIplImage() const = 0;

};

} // namespace vision
} // namespace ram


#endif // RAM_VISION_IMAGE_H_05_29_2007
