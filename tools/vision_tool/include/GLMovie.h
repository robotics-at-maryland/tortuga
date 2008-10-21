#ifndef GL_MOVIE_H
#define GL_MOVIE_H

#include <wx/glcanvas.h>
#include <wx/timer.h>
#include "vision/include/Image.h"

namespace ram
{
namespace tools
{
namespace visionvwr
{

class GLMovie: public wxGLCanvas, vision::Image
{
		friend class MediaControlPanel;

	private:
		GLuint textureid;
		void applyTexture();
		void render();
		void onPaint(wxPaintEvent &event);
		void initGL();
		void onSize(wxSizeEvent &event);
		void onTimer(wxTimerEvent &event);
		/** The source of the new images */
		vision::Camera* m_camera;
		//The size of the movie (in pixels).
        int movieWidth, movieHeight;
        //The size of the image texture (in pixels).  The imageWidth and imageHeight should be the next power of 2 greater than movieWidth and movieHeight.
        int imageWidth, imageHeight;
		//Whether OpenGL has been initialized.  It seems wxGLCanvas hasn't actually initialized OpenGL in the constructor.
		bool initialized;

		wxTimer *m_timer;

        unsigned char *imageData;

        int nextPowerOf2(int a);
	protected:
	public:
		GLMovie(wxWindow *parent);
		~GLMovie();

		void setCamera(vision::Camera *camera);

		//!Prepares the next frame by loading it as a texture but does not necessarily render with the new frame.
		void nextFrame();

		virtual void copyFrom(const Image* src);

		//virtual unsigned char* getData() const;

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
};
}
}
}
#endif
