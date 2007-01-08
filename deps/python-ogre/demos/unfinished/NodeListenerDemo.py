# This code is in the Public Domain
import Ogre as ogre
import SampleFramework as sf

class MyNodeListener(ogre.Node.Listener):
    def __init__(self):
        ogre.Node.Listener.__init__(self)
        print "**************** Created  *****************************"
    def nodeUpdated(self,xnode):
        print "**************** Updated  *****************************"
        print "A node has just been updated" #, dir(node)
    def nodeDestroyed(self,node):
        print "**************** Destroyed  *****************************"
        print "A node has just been destroyed" #, dir(node)
    def nodeAttached(self,node):
        print "**************** Attached  *****************************"
        print "A node has just been attached" #, dir(node)
    def nodeDetached(self,node):
        print "**************** Detached  *****************************"
        print "A node has just been deattached" #, dir(node)
        

class CameraTrackApplication(sf.Application):

    def __init__(self):
        sf.Application.__init__(self)
        self.myNodeListener = None
        
    def __del__(self):
        "Clear variables, this is needed to ensure the correct order of deletion"
        #if self.myNodeListener:
        #    del self.myNodeListener           
        del self.camera
        del self.sceneManager
        del self.frameListener
        del self.root
        del self.renderWindow              
    def _createScene(self):
        sceneManager = self.sceneManager
        self.myNodeListener = MyNodeListener() 

        sceneManager.ambientLight = ogre.ColourValue(0.7, 0.7, 0.7)
        sceneManager.setSkyDome(True, 'Examples/CloudySky', 4.0, 8.0)

        light = sceneManager.createLight('MainLight')
        light.setPosition (ogre.Vector3(20, 80, 50))

        plane = ogre.Plane()
        plane.normal = ogre.Vector3.UNIT_Y
        plane.d = 200
        
        mm = ogre.MeshManager.getSingleton()
        mm.createPlane('FloorPlane', 'General', plane, 200000.0, 200000.0,
               50, 50, True, 1, 1.0, 1.0, ogre.Vector3(0, 0, 1),
               ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY, ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY, 
               True,True )
               
        entity = sceneManager.createEntity('floor', 'FloorPlane')
        entity.setMaterialName('Examples/RustySteel')
        sceneManager.getRootSceneNode().createChildSceneNode().attachObject(entity)
        
        # create head entity
        headNode = sceneManager.getRootSceneNode().createChildSceneNode()
        entity = sceneManager.createEntity('head', 'ogrehead.mesh')
        headNode.attachObject(entity)
        
        # create ninja entity
        playerNode = sceneManager.getRootSceneNode().createChildSceneNode()
        entity = sceneManager.createEntity('ninja', 'ninja.mesh')
        playerNode.attachObject(entity)

        # create Node Listener
        playerNode.setListener(self.myNodeListener)

        print "NODE LISTENER",dir(playerNode.getListener())
        #print "Skeleton",dir(entity.skeleton.getBoneIterator().next)
        
        
        # detach player node from rootscene
        sceneManager.getRootSceneNode().removeChild(playerNode)
        
        # add player node to headNode, should trigger node Attached
        headNode.addChild(playerNode)
        
        # position node to headNode, should trigger node updated
        playerNode.setPosition (ogre.Vector3(10,10,10))
        p = playerNode.getPosition ()
        print "Player Position " , p.x,p.y,p.z
        
        p.z = 30
        d = playerNode.getPosition ()
        print "Player Position Reset" , d.x,d.y,d.z
        
        # remove child node to headNode, should trigger node detached
        headNode.removeChild(playerNode)

        playerNode.setOrientation (ogre.Quaternion(0.707,0.707,0.707,0.2) )
        o = playerNode.getOrientation()
        print "Orientation ",o.x,o.y,o.z,o.w
        
        o.x = 0.30
        s = playerNode.getOrientation()
        print "Orientation Reset ",s.x,s.y,s.z,s.w
   
#         w,x,y,z = q1=ogre.Quaternion(1.0,0.0,0.0,0.0) 
#         print "Getter ",w,x,y,z

#         print "Type SubMesh",entity.mesh[0]
                
        # make sure the camera track this node
        self.camera.setAutoTracking(True, headNode)

        # create the camera node & attach camera
        cameraNode = sceneManager.getRootSceneNode().createChildSceneNode()
        cameraNode.attachObject(self.camera)

        # set up spline animation of node
        animation = sceneManager.createAnimation('CameraTrack', 10)
        animation.interpolationMode = ogre.Animation.IM_SPLINE
        
        animationTrack = animation.createNodeTrack(0, cameraNode)
        
        key = animationTrack.createNodeKeyFrame(0)
        
        key = animationTrack.createNodeKeyFrame(2.5)
        key.setTranslate (ogre.Vector3( 500.0, 500.0, -1000.0))
        
        key = animationTrack.createNodeKeyFrame(5)
        key.setTranslate = (ogre.Vector3( -1500.0, -1500.0, -600.0))
        
        key = animationTrack.createNodeKeyFrame(7.5)
        key.setTranslate = (ogre.Vector3( 0.0, -100.0, 0.0))
        
        key = animationTrack.createNodeKeyFrame(10.0)
        key.setTranslate = (ogre.Vector3( 0.0, 0.0, 0.0))

        self.animationState = sceneManager.createAnimationState('CameraTrack')
        self.animationState.enabled = True


		
        # add some fog
        sceneManager.setFog(ogre.FOG_EXP, ogre.ColourValue(1, 1, 1), 0.0002)

    def _createFrameListener(self):
        self.frameListener = CameraTrackListener(self.renderWindow, self.camera, self.animationState)
        self.root.addFrameListener(self.frameListener)

class CameraTrackListener(sf.FrameListener):
    def __init__(self, renderWindow, camera, animationState):
        sf.FrameListener.__init__(self, renderWindow, camera)
        self.animationState = animationState

    def frameStarted(self, frameEvent):
        self.animationState.addTime(frameEvent.timeSinceLastFrame)
        return sf.FrameListener.frameStarted(self, frameEvent)

if __name__ == '__main__':
    try:
        application = CameraTrackApplication()
        application.go()
    except ogre.Exception, e:
        print e
    
