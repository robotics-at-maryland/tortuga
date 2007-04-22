#include <iostream>
#include <string>

// wxWidgets includes
#include <wx/event.h>
#include <wx/string.h>
#include <wx/validate.h>
#include <wx/stattext.h>

// Ogre includes
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreRenderSystem.h>
#include <OGRE/OgreStringConverter.h>

// Project includes
#include "wxOgre.h"

// Platform specific includes
#ifdef __WXGTK__
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#endif

// For the wxWidget RTTI system
IMPLEMENT_DYNAMIC_CLASS(wxOgre, wxControl)

// Sets up callback function for wxWidgets events
BEGIN_EVENT_TABLE(wxOgre, wxControl)
	EVT_SIZE(wxOgre::onSize)
	// EVT_PAINT(wxOgre::OnPaint) // Produces flickers and runs too fast!
	EVT_ERASE_BACKGROUND( wxOgre::onEraseBackground )
END_EVENT_TABLE()

wxOgre::wxOgre() :
    wxControl(),
    mRenderWindow(0),
    mViewport(0),
    mCamera(0),
    mInitialized(false)
{
}

wxOgre::wxOgre(Ogre::Camera* camera, wxWindow* parent, wxWindowID id,
               const wxPoint& pos, const wxSize& size, long style,
               const wxString& name) :
    wxControl(),
    mRenderWindow(0),
    mViewport(0),
    mCamera(0),
    mInitialized(false)
{
    Create(camera, parent, id, pos, size, style, name);
}

bool wxOgre::Create(Ogre::Camera* camera, wxWindow* parent, wxWindowID id,
                    const wxPoint& pos, const wxSize& size, long style,
                    const wxString& name)
{
    mCamera = camera;

    // Create wxWidgets superclass
    wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name);

    return true;
}

wxOgre::~wxOgre()
{
	// Get rid of all view ports and destroy the render window
    mRenderWindow->removeAllViewports();
	Ogre::Root::getSingleton().detachRenderTarget(mRenderWindow);
	mRenderWindow = 0;
}

void wxOgre::initOgre()
{
    if (!mInitialized)
    {
        mInitialized = true;

        // Create all Ogre objects
        createOgreRenderWindow();
        // Update the render window just once
        update();
    }
}

Ogre::Camera* wxOgre::getCamera(void)
{
    return mViewport->getCamera();
}

void wxOgre::setCamera(Ogre::Camera* camera)
{
    mCamera = camera;

    if (mViewport)
    {
        mViewport->setCamera(camera);
    }
    else if(mInitialized)
    {
        mViewport = mRenderWindow->addViewport(mCamera);
        mViewport->setBackgroundColour(Ogre::ColourValue(1.0f, 0.0f, 0.0f,
                                                         1.0f));
    }
}

Ogre::RenderWindow* wxOgre::getRenderWindow()
{
    return mRenderWindow;
}

bool wxOgre::initialized()
{
    return mInitialized;
}

void wxOgre::createOgreRenderWindow()
{
	// Grab the current render system from Ogre
	Ogre::RenderSystem* renderSystem =
        Ogre::Root::getSingleton().getRenderSystem();

	// Create a new parameters list according to compiled OS
	Ogre::NameValuePairList params;
    getWindowParams(&params);

	// Get wx control window size
	int width;
	int height;
	GetClientSize(&width, &height);
	std::string name(GetName().mb_str(wxConvUTF8));

	// Create the render window (give the name of wxWidget window to Ogre)
	mRenderWindow =
        renderSystem->createRenderWindow(name, width, height, false, &params);

	// Set the viewport up with camera
	// NOTE: You can only create a camera after you have made the first camera
	//       we are going to need to be passed a camera for the second one
	if (mCamera)
	{
        mViewport = mRenderWindow->addViewport(mCamera);
        mViewport->setBackgroundColour(Ogre::ColourValue(1.0f, 0.0f, 0.0f,
                                                         1.0f));
	}
}


void wxOgre::getWindowParams(Ogre::NameValuePairList* params)
{
    Ogre::String handle;
#ifdef __WXMSW__
    mContainer = this;
	handle = Ogre::StringConverter::toString((size_t)((HWND)GetHandle()));
	(*params)["externalWindowHandle"] = handle;

#elif defined(__WXGTK__)
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    std::stringstream handleStream;
    GtkWidget* widget =  GetHandle();

    // Display
    Display* display = GDK_WINDOW_XDISPLAY( widget->window );
    handleStream << (unsigned long)display << ':';

    // Screen (returns ":display.screen")
    /* The new Ogre GLX renderer patch removes the need for supplying the
       screen

    std::string screenStr = DisplayString(display);
    int dotpos = screenStr.find(".", 0);
    screenStr = screenStr.substr(dotpos + 1, screenStr.length() - dotpos);
    handleStream << screenStr << ':';*/

    // XID (typedef of an unsigned int)
    Window wid = GDK_WINDOW_XWINDOW( widget->window );
    handleStream << wid;

    handle = handleStream.str();
    (*params)["parentWindowHandle"] = handle;
#else
	#error External/Parent Window handle not supported on this platform
#endif
}

void wxOgre::onSize(wxSizeEvent& event)
{
    if (mInitialized)
    {
        // Setting new size;
        int width;
        int height;

        GetClientSize(&width, &height);
        mRenderWindow->resize( width, height );
        // Letting Ogre know the window has been resized;
        mRenderWindow->windowMovedOrResized();

        // Set the aspect ratio for the new size;
        if (mCamera)
            mCamera->setAspectRatio(Ogre::Real(width) / Ogre::Real(height));

        mViewport->_updateDimensions();
        update();
    }
}

void wxOgre::onPaint(wxPaintEvent& event)
{
//	update(); // Produces flickers and runs too fast!
}
void wxOgre::onEraseBackground( wxEraseEvent& )
{
	update();
}

void wxOgre::update()
{
    if (mInitialized)
    {
        // ****************************************************
        // TODO: REMOVE THESE LINES! These are merely for test!
        static float redTone = 0;
        redTone += 0.01;
        if(redTone>1.0)
            redTone=0;
        // ****************************************************

        if(mViewport)
            mViewport->setBackgroundColour(Ogre::ColourValue(redTone, 0.0f, 0.0f, 1.0f));
        mRenderWindow->update();
        std::cout << "Updated" << std::endl;
    }
}
