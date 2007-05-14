#ifndef OGREAPP_H_INCLUDE
#define OGREAPP_H_INCLUDE

// wxWidgets Includes
#include <wx/wx.h>
#include <wx/glcanvas.h>

class wxOgre;

class OgreApp : public wxApp
{
	virtual bool OnInit();
	virtual int OnExit();

    void setupOgre();

    void onRenderTimer(wxTimerEvent& event);

    wxTimer* mRenderTimer;

    DECLARE_EVENT_TABLE()
};

// The "main"
IMPLEMENT_APP(OgreApp)

class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
    /** Called when we can create underlying Ogre window*/
    void onActivate(wxActivateEvent& event);

    wxOgre* mWxOgre;
    //wxGLCanvas* glcanvas;

	DECLARE_EVENT_TABLE()
};

#endif // OGREAPP_H_INCLUDE
