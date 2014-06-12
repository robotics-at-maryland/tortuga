#ifndef WXOGRE_H_INCLUDE
#define WXOGRE_H_INCLUDE

// wxWidgets Includes
#include <wx/control.h>
class wxSizeEvent;
class wxPaintEvent;
class wxEraseEvent;

// Ogre Includes
namespace Ogre
{
    class RenderWindow;
    class Viewport;
    class Camera;
}

class wxOgre : public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxOgre)
public:
    wxOgre();

    wxOgre(Ogre::Camera* camera, wxWindow* parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxString& name = wxPanelNameStr);

    /** Standard 2-Stage wxWidgets window creation */
    virtual bool Create(Ogre::Camera* camera, wxWindow* parent, wxWindowID id,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize, long style = 0,
                        const wxString& name = wxPanelNameStr);

    virtual ~wxOgre();

    /** Call to startup Ogre due to platform issues, these have to be seperate
    for Linux this <b>must</b> be called after you main frame reices its
    activate event.  I suggest you do so for all platforms for simplicity */
    void initOgre();

    /** Retrieves a pointer to the camera for the main viewport */
    Ogre::Camera* getCamera(void);

    /** Sets the camera to use for rendering to this viewport. If this is the
    first you window you are creating you have to set it after creating the
    window. */
    void setCamera(Ogre::Camera* camera);

    /** Grabs the current window */
    Ogre::RenderWindow* getRenderWindow();

    /** Retrieve whether or not the ogre part of this windows has been started*/
    bool initialized();

private:
    /** Callback function to a window resize event */
    void onSize(wxSizeEvent& event);

    /** Callback function to a window paint event */
    void onPaint(wxPaintEvent& event);

    /** Callback function to an EraseBackground event */
    void onEraseBackground(wxEraseEvent& event);


    /** Creates an ogre rendering window based on the current wxControl */
	void createOgreRenderWindow();

    /** This builds the window params in a platform depenent manner */
    void getWindowParams(Ogre::NameValuePairList* params);

    /** Redraws the window */
    void update();

    /** The Ogre Rending window displayed in the application */
    Ogre::RenderWindow* mRenderWindow;

    /** The first viewport of the window */
    Ogre::Viewport* mViewport;

    /** The Camera for the main viewport */
    Ogre::Camera* mCamera;

    /** Whether or not you have started up the Ogre part of this window*/
    bool mInitialized;


    /** Setups the wxWidgets event table */
    DECLARE_EVENT_TABLE()
};

#endif // WXOGRE_H_INCLUDE
