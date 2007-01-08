# -----------------------------------------------------------------------------
# This source file is part of OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http:#www.ogre3d.org/

# Copyright (c) 2000-2005 The OGRE Team
# Also see acknowledgements in Readme.html

# You may use this sample code for anything you like, it is not covered by the
# LGPL like the rest of the engine.
# -----------------------------------------------------------------------------
# 

# 
#     \file 
#         Shadows.cpp
#     \brief
#         Shows a few ways to use ogre's shadowing techniques
# 
import Ogre as ogre
import SampleFramework as sf

if ogre.OgreVersionString[:2] =="12":
    KC_O = ogre.KC_O
    KC_M = ogre.KC_M
else:
    import OIS
    KC_O = OIS.KC_O
    KC_M = OIS.KC_M


mAnimState = 0
# Entity* pPlaneEnt
mLight =0
mSunLight =0
# SceneNode* mLightNode = 0
# AnimationState* mLightAnimationState = 0
mMinLightColour = ogre.ColourValue(0.3, 0.0, 0)
mMaxLightColour = ogre.ColourValue(0.5, 0.3, 0.1)
mMinFlareSize = 40
mMaxFlareSize = 80

NUM_ATHENE_MATERIALS= 2
mAtheneMaterials =  [ "Examples/Athene/NormalMapped",  "Examples/Athene/Basic"  ]
NUM_SHADOW_TECH = 6
mShadowTechDescriptions = [
    "Stencil Shadows (Additive)",
    "Stencil Shadows (Modulative)",
    "Texture Shadows (Additive)",
    "Texture Shadows (Modulative)",
    "Texture Shadows (Soft Modulative)",
    "None"
]
mShadowTech = [
    ogre.SHADOWTYPE_STENCIL_ADDITIVE,
    ogre.SHADOWTYPE_STENCIL_MODULATIVE,
    ogre.SHADOWTYPE_TEXTURE_ADDITIVE,
    ogre.SHADOWTYPE_TEXTURE_MODULATIVE,
    ogre.SHADOWTYPE_TEXTURE_MODULATIVE, # soft shadows
    ogre.SHADOWTYPE_NONE
]
mShadowTechSoft = [
    False, 
    False, 
    False,
    False, 
    True, 
    False
]
mSoftShadowsSupported = True

mCurrentAtheneMaterial = 0
mCurrentShadowTechnique = 0
SHADOW_COMPOSITOR_NAME = "Gaussian Blur"
mAthene =0

mShadowTechniqueInfo=0
mMaterialInfo = 0
# OverlayElement* mInfo

global NUM_ATHENE_MATERIALS,mAtheneMaterials,NUM_SHADOW_TECH,mShadowTechDescriptions
global mShadowTech,mShadowTechSoft,mSoftShadowsSupported,mCurrentAtheneMaterial,mCurrentShadowTechnique


## This class 'wibbles' the light and billboard 
class LightWibbler ( ogre.FloatControllerValue ):
   global NUM_ATHENE_MATERIALS,mAtheneMaterials,NUM_SHADOW_TECH,mShadowTechDescriptions
   global mShadowTech,mShadowTechSoft,mSoftShadowsSupported,mCurrentAtheneMaterial,mCurrentShadowTechnique
   

   def __init__(self, light, billboard, minColour, maxColour, minSize, maxSize):
        self.mLight = light
        self.mBillboard = billboard
        self.mMinColour = minColour
        self.mColourRange = ogre.ColourValue(0,0,0)
        self.mColourRange.r = maxColour.r - minColour.r
        self.mColourRange.g = maxColour.g - minColour.g
        self.mColourRange.b = maxColour.b - minColour.b
        self.mMinSize = minSize
        self.mSizeRange = maxSize - minSize
        self.intensity=0
        ogre.FloatControllerValue.__init__(self)



   def getValue ():
        return self.intensity
        
   def  setValue ( value):
        
        self.intensity = value

        newColour = ogre.ColourValue(0,0,0)

        # Attenuate the brightness of the light
        newColour.r = self.mMinColour.r + (self.mColourRange.r * self.intensity)
        newColour.g = self.mMinColour.g + (self.mColourRange.g * self.intensity)
        newColour.b = self.mMinColour.b + (self.mColourRange.b * self.intensity)

        self.mLight.setDiffuseColour(newColour)
        self.mBillboard.setColour(newColour)
        # set billboard size
        newSize = self.mMinSize + (self.intensity * self.mSizeRange)
        self.mBillboard.setDimensions(newSize, newSize)


# Real timeDelay = 0
# #define KEY_PRESSED(_key,_timeDelay, _macro) \
# { \
#     if (mInputDevice.isKeyDown(_key) && timeDelay <= 0) \
# { \
#     timeDelay = _timeDelay \
#     _macro  \
# } \
# }


#---------------------------------------------------------------------------
class GaussianListener (ogre.CompositorInstance.Listener):
    global NUM_ATHENE_MATERIALS,mAtheneMaterials,NUM_SHADOW_TECH,mShadowTechDescriptions
    global mShadowTech,mShadowTechSoft,mSoftShadowsSupported,mCurrentAtheneMaterial,mCurrentShadowTechnique
#     mVpWidth = 0
#     mVpHeight = 0
#     # Array params - have to pack in groups of 4 since this is how Cg generates them
    # also prevents dependent texture read problems if ops don't require swizzle
#     mBloomTexWeights=[15][4]
#     mBloomTexOffsetsHorz=[15][4]
#     mBloomTexOffsetsVert=[15][4]
    
    def __init__ (self):
        ogre.CompositorInstance.listener.__init__(self)
        
    def notifyViewportSize(width, height):
        self.mVpWidth = width
        self.mVpHeight = height
        # Calculate gaussian texture offsets & weights
        deviation = 3.0
        texelSize = 1.0 / min(self.mVpWidth, self.mVpHeight)

        # central sample, no offset
        self.mBloomTexOffsetsHorz[0][0] = 0.0
        self.mBloomTexOffsetsHorz[0][1] = 0.0
        self.mBloomTexOffsetsVert[0][0] = 0.0
        self.mBloomTexOffsetsVert[0][1] = 0.0
        self.mBloomTexWeights[0][0] = ogre.Math.gaussianDistribution(0, 0, deviation)
        self.mBloomTexWeights[0][1] = self.mBloomTexWeights[0][0]
        self.mBloomTexWeights[0][2] = self.mBloomTexWeights[0][0]
        self.mBloomTexWeights[0][3] = 1.0

        # 'pre' samples
        for i in range (1,8):
            self.mBloomTexWeights[i][0] = ogre.Math.gaussianDistribution(i, 0, deviation)
            self.mBloomTexWeights[i][1] = self.mBloomTexWeights[i][0]
            self.mBloomTexWeights[i][2] = self.mBloomTexWeights[i][0]
            self.mBloomTexWeights[i][3] = 1.0
            self.mBloomTexOffsetsHorz[i][0] = i * texelSize
            self.mBloomTexOffsetsHorz[i][1] = 0.0
            self.mBloomTexOffsetsVert[i][0] = 0.0
            self.mBloomTexOffsetsVert[i][1] = i * texelSize
        # 'post' samples
        for i in range (8, 15):
            self.mBloomTexWeights[i][0] = self.mBloomTexWeights[i - 7][0]
            self.mBloomTexWeights[i][1] = self.mBloomTexWeights[i][0]
            self.mBloomTexWeights[i][2] = self.mBloomTexWeights[i][0]
            self.mBloomTexWeights[i][3] = 1.0

            self.mBloomTexOffsetsHorz[i][0] = -self.mBloomTexOffsetsHorz[i - 7][0]
            self.mBloomTexOffsetsHorz[i][1] = 0.0
            self.mBloomTexOffsetsVert[i][0] = 0.0
            self.mBloomTexOffsetsVert[i][1] = -self.mBloomTexOffsetsVert[i - 7][1]

    def notifyMaterialSetup( pass_id, mat):
        # Prepare the fragment params offsets
        if pass_id == 701: #blur Horz
            # horizontal bloom
            mat.load()
            fparams = mat.getBestTechnique().getPass(0).getFragmentProgramParameters()
            progName = mat.getBestTechnique().getPass(0).getFragmentProgramName()
            # A bit hacky - Cg & HLSL index arrays via [0], GLSL does not
            if progName.find("GLSL") != ogre.String.npos:
                fparams.setNamedConstant("sampleOffsets", self.mBloomTexOffsetsHorz[0], 15)
                fparams.setNamedConstant("sampleWeights", self.mBloomTexWeights[0], 15)
            else:
                fparams.setNamedConstant("sampleOffsets[0]", self.mBloomTexOffsetsHorz[0], 15)
                fparams.setNamedConstant("sampleWeights[0]", self.mBloomTexWeights[0], 15)
        elif pass_id == 700: # blur vert
            # vertical bloom 
            mat.load()
            fparams = mat.getTechnique(0).getPass(0).getFragmentProgramParameters()
            progName = mat.getBestTechnique().getPass(0).getFragmentProgramName()
            # A bit hacky - Cg & HLSL index arrays via [0], GLSL does not
            if progName.find("GLSL") != ogre.String.npos:
                fparams.setNamedConstant("sampleOffsets", self.mBloomTexOffsetsVert[0], 15)
                fparams.setNamedConstant("sampleWeights", self.mBloomTexWeights[0], 15)
            else:
                fparams.setNamedConstant("sampleOffsets[0]", self.mBloomTexOffsetsVert[0], 15)
                fparams.setNamedConstant("sampleWeights[0]", self.mBloomTexWeights[0], 15)
    
    def notifyMaterialRender(pass_id, mat):
        pass
    
    

###GaussianListener gaussianListener


class ShadowsListener (sf.FrameListener):

    def __init__ ( self, win, cam, sm):
        sf.FrameListener.__init__(self,win, cam)
        self.sceneManager = sm
        self.mShadowVp=0
        self.mShadowCompositor=None
        self.timeDelay=0

#     def frameStarted(self, frameEvent):
#         return sf.FrameListener.frameStarted(self, frameEvent)
 
    def changeShadowTechnique(self):
        global NUM_ATHENE_MATERIALS,mAtheneMaterials,NUM_SHADOW_TECH,mShadowTechDescriptions
        global mShadowTech,mShadowTechSoft,mSoftShadowsSupported,mCurrentAtheneMaterial,mCurrentShadowTechnique
        global mShadowTechniqueInfo, mSunLight, mLight, mMinLightColour, SHADOW_COMPOSITOR_NAME
        
        prevTech = mCurrentShadowTechnique ##TTT
        
        if mCurrentShadowTechnique < (NUM_SHADOW_TECH -1 ):
            mCurrentShadowTechnique+=1
        else : mCurrentShadowTechnique =0
        
#         if not SoftShadowsSupported and mShadowTechSoft[mCurrentShadowTechnique]:
#             # Skip soft shadows if not supported
#             mCurrentShadowTechnique = ++mCurrentShadowTechnique % NUM_SHADOW_TECH
            
            
        mShadowTechniqueInfo.setCaption("Current: " + mShadowTechDescriptions[mCurrentShadowTechnique])

        if mShadowTechSoft[prevTech] and not mShadowTechSoft[mCurrentShadowTechnique]:
            # Clean up compositors
            try:
                mShadowCompositor.removeListener(gaussianListener)
            except:
                pass
            ogre.CompositorManager.getSingleton().setCompositorEnabled(self.mShadowVp, SHADOW_COMPOSITOR_NAME, False)
            # Remove entire compositor chain
            ogre.CompositorManager.getSingleton().removeCompositorChain(self.mShadowVp)
            self.mShadowVp = 0
            self.mShadowCompositor = 0

        self.sceneManager.setShadowTechnique(mShadowTech[mCurrentShadowTechnique])
        ##Vector3 dir #TT
        if mShadowTech[mCurrentShadowTechnique] == ogre.SHADOWTYPE_STENCIL_ADDITIVE:
            # Fixed light, dim
            mSunLight.setCastShadows(True)

            # Point light, movable, reddish
            mLight.setType(Light.LT_POINT)
            mLight.setCastShadows(True)
            mLight.setDiffuseColour(mMinLightColour)
            mLight.setSpecularColour(1, 1, 1)
            mLight.setAttenuation(8000,1,0.0005,0)
        elif  mShadowTech[mCurrentShadowTechnique] == ogre.SHADOWTYPE_STENCIL_MODULATIVE:
            # Multiple lights cause obvious silhouette edges in modulative mode
            # So turn off shadows on the direct light
            # Fixed light, dim
            mSunLight.setCastShadows(False)

            # Point light, movable, reddish
            mLight.setType(ogre.Light.LT_POINT)
            mLight.setCastShadows(True)
            mLight.setDiffuseColour(mMinLightColour)
            mLight.setSpecularColour(1, 1, 1)
            mLight.setAttenuation(8000,1,0.0005,0)
            #break
        elif  (mShadowTech[mCurrentShadowTechnique]== ogre.SHADOWTYPE_TEXTURE_MODULATIVE) or (mShadowTech[mCurrentShadowTechnique] == ogre.SHADOWTYPE_TEXTURE_ADDITIVE) :
            # Fixed light, dim
            mSunLight.setCastShadows(not mShadowTechSoft[mCurrentShadowTechnique])

            # Change moving light to spotlight
            # Point light, movable, reddish
            mLight.setType(ogre.Light.LT_SPOTLIGHT)
            mLight.setDirection(ogre.Vector3.NEGATIVE_UNIT_Z)
            mLight.setCastShadows(True)
            mLight.setDiffuseColour(mMinLightColour)
            mLight.setSpecularColour(1, 1, 1)
            mLight.setAttenuation(8000,1,0.0005,0)
            mLight.setSpotlightRange(ogre.Degree(d=80),ogre.Degree(d=90))


            if mShadowTechSoft[mCurrentShadowTechnique]:
                # set up compositors
                shadowTex = ogre.TextureManager.getSingleton()
                print "###", shadowTex
                shadowTex = shadowTex.getByName(name="ogre/ShadowTexture0") ## returns a None object
                print "###", shadowTex
                if shadowTex:
                    shadowRtt = shadowTex.getBuffer().getRenderTarget()
                    mShadowVp = shadowRtt.getViewport(0)
                    mShadowCompositor = ogre.CompositorManager.getSingleton().addCompositor(mShadowVp, SHADOW_COMPOSITOR_NAME)
                    ogre.CompositorManager.getSingleton().setCompositorEnabled(self.mShadowVp, SHADOW_COMPOSITOR_NAME, True)
                    mShadowCompositor.addListener(gaussianListener)
                    gaussianListener.notifyViewportSize(self.mShadowVp.getActualWidth(), self.mShadowVp.getActualHeight())

    def changeAtheneMaterial(self):
        global mCurrentAtheneMaterial, mMaterialInfo, mAthene, mAtheneMaterials, NUM_ATHENE_MATERIALS
        if mCurrentAtheneMaterial < (NUM_ATHENE_MATERIALS-1):
            mCurrentAtheneMaterial += 1
        else:
            mCurrentAtheneMaterial=0
        mMaterialInfo.setCaption("Current: " + mAtheneMaterials[mCurrentAtheneMaterial])
        mAthene.setMaterialName(mAtheneMaterials[mCurrentAtheneMaterial])
    
    def CheckKeyPressed ( self, key, time, func):
        if self._isToggleKeyDown(key, time):
            self.timeDelay = time 
            func()
        
    def frameEnded(self,  evt):
        global mAnimState
        if (self.timeDelay >= 0) :
            self.timeDelay -= evt.timeSinceLastFrame

        if (mAnimState):
            mAnimState.addTime(evt.timeSinceLastFrame)

        self.CheckKeyPressed(KC_O, 1, self.changeShadowTechnique)  
        self.CheckKeyPressed(KC_M, 0.5, self.changeAtheneMaterial)

        return sf.FrameListener.frameStarted(self, evt) and sf.FrameListener.frameEnded(self, evt)        


class ShadowsApplication ( sf.Application ):
    global NUM_ATHENE_MATERIALS,mAtheneMaterials,NUM_SHADOW_TECH,mShadowTechDescriptions
    global mShadowTech,mShadowTechSoft,mSoftShadowsSupported,mCurrentAtheneMaterial,mCurrentShadowTechnique
    global mCurrentAtheneMaterial, mMaterialInfo, mAthene, mAtheneMaterials
    global mAnimState
    def __init__(self):
        sf.Application.__init__(self)

    def _createScene(self):
        global NUM_ATHENE_MATERIALS,mAtheneMaterials,NUM_SHADOW_TECH,mShadowTechDescriptions
        global mShadowTech,mShadowTechSoft,mSoftShadowsSupported,mCurrentAtheneMaterial,mCurrentShadowTechnique
        global mCurrentAtheneMaterial, mMaterialInfo, mAthene, mAtheneMaterials
        global mShadowTechniqueInfo, mSunLight, mLight, mMinLightColour
        sceneManager = self.sceneManager
       
        # do this first so we generate edge lists
        sceneManager.setShadowTechnique(ogre.SHADOWTYPE_STENCIL_ADDITIVE)
        # Set the default Athene material
        # We'll default it to the normal map for ps_2_0 capable hardware
        # everyone else will default to the basic
#         if (GpuProgramManager.getSingleton().isSyntaxSupported("ps_2_0") or
#             GpuProgramManager.getSingleton().isSyntaxSupported("arbfp1")):
#             mCurrentAtheneMaterial = 0
#         else:
        mCurrentAtheneMaterial = 1

        # Set ambient light off
        sceneManager.setAmbientLight(ogre.ColourValue(0.0, 0.0, 0.0))

        # Fixed light, dim
        mSunLight = sceneManager.createLight("SunLight")
        mSunLight.setType(ogre.Light.LT_SPOTLIGHT)
        mSunLight.setPosition(1000,1250,500)
        mSunLight.setSpotlightRange(ogre.Degree(d=30), ogre.Degree(d=50))
        dirV = -mSunLight.getPosition()
        dirV.normalise()
        mSunLight.setDirection(dirV)
        mSunLight.setDiffuseColour(0.35, 0.35, 0.38)
        mSunLight.setSpecularColour(0.9, 0.9, 1)

        # Point light, movable, reddish
        mLight = sceneManager.createLight("Light2")
        mLight.setDiffuseColour(mMinLightColour)
        mLight.setSpecularColour(1, 1, 1)
        mLight.setAttenuation(8000,1,0.0005,0)

        # Create light node
        mLightNode = sceneManager.getRootSceneNode().createChildSceneNode("MovingLightNode")
        mLightNode.attachObject(mLight)
        # create billboard set
        bbs = sceneManager.createBillboardSet("lightbbs", 1)
        bbs.setMaterialName("Examples/Flare")
        bb = bbs.createBillboard(0,0,0,mMinLightColour)
        # attach
        mLightNode.attachObject(bbs)

        # create controller, after this is will get updated on its own
#         func = ogre.ControllerFunctionRealPtr(ogre.WaveformControllerFunction(ogre.WFT_SINE, 0.75, 0.5))
#         contMgr = ogre.ControllerManager.getSingleton()
#         val = ogre.ControllerValueRealPtr(LightWibbler(mLight, bb, mMinLightColour, mMaxLightColour, mMinFlareSize, mMaxFlareSize))
# #        val = LightWibbler(mLight, bb, mMinLightColour, mMaxLightColour, mMinFlareSize, mMaxFlareSize)
# #        controller = contMgr.createController(contMgr.getFrameTimeSource(), val, func)

        #mLight.setPosition(Vector3(300,250,-300))
        mLightNode.setPosition(ogre.Vector3(300,250,-300))


        # Create a track for the light
        anim = sceneManager.createAnimation("LightTrack", 20)
        # Spline it for nice curves
        anim.setInterpolationMode(ogre.Animation.IM_SPLINE)
        # Create a track to animate the camera's node
        track = anim.createNodeTrack(0, mLightNode)
        # Setup keyframes
        key = track.createNodeKeyFrame(0) # A startposition
        key.setTranslate(ogre.Vector3(300,250,-300))
        key = track.createNodeKeyFrame(2)#B
        key.setTranslate(ogre.Vector3(150,300,-250))
        key = track.createNodeKeyFrame(4)#C
        key.setTranslate(ogre.Vector3(-150,350,-100))
        key = track.createNodeKeyFrame(6)#D
        key.setTranslate(ogre.Vector3(-400,200,-200))
        key = track.createNodeKeyFrame(8)#E
        key.setTranslate(ogre.Vector3(-200,200,-400))
        key = track.createNodeKeyFrame(10)#F
        key.setTranslate(ogre.Vector3(-100,150,-200))
        key = track.createNodeKeyFrame(12)#G
        key.setTranslate(ogre.Vector3(-100,75,180))
        key = track.createNodeKeyFrame(14)#H
        key.setTranslate(ogre.Vector3(0,250,300))
        key = track.createNodeKeyFrame(16)#I
        key.setTranslate(ogre.Vector3(100,350,100))
        key = track.createNodeKeyFrame(18)#J
        key.setTranslate(ogre.Vector3(250,300,0))
        key = track.createNodeKeyFrame(20)#K == A
        key.setTranslate(ogre.Vector3(300,250,-300))
        # Create a new animation state to track this
        mAnimState = sceneManager.createAnimationState("LightTrack")
        mAnimState.setEnabled(True)
        # Make light node look at origin, this is for when we
        # change the moving light to a spotlight
        mLightNode.setAutoTracking(True, sceneManager.getRootSceneNode())

        # Prepare athene mesh for normalmapping
        pAthene = ogre.MeshManager.getSingleton().load("athene.mesh", 
            ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME)
        src = 0
        dest = 0 
#         if (not pAthene.suggestTangentVectorBuildParams(outSourceCoordSet=src, outDestCoordSet=dest)):
#              pAthene.buildTangentVectors(src, dest)
 
        node = sceneManager.getRootSceneNode().createChildSceneNode()
        mAthene = sceneManager.createEntity( "athene", "athene.mesh" )
        mAthene.setMaterialName(mAtheneMaterials[mCurrentAtheneMaterial])
        node.attachObject( mAthene )
        node.translate(0,-20, 0)
        node.yaw(ogre.Degree(d=90))

        node = sceneManager.getRootSceneNode().createChildSceneNode()
        pEnt = sceneManager.createEntity( "col1", "column.mesh" )
        pEnt.setMaterialName("Examples/Rockwall")
        node.attachObject( pEnt )
        node.translate(200,0, -200)

        node = sceneManager.getRootSceneNode().createChildSceneNode()
        pEnt = sceneManager.createEntity( "col2", "column.mesh" )
        pEnt.setMaterialName("Examples/Rockwall")
        node.attachObject( pEnt )
        node.translate(200,0, 200)

        node = sceneManager.getRootSceneNode().createChildSceneNode()
        pEnt = sceneManager.createEntity( "col3", "column.mesh" )
        pEnt.setMaterialName("Examples/Rockwall")
        node.attachObject( pEnt )
        node.translate(-200,0, -200)

        node = sceneManager.getRootSceneNode().createChildSceneNode()
        pEnt = sceneManager.createEntity( "col4", "column.mesh" )
        pEnt.setMaterialName("Examples/Rockwall")
        node.attachObject( pEnt )
        node.translate(-200,0, 200)
        # Skybox
        sceneManager.setSkyBox(True, "Examples/StormySkyBox")

        # Floor plane
        plane = ogre.Plane()
        plane.normal = ogre.Vector3.UNIT_Y
        plane.d = 100
        ogre.MeshManager.getSingleton().createPlane("Myplane",
            ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
            1500,1500,50,50,True,1,5,5,ogre.Vector3.UNIT_Z)
        pPlaneEnt = sceneManager.createEntity( "plane", "Myplane" )
        pPlaneEnt.setMaterialName("Examples/Rockwall")
        pPlaneEnt.setCastShadows(False)
        sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt)

        # show overlay
        pOver = ogre.OverlayManager.getSingleton().getByName("Example/ShadowsOverlay")    
        mShadowTechniqueInfo = ogre.OverlayManager.getSingleton().getOverlayElement("Example/Shadows/ShadowTechniqueInfo")
        mMaterialInfo = ogre.OverlayManager.getSingleton().getOverlayElement("Example/Shadows/MaterialInfo")
        mInfo = ogre.OverlayManager.getSingleton().getOverlayElement("Example/Shadows/Info")

        mShadowTechniqueInfo.setCaption("Current: " + mShadowTechDescriptions[mCurrentShadowTechnique])
        mMaterialInfo.setCaption("Current: " + mAtheneMaterials[mCurrentAtheneMaterial])
        pOver.show()

#         if (sceneManager.getRootSceneNode().getRenderSystem().getCapabilities().hasCapability(ogre.RSC_HWRENDER_TO_TEXTURE)):
#             # In D3D, use a 1024x1024 shadow texture
#             sceneManager.setShadowTextureSettings(1024, 2)
#         else:
#             # Use 512x512 texture in GL since we can't go higher than the window res
#             sceneManager.setShadowTextureSettings(512, 2)
        sceneManager.setShadowTextureSettings(512, 2)
        sceneManager.setShadowColour(ogre.ColourValue(0.5, 0.5, 0.5))

        # incase infinite far distance is not supported
#        self.camera.setFarClipDistance(100000)

        #sceneManager.setShowDebugShadows(True)
        mSoftShadowsSupported = False

#         caps = Root.getSingleton().getRenderSystem().getCapabilities()
#         if (not caps.hasCapability(ogre.RSC_VERTEX_PROGRAM) or not (caps.hasCapability(ogre.RSC_FRAGMENT_PROGRAM))):
#             mSoftShadowsSupported = False
#         else:
#             if (not GpuProgramManager.getSingleton().isSyntaxSupported("glsl") and
#                 not GpuProgramManager.getSingleton().isSyntaxSupported("ps_2_0")) :
#                 mSoftShadowsSupported = False

    # Create new frame listener
    def _createFrameListener(self):
        self.frameListener= ShadowsListener(self.renderWindow, self.camera, self.sceneManager)
        self.frameListener.showDebugOverlay(True)
        self.root.addFrameListener(self.frameListener)


 
if __name__ == '__main__':
    try:
        application = ShadowsApplication()
        application.go()
    except ogre.Exception, e:
        print e
        print
        print "** FullDesc ", e.fullDescription
        print "** Source, File, Line ",e.source, e.file, e.line


