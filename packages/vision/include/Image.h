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
#include "cv.h"
#include "highgui.h"

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
        PF_START = 0, /** Sentinal Value */
        PF_RGB_8,     /** Red Green Blue, 8 bits per channel */
        PF_BGR_8,     /** Blue Green Red, 8 bits*/
        PF_YUV444_8,  /** YUV, 8 bits for each channel */
        PF_GRAY_8,    /** Grayscale, 8 bits one channel */
        PF_HSV_8,     /** HSV, 8 bits per channel */
        PF_LUV_8,     /** CIELUV, 8 bits per channel */
        PF_LCHUV_8,   /** CIELCH_UV, 8 bits per channel */
        PF_LAB_8,     /** CIELAB, 8 bits per channel */
        PF_LCHAB_8,   /** CIELCH_AB, 8 bits per channel */
        PF_END,       /** Sentinal Value */
    };

    static const size_t formatDepthLookup[11];
    static const size_t formatChannelsLookup[11];

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

    /** Gives you an image that is a sub of the current one
     *
     *  @param source
     *      The image to do the extract from
     *  @param border
     *      The border around the blob you want
     *  @param centerX
     *      X coordinate of center of rectangle to extract
     *  @param centerY
     *      Y coordinate of center of rectangle to extract
     *  @param width
     *      Width of the sub image. For even width, the 'center' will be
     *      the left pixel of the middle two.
     *  @param height
     *      Height of the sub image. For even height, the 'center' will be
     *      the upper pixel of the middle two.
     *
     *  @return
     *      An image class using (but not owning) your buffer, you must delete
     *      the image.
     */
	static Image* extractSubImage(Image* source,
                                  int centerX, int centerY,
                                  int width, int height,
                                  unsigned char* buffer);


    /** Count white pixels in the sub part of the given image */
    static int countWhitePixels(Image* source,
				int upperLeftX, int upperLeftY,
				int lowerRightX, int lowerRightY);

    
    /** Blits the images using the clear color to the destination image */
    static void blitImage(Image* toBlit, Image* src, Image* dest,
                          unsigned char R = 255,
                          unsigned char G = 255,
                          unsigned char B = 255,
                          int xOffset = 0, int yOffset = 0);

    static void fillMask(Image* src, Image* mask,
                         unsigned char R = 255,
                         unsigned char G = 255,
                         unsigned char B = 255);

    /** Gives you an image that is a sub of the current one
     *
     *  @param source
     *      The image to do the extract from
     *  @param upperLeftX
     *      Upper left X coordinate of square to examine
     *  @param upperLeftY
     *      Upper left Y coordinate of square to examine
     *  @param lowerRightX
     *      Lower right X coordinate of square to examine
     *  @param lowerRightY
     *      Lower right Y coordinate of square to examine
     *  @param channel1
     *      Will be set to the average value of the first channel in the image
     *  @param channel2
     *      Will be set to the average value of the second channel in the image
     *  @param channel33
     *      Will be set to the average value of the third channel in the image
     *
     */
    static void getAveragePixelValues(Image* source,
                                      int upperLeftX, int upperLeftY,
                                      int lowerRightX, int lowerRightY,
                                      double& channel1, double& channel2,
                                      double& channel3);
    
    /** To write must be smaller then source */
    static void drawImage(Image* toWrite, int x, int y, Image* src,
                          Image* dest);

    /** Writs the desried text at the given X, Y cordinates */
    static void writeText(Image* image, std::string, int x, int y,
                          int height = 10);
    
    /** Creates and image given the data buffer (PF_BGR_8 is assumed) */
    static Image* loadFromBuffer(unsigned char* buffer, int width,
                                 int height, bool ownership,
                                 PixelFormat fmt = PF_START);
    
    /** Returns true if both images are the same size */
    static bool sameSize(Image* imageA, Image* imageB);

    /** Get the depth associated with given pixel format */
    static size_t getFormatDepth(PixelFormat fmt);

    /** Get the number of channels for the given pixel format */
    static size_t getFormatNumChannels(PixelFormat fmt);

    /** Makes the current image an exact copy of the source */
    virtual void copyFrom (const Image* src) = 0;
    
    /** The raw image data */
    virtual unsigned char* getData() const = 0;

    /** Width of image in pixels */
    virtual size_t getWidth() const = 0;

    /** Height of image in pixels */
    virtual size_t getHeight() const = 0;

    /** Depth of the image */
    virtual size_t getDepth() const = 0;

    /** Number of channels in the image */
    virtual size_t getNumChannels() const = 0;

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

    /** Provided for OpenCV Compatibiltiy */
    virtual IplImage* asIplImage() const = 0;

};

} // namespace vision
} // namespace ram


#endif // RAM_VISION_IMAGE_H_05_29_2007
