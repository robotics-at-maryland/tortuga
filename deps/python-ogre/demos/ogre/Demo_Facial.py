import Ogre as ogre
import OIS as OIS
import SampleFramework 

class TutorialListener(SampleFramework.FrameListener):
    
    def __init__(self, renderWindow, camera, sceneManager,AnimStateList, camnode):
        SampleFramework.FrameListener.__init__(self, renderWindow, camera)
        self.camNode = camnode ## camera.getParentSceneNode().parentSceneNode
        self.sceneManager = sceneManager
        self.animationStates = AnimStateList
        
    def frameStarted(self, evt):
        self.animationStates[0].addTime(evt.timeSinceLastFrame)
        return SampleFramework.FrameListener.frameStarted(self, evt)


class TutorialApplication(SampleFramework.Application):

    def __init__(self):
        self.animationStates = []
        SampleFramework.Application.__init__(self)

    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.setAmbientLight(ogre.ColourValue(1, 1, 1))
        sceneManager.setShadowTechnique(ogre.SHADOWTYPE_STENCIL_ADDITIVE)

        plane = ogre.Plane(ogre.Vector3(0, 1, 0), 0)
        mm = ogre.MeshManager.getSingleton()
        mm.createPlane('ground', 'general', plane, 1500, 1500, 20, 20,
                       True, 1, 5, 5, ogre.Vector3(0, 0, 1),
                       ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY, ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY, 
                       True,True )

   
        ent = sceneManager.createEntity("GroundEntity", "ground")
        ent.setMaterialName("Examples/Rockwall")
        sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ent)
        ent.castShadows = False

        headNode = sceneManager.getRootSceneNode().createChildSceneNode()
        head = sceneManager.createEntity("Head", "facial.mesh")
        headNode.attachObject(head)
        anim = head.getAnimationState("Speak")
        anim.setEnabled (True)
        anim.setLoop ( True)
        self.animationStates.append( anim)

        # Create Camera
        self.camnode = sceneManager.RootSceneNode.createChildSceneNode("CamNode1", ogre.Vector3(0, 25, 90))
        node = self.camnode.createChildSceneNode("PitchNode1")
        node.attachObject(self.camera)

        
    def _createCamera(self):
        self.camera = self.sceneManager.createCamera("PlayerCam")
        self.camera.nearClipDistance = 5

    def _createFrameListener(self):
        self.frameListener = TutorialListener(self.renderWindow, self.camera, self.sceneManager,
                                              self.animationStates, self.camnode)
        self.root.addFrameListener(self.frameListener)
        self.frameListener.showDebugOverlay(True)

 

if __name__ == '__main__':
    import exceptions
    try:
        application = TutorialApplication()
        application.go()
    except ogre.Exception, e:
        print e
#        print dir(e)
    except exceptions.RuntimeError, e:
        print "Runtime error:", e
    except exceptions.TypeError, e:
        print "Type error:", e
    except exceptions.AttributeError, e:
        print "Attribute error:", e
    except exceptions.NameError, e:
        print "Name error:", e
    except Exception,inst:
        print "EException"
        print type(inst)     # the exception instance
        print inst.args      # arguments stored in .args
        print inst
    except exceptions.ValueError,e:
        print "ValueError",e
    except :
        print "Unexpected error:", sys.exc_info()[0]