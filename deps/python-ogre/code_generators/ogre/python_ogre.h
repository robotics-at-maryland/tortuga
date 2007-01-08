#ifndef __python_ogre_h_5B402FBF_8DE8_49C9_AF71_1CF9634B5344__
#define __python_ogre_h_5B402FBF_8DE8_49C9_AF71_1CF9634B5344__

//See best practices section in Py++ documentation
#include "Ogre.h"
#include "OgreParticleAffectorFactory.h"

#include "OgreBillboardParticleRenderer.h"
#include "OgreParticleSystemRenderer.h"
#include "OgreParticleEmitterFactory.h"
#include "OgreArchiveFactory.h"

#include "OgreErrorDialog.h"
#include "OgreConfigDialog.h"
#include "OgreTagPoint.h"
#include "OgreTargetManager.h"
#include "OgreOverlayElementFactory.h"
#include "OgreArchiveFactory.h"
#include "OgreParticleSystemRenderer.h"
#include "OgreParticleEmitterFactory.h"
#include "OgreParticleAffectorFactory.h"
#include "OgreSkeletonInstance.h"
#include "OgreSkeletonManager.h"
#include "OgreSkeleton.h"
#include "OgreCompositor.h"

//#ifdef OGRE_VERSION_CVS


namespace pyplusplus { namespace aliases {
//inline void instantiate(){    
 
 #include "python_ogre_alias.h"
} } // }


namespace python_ogre{ namespace details{
inline void instantiate(){
 using namespace Ogre;
 #include "python_ogre_sizeof.h"
 
 // note these std's need to be specifically 'included' in generate_code...
 sizeof (std::pair<bool, float> );
 sizeof (std::pair<unsigned int,unsigned int> );
 sizeof (std::map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > >);
 sizeof (ConstMapIterator<std::map<std::string, std::string, std::less<std::string>, std::allocator<std::pair<std::string const, std::string> > > >); 
 sizeof (stdext::_Hash<stdext::_Hmap_traits<std::string, unsigned short, stdext::hash_compare<std::string, std::less<std::string> >, std::allocator<std::pair<std::string const, unsigned short> >, false> >);
 sizeof (stdext::hash_compare<std::string, std::less<std::string> >);
 sizeof(stdext::hash_map<std::string, unsigned short, stdext::hash_compare<std::string, std::less<std::string> >, std::allocator<std::pair<std::string const, unsigned short> > >);
 sizeof(HashMap<String, ushort>);
 sizeof ( std::vector<Ogre::RenderSystem*, std::allocator<Ogre::RenderSystem*> > );
 
 
} } }

//#endif



#ifndef OGRE_VERSION_CVS        // these are needed for the 1.2.x SDK
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
