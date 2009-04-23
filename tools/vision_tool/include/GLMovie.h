/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 John Edmonds
 * All rights reserved.
 *
 * Author: John "Jack" Edmonds <pocketcookies2@gmail.com>
 * File:   tools/vision_viewer/include/GLMovie.h
 */

#ifndef RAM_TOOLS_VISION_TOOL_GLMOVIE_10_18_2008
#define RAM_TOOLS_VISION_TOOL_GLMOVIE_10_18_2008

// Library Includes
#include <wx/glcanvas.h>

// Project Includes
#include "vision/include/Image.h"
#include "core/include/Forward.h"

#include "Forward.h"

namespace ram {
namespace tools {
namespace visionvwr {

class GLMovie: public wxGLCanvas, vision::Image
{
public:
    GLMovie(wxWindow *parent, Model* model);
    ~GLMovie();
    
    // Image methods TODO: Integrate me a different way
    virtual void copyFrom(const Image* src);
    
    virtual size_t getWidth() const;

    virtual size_t getHeight() const;
    
    virtual vision::Image::PixelFormat getPixelFormat() const;
    
    const wxBitmap* getBitmap();
    
    // Not Implemented yet for the wxBitmap
    virtual unsigned char* getData() const;

    virtual bool getOwnership() const;
    
    virtual unsigned char* setData(unsigned char* data, bool ownership =
                                   true);
    
    virtual void setSize(int width, int height);
    
    virtual void setPixelFormat(vision::Image::PixelFormat format);
    
    virtual operator IplImage*();
    
    virtual IplImage* asIplImage() const;
    
    DECLARE_EVENT_TABLE()
    
private:
    void applyTexture();
    void render();
    void initGL();
    
    void onPaint(wxPaintEvent &event);
    void onSize(wxSizeEvent &event);
    void onNewImage(core::EventPtr event);
    
   
    GLuint textureid;
    
    /** The source of the new images */
    Model* m_model;
    
    //The size of the movie (in pixels).
    int movieWidth, movieHeight;
    //The size of the image texture (in pixels).  The imageWidth and imageHeight should be the next power of 2 greater than movieWidth and movieHeight.
    int imageWidth, imageHeight;
    //Whether OpenGL has been initialized.  It seems wxGLCanvas hasn't actually initialized OpenGL in the constructor.
    bool initialized;

    bool m_inRender;
    
    unsigned char *imageData;

    int nextPowerOf2(int a);
};
    
} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISION_TOOL_GLMOVIE_10_18_2008
