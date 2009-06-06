#include <iostream>

// Ogre Includes
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreResourceGroupManager.h>
#include <OGRE/OgreWindowEventUtilities.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreHardwareBufferManager.h>
#include <OGRE/OgreRenderSystem.h>

// Project Includes
#include "OgreApp.h"
#include "wxOgre.h"

// Required for the timer
const long ID_RENDERTIMER = wxNewId();

BEGIN_EVENT_TABLE(OgreApp, wxApp)
	EVT_TIMER( ID_RENDERTIMER, OgreApp::onRenderTimer )
END_EVENT_TABLE()

DECLARE_APP(OgreApp);

// OgreApp Methods
bool OgreApp::OnInit()
{
    setupOgre();

	MyFrame *frame = new MyFrame(_T("Hello World"), wxPoint(50,50),
                                 wxSize(450,350));

	frame->Show(TRUE);
	SetTopWindow(frame);

    mRenderTimer = new wxTimer(this, ID_RENDERTIMER);
    mRenderTimer->Start(50); // 20 FPS
	return TRUE;
}
#define STR(s) #s

void OgreApp::setupOgre()
{
    //new Ogre::HardwareBufferManager();

    Ogre::Root* root = new Ogre::Root;
    root->loadPlugin("/home/jlisee/projects/mrbc/trunk/deps/local/lib/OGRE/RenderSystem_GL");

    Ogre::RenderSystemList *rs = root->getAvailableRenderers();
    if(rs&&rs->size()&&rs->at(0)->getName().compare("RenderSystem_GL"))
    {
        Ogre::RenderSystem* r = rs->at(0);
        root->setRenderSystem(r);
    }
    else
    {
        exit(1);
    }

    Ogre::ResourceGroupManager& resources =
        Ogre::ResourceGroupManager::getSingleton();
    resources.addResourceLocation("data","FileSystem");
    resources.initialiseAllResourceGroups();


    root->initialise(false);
}

int OgreApp::OnExit()
{
    mRenderTimer->Stop();
    delete Ogre::Root::getSingletonPtr();
    return 0;
}

void OgreApp::onRenderTimer(wxTimerEvent& event)
{
    Ogre::WindowEventUtilities::messagePump();
    Ogre::Root::getSingleton().renderOneFrame();
}

// Basic Frame methods
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_ACTIVATE(MyFrame::onActivate )
END_EVENT_TABLE()

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame((wxFrame*)NULL,-1,title,pos,size)
{
    //Ogre::SceneManager* sceneMgr =
        Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC,
                                                      "ExampleSceneManager");

    //glcanvas = new wxGLCanvas(this);
    mWxOgre = new wxOgre(0, this, wxID_ANY, wxDefaultPosition,
                               wxDefaultSize);
}

void FBConfigToString (Display *dpy, GLXFBConfig config);

void MyFrame::onActivate(wxActivateEvent& event)
{

    //FBConfigToString(GDK_WINDOW_XDISPLAY( GetHandle()->window ),
    //                *glcanvas->m_fbc);

    if (!mWxOgre->initialized())
    {
        mWxOgre->initOgre();

        Ogre::SceneManager* sceneMgr =
            Ogre::Root::getSingleton().getSceneManager("ExampleSceneManager");

        Ogre::Camera* cam = sceneMgr->createCamera("SimpleCamera");
        cam->setPosition(Ogre::Vector3(0.0f,0.0f,500.0f));
        cam->lookAt(Ogre::Vector3(0.0f,0.0f,0.0f));
        cam->setNearClipDistance(5.0f);
        cam->setFarClipDistance(5000.0f);

        mWxOgre->setCamera(cam);
        sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(cam);
    }
}

void FBConfigToString (Display *dpy, GLXFBConfig config)
{
        int configID, visualID, bufferSize, level, drawableType, caveat;
        int doubleBuffer, stereo, auxBuffers, renderType, depthSize, stencilSize;
        int redSize, greenSize, blueSize, alphaSize;
        int accumRedSize, accumGreenSize, accumBlueSize, accumAlphaSize;
        int maxPBufferWidth, maxPBufferHeight, maxPBufferPixels;
        int sampleBuffers, samples;

        glXGetFBConfigAttrib (dpy, config, GLX_FBCONFIG_ID, &configID);
        glXGetFBConfigAttrib (dpy, config, GLX_VISUAL_ID, &visualID);
        glXGetFBConfigAttrib (dpy, config, GLX_BUFFER_SIZE, &bufferSize);
        glXGetFBConfigAttrib (dpy, config, GLX_LEVEL, &level);
        glXGetFBConfigAttrib (dpy, config, GLX_DOUBLEBUFFER, &doubleBuffer);
        glXGetFBConfigAttrib (dpy, config, GLX_STEREO, &stereo);
        glXGetFBConfigAttrib (dpy, config, GLX_AUX_BUFFERS, &auxBuffers);
        glXGetFBConfigAttrib (dpy, config, GLX_RENDER_TYPE, &renderType);
        glXGetFBConfigAttrib (dpy, config, GLX_RED_SIZE, &redSize);
        glXGetFBConfigAttrib (dpy, config, GLX_GREEN_SIZE, &greenSize);
        glXGetFBConfigAttrib (dpy, config, GLX_BLUE_SIZE, &blueSize);
        glXGetFBConfigAttrib (dpy, config, GLX_ALPHA_SIZE, &alphaSize);
        glXGetFBConfigAttrib (dpy, config, GLX_DEPTH_SIZE, &depthSize);
        glXGetFBConfigAttrib (dpy, config, GLX_STENCIL_SIZE, &stencilSize);
        glXGetFBConfigAttrib (dpy, config, GLX_ACCUM_RED_SIZE, &accumRedSize);
        glXGetFBConfigAttrib (dpy, config, GLX_ACCUM_GREEN_SIZE, &accumGreenSize);
        glXGetFBConfigAttrib (dpy, config, GLX_ACCUM_BLUE_SIZE, &accumBlueSize);
        glXGetFBConfigAttrib (dpy, config, GLX_ACCUM_ALPHA_SIZE, &accumAlphaSize);
        glXGetFBConfigAttrib (dpy, config, GLX_DRAWABLE_TYPE, &drawableType);
        glXGetFBConfigAttrib (dpy, config, GLX_CONFIG_CAVEAT, &caveat);
        glXGetFBConfigAttrib (dpy, config, GLX_MAX_PBUFFER_WIDTH, &maxPBufferWidth);
        glXGetFBConfigAttrib (dpy, config, GLX_MAX_PBUFFER_HEIGHT, &maxPBufferHeight);
        glXGetFBConfigAttrib (dpy, config, GLX_MAX_PBUFFER_PIXELS, &maxPBufferPixels);
        glXGetFBConfigAttrib (dpy, config, GLX_SAMPLE_BUFFERS_ARB, &sampleBuffers);
        glXGetFBConfigAttrib (dpy, config, GLX_SAMPLES_ARB, &samples);

        //char buff [400];

        printf (
                  "ConfigID:0x%02x VisualID:0x%02x bufferSize:%d level:%d doubleBuffer:%d "
                  "stereo:%d caveat:%d renderType:%d drawableType:%d auxBuffers:%d "
                  "depth bits:%d stencil bits:%d R/G/B/A bits:%d/%d/%d/%d "
                  "Accum R/G/B/A bits:%d/%d/%d/%d Max PBuffer W/H/Size:%d/%d/%d "
                  "Sample buffers/samples:%d/%d\n",
                  configID, visualID, bufferSize, level, doubleBuffer, stereo,
                  caveat & ~GLX_NONE, renderType, drawableType, auxBuffers,
                  depthSize, stencilSize,
                  redSize, greenSize, blueSize, alphaSize,
                  accumRedSize, accumGreenSize, accumBlueSize, accumAlphaSize,
                  maxPBufferWidth, maxPBufferHeight, maxPBufferPixels,
                  sampleBuffers, samples);

}

