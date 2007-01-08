# This code is in the Public Domain.
import Ogre as ogre
import SampleFramework as sf
import sys,operator

NUM_JAIQUAS = 6
mAnimationRotation = ogre.Degree(d=-60)
mAnimChop = 7.96666
mAnimChopBlend = 0.3
mAnimState=[0,0,0,0,0,0]  #NUM_JAIQUAS]
mAnimationSpeed=[0,0,0,0,0,0]  #
mSneakStartOffset=ogre.Vector3(100,200,30)
mSneakEndOffset=ogre.Vector3(0,0,0)
mOrientations=[0,0,0,0,0,0]  #
mBasePositions=[0,0,0,0,0,0]  #
mSceneNode=[0,0,0,0,0,0]  #]


class SkeletalApplication(sf.Application):

    def _createScene(self):
        global NUM_JAIQUAS
        global mAnimationRotation,mAnimChop,mAnimChopBlend,mAnimState,mAnimationSpeed,mSneakStartOffset
        global mSneakEndOffset, mOrientations, mBasePositions, mSceneNode

        sceneManager = self.sceneManager
        camera = self.camera
        
        #setup Shadows
        sceneManager.setShadowTechnique(ogre.SHADOWTYPE_TEXTURE_MODULATIVE)
        sceneManager.setShadowTextureSize(512)
        sceneManager.setShadowColour(ogre.ColourValue(0.6, 0.6, 0.6))

        # Setup animation default
        ogre.Animation.setDefaultInterpolationMode(ogre.Animation.IM_LINEAR)
        ogre.Animation.setDefaultRotationInterpolationMode(ogre.Animation.RIM_LINEAR)
        
        # Need some basic light
        sceneManager.AmbientLight = ogre.ColourValue(0.5, 0.5, 0.5)

        
        # The jaiqua sneak animation doesn't loop properly, so lets hack it so it does
        # We want to copy the initial keyframes of all bones, but alter the Spineroot
        # to give it an offset of where the animation ends
        
        ## Doing this returns a SharedPtr_less_Ogre_scope_Resource_grate 
        skel = ogre.SkeletonManager.getSingleton().load("jaiqua.skeleton", ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,False, ogre.ManualResourceLoader(), ogre.NamedValuePairList()) 
        ### NOTE the need for pointer here....
        anim = skel.getAnimation("Sneak") 
        
        cameraNode = sceneManager.getRootSceneNode().createChildSceneNode()
        cameraNode.attachObject(self.camera)
    
        animation = sceneManager.createAnimation('TestTrack', 1)
        animationTrack = animation.createNodeTrack(0, cameraNode)
    
        blankKF =animationTrack.createKeyFrame (0) 
        evect = ogre.Vector3(0,0,0)
        blankKF.setScale=evect
        blankKF.setTranslate=evect

               
#        for track in anim.getNodeTrackIterator(): 
###
### AJM:  Note that we haven't implemented a python wrapper to make TrackIterator a real python iterator
###       so work around with a classic while loop...
###
        trackIter = anim.getNodeTrackIterator()
        while (trackIter.hasMoreElements()):
            track = trackIter.getNext()
            oldKf = blankKF
            track.getInterpolatedKeyFrame(mAnimChop, oldKf) 

            # Drop all keyframes after the chop
            while (track.getKeyFrame(track.getNumKeyFrames()-1).getTime() >= mAnimChop - mAnimChopBlend):
                track.removeKeyFrame(track.getNumKeyFrames()-1) 

            newKf = track.createNodeKeyFrame(mAnimChop) 
            startKf = track.getNodeKeyFrame(0) 

            bone = skel.getBone(track.getHandle())
             
            if (bone.getName() == "Spineroot") :
                mSneakStartOffset = startKf.getTranslate() + bone.getInitialPosition() 
                mSneakEndOffset = oldKf.getTranslate() + bone.getInitialPosition() 
                mSneakStartOffset.y = mSneakEndOffset.y 
                # Adjust spine root relative to new location
                newKf.setRotation(oldKf.getRotation()) 
                newKf.setTranslate(oldKf.getTranslate()) 
                newKf.setScale(oldKf.getScale()) 
            else:
                newKf.setRotation(startKf.getRotation()) 
                newKf.setTranslate(startKf.getTranslate()) 
                newKf.setScale(startKf.getScale()) 

        rotInc = ogre.Math.TWO_PI / NUM_JAIQUAS 
        rot = 0.0 
        for i in range( NUM_JAIQUAS) :
            q = ogre.Quaternion()
            q.FromAngleAxis(ogre.Radian(r=rot), ogre.Vector3.UNIT_Y) 
            mOrientations[i] = q 
            mBasePositions[i] = q * ogre.Vector3(0,0,-20) 
            ent = sceneManager.createEntity("jaiqua" + str(i), "jaiqua.mesh") 
            # Add entity to the scene node
            mSceneNode[i] = sceneManager.getRootSceneNode().createChildSceneNode() 
            mSceneNode[i].attachObject(ent) 
            mSceneNode[i].rotate(q) 
            mSceneNode[i].translate(mBasePositions[i]) 
            
            mAnimState[i] = ent.getAnimationState("Sneak") 
            mAnimState[i].setEnabled(True) 
            mAnimState[i].setLoop(False)  # manual loop since translation involved
            mAnimationSpeed[i] = ogre.Math.RangeRandom(0.5, 1.5) 

            rot = rot + rotInc 

        light = sceneManager.createLight('BlueLight')
        light.setType(ogre.Light.LT_SPOTLIGHT)
        light.setPosition (-200, 150, -100)
        dirvec = -light.getPosition()
        dirvec.normalise()
        light.setDirection(dirvec)
        light.setDiffuseColour(0.5, 0.5, 1.0)

        light = sceneManager.createLight('GreenLight')
        light.setType(ogre.Light.LT_SPOTLIGHT)
        light.setPosition (0, 150, -100)
        dirvec = -light.getPosition()
        dirvec.normalise()
        light.setDirection(dirvec)
        light.setDiffuseColour (0.5, 1.0, 0.5)
 
        # Position the camera
        camera.setPosition(100, 20, 0)
        camera.lookAt(0, 10, 0)
        # Report whether hardware skinning is enabled or not
###        subEntity = entity.getSubEntity(0)
#         material = subEntity.material
#         technique = material.bestTechnique
#         techniquePass = technique.getPass(0)
#         if techniquePass.hasVertexProgram and techniquePass.vertexProgram.skeletalAnimationIncluded:
#             self.renderWindow.debugText = 'Hardware skinning is enabled'
#         else:
#             self.renderWindow.debugText = 'Software skinning is enabled'

            
        plane = ogre.Plane()
        plane.normal = ogre.Vector3.UNIT_Y
        plane.d = 100
        ogre.MeshManager.getSingleton().createPlane("Myplane",
            ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
            1500,1500,20,20,True,1,60,60,ogre.Vector3.UNIT_Z)
        pPlaneEnt = sceneManager.createEntity( "plane", "Myplane" )
        pPlaneEnt.setMaterialName("Examples/Rockwall")
        pPlaneEnt.setCastShadows(False)
        sceneManager.getRootSceneNode().createChildSceneNode(ogre.Vector3(0,99,0)).attachObject(pPlaneEnt)

            
            
    def _createFrameListener(self):
        self.frameListener = SkeletalAnimationFrameListener(self.renderWindow, self.camera ) # self.animationStates, self.animationSpeeds)
        self.root.addFrameListener(self.frameListener)

class SkeletalAnimationFrameListener(sf.FrameListener):
    global NUM_JAIQUAS
    global mAnimationRotation,mAnimChop,mAnimChopBlend,mAnimState,mAnimationSpeed,mSneakStartOffset
    global mSneakEndOffset, mOrientations, mBasePositions, mSceneNode

    def __init__(self, renderWindow, camera ):  #, animationStates, animationSpeeds):
        sf.FrameListener.__init__(self, renderWindow, camera)
        #self.animationStates = animationStates
        #self.animationSpeeds = animationSpeeds

    def frameStarted(self, frameEvent):
 
        for i in range(NUM_JAIQUAS):
            inc = frameEvent.timeSinceLastFrame * mAnimationSpeed[i]
            if (mAnimState[i].getTimePosition() + inc) >= mAnimChop :
                # pass
                # Loop
                # Need to reposition the scene node origin since animation includes translation
                # Calculate as an offset to the end position, rotated by the
                # amount the animation turns the character
                rot = ogre.Quaternion(mAnimationRotation, ogre.Vector3.UNIT_Y) 
                startoffset = mSceneNode[i].getOrientation() * -mSneakStartOffset 
                endoffset = mSneakEndOffset 
                offset = rot * startoffset 
                currEnd = mSceneNode[i].getOrientation() * endoffset + mSceneNode[i].getPosition() 
                mSceneNode[i].setPosition(currEnd + offset) 
                mSceneNode[i].rotate(q=rot) 
                mAnimState[i].setTimePosition((mAnimState[i].getTimePosition() + inc) - mAnimChop) 
            else:
                mAnimState[i].addTime(inc)
        return sf.FrameListener.frameStarted(self, frameEvent)

if __name__ == '__main__':
    try:
        application = SkeletalApplication()
        application.go()
#     except ogre.Exception, e:
#         print "############"
#         print e
    except:
        print "Unexpected error:", sys.exc_info()[0]
        raise
#     except
#         print "-------"
#         print "PROBLEM!!!"
#         e = ogre.Exception.getLastException()
#         print "EXCEPTION :  ", e
        
            
