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

    mWxOgre = new wxOgre(0, this, wxID_ANY, wxDefaultPosition,
                                wxSize(400,400), 0, wxT("OgreWindow"));

    /*new MyTestControl(this, wxID_ANY, wxDefaultPosition,
                      wxSize(400,400), 0, wxT("OgreWindow"));*/
}

void MyFrame::onActivate(wxActivateEvent& event)
{
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
