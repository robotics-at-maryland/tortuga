# This code is in the Public Domain
# Note light trails disabled as currently broken in OgreCVS (15/10/06)
import Ogre as ogre
import SampleFramework as sf

class LightingApplication(sf.Application):
    def __init__(self):
        self.mAnimStateList=[]	
        sf.Application.__init__(self)
    def _createScene(self):
        sceneManager = self.sceneManager
        camera = self.camera
        
        sceneManager.AmbientLight = ogre.ColourValue(0.5, 0.5, 0.5)
        sceneManager.setSkyBox(True, 'Examples/SpaceSkyBox')

        entity = sceneManager.createEntity('head', 'ogrehead.mesh')
        sceneManager.getRootSceneNode().attachObject(entity)

        
        
        ## SETUP Trail Lighting
        # light 1		
        direct_vec = ogre.Vector3(-1, -1, 0.5)
        direct_vec.normalise() #dCopy()
        l = sceneManager.createLight("light1")
        l.setType (ogre.Light.LT_DIRECTIONAL)   # or .type
        l.setDirection(direct_vec)

#         # create Ribbon Trail
        pairList = ogre.NameValuePairList()
        pairList['numberOfChains'] = '2'
        pairList['maxElements'] = '80'

        trail=sceneManager.createMovableObject("1", "RibbonTrail", pairList)    # this returns a ribbontrail object
        trail.setMaterialName ("Examples/LightRibbonTrail")
        trail.setTrailLength (200)

        # attach Ribbon Trail scene node
        sceneManager.getRootSceneNode().createChildSceneNode().attachObject(trail)

		#Create 3 nodes for trail to follow
        animNode = sceneManager.getRootSceneNode().createChildSceneNode()
        animNode.setPosition(50,30,0)
        anim = sceneManager.createAnimation("an1", 14)
        anim.setInterpolationMode(ogre.Animation.IM_SPLINE)
        track = anim.createNodeTrack(1, animNode)
        kf = track.createNodeKeyFrame(0)
        kf.Translate=ogre.Vector3(50,30,0)
        kf = track.createNodeKeyFrame(2)
        kf.Translate=ogre.Vector3(100, -30, 0)
        kf = track.createNodeKeyFrame(4)
        kf.Translate=ogre.Vector3(120, -100, 150)
        kf = track.createNodeKeyFrame(6)
        kf.Translate=ogre.Vector3(30, -100, 50)
        kf = track.createNodeKeyFrame(8)
        kf.Translate=ogre.Vector3(-50, 30, -50)
        kf = track.createNodeKeyFrame(10)
        kf.Translate=ogre.Vector3(-150, -20, -100)
        kf = track.createNodeKeyFrame(12)
        kf.Translate=ogre.Vector3(-50, -30, 0)
        kf = track.createNodeKeyFrame(14)
        kf.Translate=ogre.Vector3(50,30,0)

        animState = sceneManager.createAnimationState("an1")
        animState.Enabled=True
        self.mAnimStateList.append(animState)

        trail.setInitialColour(0, 1.0, 0.8, 0)
        trail.setColourChange(0, 0.5, 0.5, 0.5, 0.5)
        trail.setInitialWidth(1, 5)
        trail.addNode(animNode)

        #Add light
        l2 = sceneManager.createLight("l2")
        l2.setDiffuseColour(trail.getInitialColour(0))
        animNode.attachObject(l2)

        #Add billboard
        bbs = sceneManager.createBillboardSet("bb", 1)
        bbs.createBillboard(ogre.Vector3.ZERO, trail.getInitialColour(0))
        bbs.MaterialName="Examples/Flare"
        animNode.attachObject(bbs)

        animNode = sceneManager.getRootSceneNode().createChildSceneNode()
        
        ### Note the setPosition --  the Position/position properties are only 'get' properties as
        ### there are multiple overrides for setPosition (one 
        
        #animNode.setPosition(-50,100,0)
        anim = sceneManager.createAnimation("an2", 10)
        anim.setInterpolationMode(ogre.Animation.IM_SPLINE)
        track = anim.createNodeTrack(1, animNode)
        kf = track.createNodeKeyFrame(0)
        kf.Translate=ogre.Vector3(-50,100,0)
        kf = track.createNodeKeyFrame(2)
        kf.Translate=ogre.Vector3(-100, 150, -30)
        kf = track.createNodeKeyFrame(4)
        kf.Translate=ogre.Vector3(-200, 0, 40)
        kf = track.createNodeKeyFrame(6)
        kf.Translate=ogre.Vector3(0, -150, 70)
        kf = track.createNodeKeyFrame(8)
        kf.Translate=ogre.Vector3(0, 0, 30)
        kf = track.createNodeKeyFrame(10)
        kf.Translate=ogre.Vector3(-50,100,0)

        animState = sceneManager.createAnimationState("an2")
        animState.Enabled=True
        self.mAnimStateList.append(animState)

        trail.setInitialColour(1, 0.0, 1.0, 0.4)
        trail.setColourChange(1, 0.5, 0.5, 0.5, 0.5)
        trail.setInitialWidth(1, 5)
        trail.addNode(animNode)


        #Add light
        l3 = sceneManager.createLight("l3")
        l3.setDiffuseColour (trail.getInitialColour(1))
        animNode.attachObject(l3)

        #Add billboard
        bbs1 = sceneManager.createBillboardSet("bb2", 1)
        bbs1.createBillboard(ogre.Vector3.ZERO, trail.getInitialColour(1))
        bbs1.MaterialName="Examples/Flare"
        animNode.attachObject(bbs1)

			
    def _createFrameListener(self):
	
        self.frameListener = LightingListener(self.renderWindow, self.camera, self.mAnimStateList)
        self.root.addFrameListener(self.frameListener)

class LightingListener(sf.FrameListener):
    def __init__(self, renderWindow, camera, mAnimStateList):
        sf.FrameListener.__init__(self, renderWindow, camera)
        self.mAnimStateList = mAnimStateList

    def frameStarted(self, frameEvent):
        for a in self.mAnimStateList:
            a.addTime(frameEvent.timeSinceLastFrame)            		
        return sf.FrameListener.frameStarted(self, frameEvent)
        #self.speakAnimState.addTime(evt.timeSinceLastFrame)


if __name__ == '__main__':
    try:
        application = LightingApplication()
        application.go()
    except ogre.Exception, e:
        print e
