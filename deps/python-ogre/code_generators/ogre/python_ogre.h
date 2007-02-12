#ifndef __python_ogre_h_5B402FBF_8DE8_49C9_AF71_1CF9634B5344__
#define __python_ogre_h_5B402FBF_8DE8_49C9_AF71_1CF9634B5344__

//See best practices section in Py++ documentation
#include "python_ogre_masterlist.h"

// #include "Ogre.h"
// #include "OgreParticleAffectorFactory.h"

// #include "OgreBillboardParticleRenderer.h"
// #include "OgreParticleSystemRenderer.h"
// #include "OgreParticleEmitterFactory.h"
// #include "OgreArchiveFactory.h"

// #include "OgreErrorDialog.h"
// #include "OgreConfigDialog.h"
// #include "OgreTagPoint.h"
// #include "OgreOverlayElementFactory.h"
// #include "OgreArchiveFactory.h"
// #include "OgreParticleSystemRenderer.h"
// #include "OgreParticleEmitterFactory.h"
// #include "OgreParticleAffectorFactory.h"
// #include "OgreSkeletonInstance.h"
// #include "OgreSkeletonManager.h"
// #include "OgreSkeleton.h"
// #include "OgreCompositor.h"
// #include "OgrePlugin.h"

typedef HashMap<Ogre::String, unsigned short> OgreHashMapStringUshortOuter;

// First we create a magic namespace to hold all our aliases
namespace pyplusplus { namespace aliases {
    
 #include "python_ogre_aliases.h"
} } 

// then we exposed everything needed (and more) to ensure GCCXML makes them visible to Py++
//
namespace python_ogre{ namespace details{
inline void instantiate(){
 using namespace Ogre;
 #include "python_ogre_sizeof.h"
 
} } }


// This code is VERY UNTESTED and PROBABLY BROKEN
// Uee the CVS version of Ogre !!!!
#ifndef OGRE_VERSION_1.4        // these are needed for the 1.2.x SDK
    #include "OgreEventListeners.h"
    #include "OgreEventQueue.h"
    #include "OgreKeyEvent.h"
    #include "OgreMouseEvent.h"

namespace Ogre {
class CombinedListener : public FrameListener, public KeyListener,
                 public MouseListener, public MouseMotionListener
{
public:
    CombinedListener() {}
    virtual ~CombinedListener() {}
    
    // KeyListener methods
    virtual void keyClicked(KeyEvent *e) {}
    virtual void keyPressed(KeyEvent *e) {}
    virtual void keyReleased(KeyEvent *e) {}
    
    // MouseListener methods
    virtual void mouseClicked(MouseEvent *e) {}
    virtual void mousePressed(MouseEvent *e) {}
    virtual void mouseReleased(MouseEvent *e) {}
    virtual void mouseEntered(MouseEvent *e) {}
    virtual void mouseExited(MouseEvent *) {}
    
    // MouseMotionListener methods
    virtual void mouseMoved(MouseEvent *e) {}
    virtual void mouseDragged(MouseEvent *e) {}
    
    // FrameEvent methods
    virtual bool frameStarted (const FrameEvent &evt) {return true;}
    virtual bool frameEnded (const FrameEvent &evt) {return true;}
};

}
#endif



#endif//__python_ogre_h_5B402FBF_8DE8_49C9_AF71_1CF9634B5344__
