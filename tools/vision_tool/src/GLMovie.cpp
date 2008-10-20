//TODO: Maybe EVT_SIZE is keeping glGenTextures from working.

#include "GLMovie.h"
#include <wx/dcclient.h>
#include "vision/include/Camera.h"

#include <wx/msgdlg.h>

namespace ram {
  namespace tools {
    namespace visionvwr {

	static int ID_TIMER = 5;
	
      BEGIN_EVENT_TABLE(GLMovie, wxGLCanvas)
      EVT_PAINT(GLMovie::onPaint)
      EVT_SIZE(GLMovie::onSize)
      EVT_TIMER(ID_TIMER, GLMovie::onTimer)
      END_EVENT_TABLE()
      
      void GLMovie::onPaint(wxPaintEvent &event)
      {
	//When this widget is asked to paint, it should perform the OpenGL rendering.
	this->render();
      }
      GLMovie::GLMovie(wxWindow *parent) : wxGLCanvas(parent, -1, wxDefaultPosition)
      {
	this->oldWidth=0;
	this->oldHeight=0;
	this->imageData=NULL;
	this->m_camera=NULL;
	this->initialized=false;
	this->m_timer=new wxTimer(this, ID_TIMER);
      }
      void GLMovie::setCamera(vision::Camera *camera)
      {
	this->m_camera = camera;
      }
      void GLMovie::initGL()
      {
	//Seems to occur in all samples.
	SetCurrent();
	
	//Enable texturing.
	glEnable(GL_TEXTURE_2D);

	//Create space for the movie texture.
	glGenTextures(1, &this->textureid);
      }
      GLMovie::~GLMovie()
      {
	delete[] this->imageData;
	//Free the allocated texture.
	glDeleteTextures(1, &this->textureid);
      }
      void GLMovie::render()
      {
	if (!initialized)
	  {
	    //Initiate OpenGL.
	    this->initGL();
	    this->initialized=true;
	  }
	//Clear the matricies.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	int myWidth, myHeight;

	GetClientSize(&myWidth, &myHeight);

	//Set up the matrices.
	glOrtho(0.0f, (float)myWidth, 0.0f, (float)myHeight, -1.0f, 1.0f);
	glViewport(0, 0, myWidth, myHeight);

	//Samples seem to prefer Update to Refresh.  Update also seems to correctly resize the OpenGL drawing area.  When using Refresh, the glcanvas is always one size behind.
	Update();
	//Seems to occur in all samples.
	SetCurrent();
	//Seems to occur in all samples.
	wxPaintDC dc(this);
	//Make the background color black.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//Clear any old data.
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, this->textureid);

	//Find out where to draw the quad.
	int left, right, top, bottom;
	if (oldWidth==0 || oldHeight==0)
	{
		left=right=top=bottom=0;
	}
	else if (myHeight-oldHeight<myWidth-oldWidth)
	{
		bottom=0;
		top=myHeight;
		int screenWidth=oldWidth*myHeight/oldHeight;
		left=(myWidth-screenWidth)/2;
		right=left+screenWidth;
	}
	else
	{
		left=0;
		right=myWidth;
		int screenHeight=myWidth*oldHeight/oldWidth;
		bottom=(myHeight-screenHeight)/2;
		top=bottom+screenHeight;
	}

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(left, bottom);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(left, top);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(right, top);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(right, bottom);
	glEnd();
	
	//Finish drawing.  Send all drawing commands now.
	glFlush();
	
	//Show the results.
	SwapBuffers();
      }
      void GLMovie::onSize(wxSizeEvent &event)
      {
	//Clear the matricies.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//Set up the matrices.
	glOrtho(0.0f, (float)event.GetSize().GetWidth(), 0.0f, (float)event.GetSize().GetHeight(), -1.0f, 1.0f);
	glViewport(0, 0, event.GetSize().GetWidth(), event.GetSize().GetHeight());

	//Samples seem to prefer Update to Refresh.  Update also seems to correctly resize the OpenGL drawing area.  When using Refresh, the glcanvas is always one size behind.
	Update();
      }
      void GLMovie::nextFrame()
      {
	if (m_camera==NULL)
	  return;
	m_camera->update(0);
	m_camera->getImage(this);
      }


      void GLMovie::copyFrom (const Image* src)
      {
	// Handle self copy
	if (this == src)
	  return;

	int width = (int)src->getWidth();
	int height = (int)src->getHeight();
	    
	// Ensure bitmap is of the proper size
	if (width!=this->oldWidth && height!=this->oldHeight)
	  {
	    delete this->imageData;
	    this->imageData = new unsigned char[width*height*3];
	  }
	//Store the heights.
	this->oldWidth=width;
	this->oldHeight=height;
	    
	unsigned char* srcImageData = src->getData();
	unsigned char* currentImageData=this->imageData;
	size_t length = width * height;

	// Copy from source to the wxBitmap
	switch(src->getPixelFormat())
	  {
	  case vision::Image::PF_RGB_8:
	    {
	      for (size_t i = 0; i < length; ++i)
		{
		  currentImageData[0] = *srcImageData; 
		  currentImageData[1] = *(srcImageData + 1);
		  currentImageData[2] = *(srcImageData + 2);

		  srcImageData += 3;
		  currentImageData += 3;
		}            
	    }
	    break;

	  case vision::Image::PF_BGR_8:
	    {
	      for (size_t i = 0; i < length; ++i)
		{
		  currentImageData[2] = *srcImageData; 
		  currentImageData[1] = *(srcImageData + 1);
		  currentImageData[0] = *(srcImageData + 2);

		  srcImageData += 3;
		  currentImageData += 3;
		}
	    }
	    break;
		    
	  default:
	    {
	      assert("Unsupported pixel format");
	    }
	    break;
	  }

	//Do the texture.
	glBindTexture(GL_TEXTURE_2D, this->textureid);
	//TODO: Fix in case of BGR
	//Load the texture into memory.
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, this->oldWidth, this->oldHeight, GL_RGB, GL_UNSIGNED_BYTE, this->imageData);
	//Set parameters for the texture.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }

      unsigned char* GLMovie::getData() const
      {
	assert(false && "GLMovie::getData Not implemented");
      }
	    
      size_t GLMovie::getWidth() const
      {
	return this->oldWidth;
      }

      size_t GLMovie::getHeight() const
      {
	return this->oldHeight;
      }

      vision::Image::PixelFormat GLMovie::getPixelFormat() const
      {
	return vision::Image::PF_RGB_8;
      }

      const wxBitmap* GLMovie::getBitmap()
      {
	assert(false && "GLMovie::getBitmap Not implemented");
      }
	    
      bool GLMovie::getOwnership() const
      {
	assert(false && "GLMovie::getOwnership Not implemented");
      }
	    
      unsigned char* GLMovie::setData(unsigned char* data, bool ownership)
      {
	assert(false && "GLMovie::setData Not implemented");
      }

      void  GLMovie::setSize(int width, int height)
      {
	assert(false && "GLMovie::setSize Not implemented");
      }

      void  GLMovie::setPixelFormat(Image::PixelFormat format)
      {
	assert(false && "GLMovie::setPixelFormat Not implemented");
      }

      GLMovie::operator IplImage* ()
      {
	assert(false && "GLMovie::operator IplImage*Not implemented");
      }

      IplImage* GLMovie::asIplImage() const
      {
	assert(false && "GLMovie::asIplImage Not implemented");
      }

	void GLMovie::onTimer(wxTimerEvent &event)
	{
		this->nextFrame();
		Refresh();
	}

    }
  }
}
