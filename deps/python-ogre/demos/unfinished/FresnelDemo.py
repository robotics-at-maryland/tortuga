#Fish!
NUM_FISH = 30
NUM_FISH_WAYPOINTS= 10
FISH_PATH_LENGTH=200 

import Ogre as ogre
import SampleFramework as sf
import math

# A sample manual resource loader
class MyManualLoader(ogre.ManualResourceLoader):

    def __init__(self):
        ogre.ManualResourceLoader.__init__(self)
        
    def	loadResource(self,resource):
        pass    
        
# Refraction Render Target Listener
class RefractionTextureListener(ogre.RenderTargetListener):

    def __init__(self,planeEnt,aboveWaterEnts):
        ogre.RenderTargetListener.__init__(self)
        self.planeEnt = planeEnt
        self.aboveWaterEnts = aboveWaterEnts
        
    def preRenderTargetUpdate(self,evt):
        #Hide plane and objects above the water
        self.planeEnt.visible = False
        for e in self.aboveWaterEnts:
            e.visible = False                
           
    def postRenderTargetUpdate(self,evt):
        #Show plane and objects above the water
        self.planeEnt.visible = True
        for e in self.aboveWaterEnts:
            e.visible = True                

# Reflection Render Target Listener
class ReflectionTextureListener(ogre.RenderTargetListener):

    def __init__(self,planeEnt,reflectionPlane,belowWaterEnts,cam):
        ogre.RenderTargetListener.__init__(self)
        self.planeEnt = planeEnt
        self.reflectionPlane = reflectionPlane        
        self.cam = cam
        self.belowWaterEnts = belowWaterEnts         
                
    def preRenderTargetUpdate(self,evt):
        #Hide plane and objects above the water
        self.planeEnt.visible = False
        for e in self.belowWaterEnts:
            e.visible = False                
        self.cam.enableReflection(self.reflectionPlane)
            
    def postRenderTargetUpdate(self,evt):
        #Show plane and objects above the water
        self.planeEnt.visible = True
        for e in self.belowWaterEnts:
            e.visible = True     
        self.cam.disableReflection()

class FresnelFrameListener(sf.FrameListener):

    def __init__(self, renderWindow, camera,fishNodes,fishAnimations,fishSplines):
        sf.FrameListener.__init__(self, renderWindow, camera)
        self.animTime = 0
        self.fishNodes = fishNodes
        self.renderWindow=renderWindow
        self.fishAnimations = fishAnimations
        self.fishSplines = fishSplines
        self.fishLastPosition = []        
        for fish in xrange(0,len(self.fishAnimations)):
            self.fishLastPosition.append(ogre.Vector3(0,0,0))

    
    def frameStarted(self, frameEvent):
        if(self.renderWindow.closed):
            return false    
        self.animTime += frameEvent.timeSinceLastFrame
        while (self.animTime > FISH_PATH_LENGTH):
            self.animTime -= FISH_PATH_LENGTH
        for fish in xrange(0,len(self.fishAnimations)):
            #Animate the fish
            self.fishAnimations[fish].addTime(frameEvent.timeSinceLastFrame*2)
            #Move the fish
            newPos = self.fishSplines[fish].interpolate(self.animTime / FISH_PATH_LENGTH*1.0)
            self.fishNodes[fish].position=newPos
            #Work out the direction
            direction = self.fishLastPosition[fish] - newPos
            direction.normalise
			#Test for opposite vectors
            d = 1.0 + ogre.Vector3.UNIT_X.dotProduct(direction)
            if (math.fabs(d) < 0.00001):
                #Diametrically opposed vectors
                orientation = ogre.Quaternion.IDENTITY
                orientation.FromAxes(ogre.Vector3.NEGATIVE_UNIT_X, 
					ogre.Vector3.UNIT_Y, ogre.Vector3.NEGATIVE_UNIT_Z)
                self.fishNodes[fish].orientation = orientation
            else:
                self.fishNodes[fish].orientation=ogre.Vector3.UNIT_X.getRotationTo(direction)
            self.fishLastPosition[fish] = newPos
        return sf.FrameListener.frameStarted(self,frameEvent)

class FresnelApplication(sf.Application):

    def __init__(self):
        "Init Fresnel Application"
        self.frameListener = None
        self.root = None
        self.camera = None
        self.renderWindow = None
        self.sceneManager = None
        self.refractionListener = None
        self.reflectionTextureListener=None        
        self.reflectionPlane = None
        self.planeEnt = None
        self.belowWaterEnts = []        
        self.aboveWaterEnts = []        
        self.fishNodes = []
        self.fishAnimations = []
        self.fishSplines = []
        self.myManualLoader=None

    def __del__(self):
        "Clear variables, this should not actually be needed."
        del self.camera
        del self.sceneManager
        del self.rendtargetlistener
        del self.frameListener
        del self.root
        del self.renderWindow
		
    def debugSupportedFormats(self):
        "Debug Card Capabilities"
        txMan = ogre.TextureManager.getSingleton()
        print " ----------------- Formats ---------------------------------" 
        print "Is Supported",txMan.isFormatSupported(ogre.TEX_TYPE_2D, ogre.PF_R8G8B8, ogre.TU_RENDERTARGET)
        print "Is Equivalent Supported",  txMan.isEquivalentFormatSupported(ogre.TEX_TYPE_2D, ogre.PF_R8G8B8, ogre.TU_RENDERTARGET)
        print "Is Equivalent Supported",  txMan.isEquivalentFormatSupported(ogre.TEX_TYPE_2D, ogre.PF_R8G8B8, ogre.TU_RENDERTARGET)
        print "Pixel Format Native Support", txMan.getNativeFormat(ogre.TEX_TYPE_2D, ogre.PF_R8G8B8, ogre.TU_RENDERTARGET)
        print "Value is",ogre.PF_R8G8B8
        print "Value is",ogre.TEX_TYPE_2D
        print "Value is",ogre.TU_RENDERTARGET
        print "Value is",ogre.TU_STATIC, ogre.TU_DYNAMIC,ogre.TU_RENDERTARGET,0x200
        print " ----------------- End Formats ---------------------------------" 
		
    def _createScene(self):
        "Override sf create scene"
        self.myManualLoader=MyManualLoader()
        sceneManager = self.sceneManager
        camera = self.camera

        # check graphics card capabilities                        
        capabilities = ogre.Root.getSingleton().getRenderSystem().getCapabilities()
        if not capabilities.hasCapability(ogre.RSC_VERTEX_PROGRAM) or not capabilities.hasCapability(ogre.RSC_FRAGMENT_PROGRAM):
            raise ogre.Exception(111, 'Your card does not support vertex and fragment programs, so cannot run this demo. Sorry!', 'fresneldemo.py')
        
        else:
            if (ogre.GpuProgramManager.getSingleton().isSyntaxSupported("arbfp1") and
                ogre.GpuProgramManager.getSingleton().isSyntaxSupported("ps_2_0") and
				ogre.GpuProgramManager.getSingleton().isSyntaxSupported("ps_1_4")):
				
                raise ogre.Exception(111, 'Your card does not support advanced fragment programs, so cannot run this demo. Sorry!', 'fresneldemo.py')

        theCam = self.camera
        self.camera.position = -100,20,700
        #Set ambient light
        sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)

        #Create a point light
        l = sceneManager.createLight("MainLight")
        l.type = ogre.Light.LT_DIRECTIONAL
        l.direction = -ogre.Vector3.UNIT_Y

        # debug graphics card capabilities                        
        self.debugSupportedFormats()    
        	             
        # create Refraction Render Target
        mTexture = ogre.TextureManager.getSingleton().createManual( 'Refraction', 
			'General', ogre.TEX_TYPE_2D, 
			512, 512, 0, ogre.PF_R8G8B8, ogre.TU_RENDERTARGET,self.myManualLoader) 
			#, self.myManualLoader )
       
        #RenderTarget 
        self.rttTex = mTexture.getRenderTarget()
        #Viewport 
        v = self.rttTex.addViewport( self.camera )
        
        #MaterialPtr 
        self.mat = ogre.MaterialPointer(ogre.MaterialManager.getSingleton().getByName("Examples/FresnelReflectionRefraction"))
        self.mat.getTechnique(0).getPass(0).getTextureUnitState(2).setTextureName("Refraction")
        v.overlaysEnabled=False
        
        # create Reflection Render Target
        #TexturePtr 
        mTexture = ogre.TextureManager.getSingleton().createManual( "Reflection", 
			'General', ogre.TEX_TYPE_2D, 
			512, 512, 0, ogre.PF_R8G8B8, ogre.TU_RENDERTARGET,self.myManualLoader )
        #RenderTarget 
        self.rttTex2 = mTexture.getRenderTarget()
        #Viewport 
        v = self.rttTex2.addViewport( self.camera )
        #MaterialPtr 
        self.mat2 = ogre.MaterialPointer(ogre.MaterialManager.getSingleton().getByName("Examples/FresnelReflectionRefraction"))
        self.mat2.getTechnique(0).getPass(0).getTextureUnitState(1).setTextureName("Reflection")
        v.overlaysEnabled=False

        #My node to which all objects will be attached
        self.rootNode = sceneManager.rootSceneNode.createChildSceneNode()

        #Define a floor plane mesh
        self.reflectionPlane = ogre.Plane()
        self.reflectionPlane.normal = ogre.Vector3.UNIT_Y
        self.reflectionPlane.d = 0

        ogre.MeshManager.getSingleton().createPlane('ReflectPlane', "General",
                                                    self.reflectionPlane, 1500, 1500,
                                                    10, 10, True, 1, 5, 5,
                                                    (0, 0, 1))
        
        self.planeEnt = sceneManager.createEntity( "plane", "ReflectPlane" )
        self.planeEnt.setMaterialName("Examples/FresnelReflectionRefraction")
        
        
        sceneManager.rootSceneNode.createChildSceneNode().attachObject(self.planeEnt)
        
        sceneManager.setSkyBox(True, "Examples/CloudyNoonSkyBox")
                       
        # create above Water Entities
        for entity_name in "head1 Pillar1 Pillar2 Pillar3 Pillar4 UpperSurround".split():
            pEnt = self.sceneManager.createEntity( entity_name, entity_name + ".mesh" )
            self.rootNode.attachObject(pEnt)
            self.aboveWaterEnts.append(pEnt)

        # create below Water Entities
        for entity_name in "LowerSurround PoolFloor".split():
            pEnt = self.sceneManager.createEntity( entity_name, entity_name + ".mesh" )
            self.rootNode.attachObject(pEnt)
            self.belowWaterEnts.append(pEnt)
                
        # create fish
        for fish in xrange(0,NUM_FISH):
            pEnt = self.sceneManager.createEntity("fish" + ogre.StringConverter.toString(fish), "fish.mesh")
            node = self.rootNode.createChildSceneNode()
            amimstate = pEnt.getAnimationState("swim")
            amimstate.enabled = True
            node.attachObject(pEnt)
            self.fishSplines.append(ogre.SimpleSpline())
            self.fishNodes.append(node)                # nodes
            self.fishAnimations.append(amimstate)      # animation states  
            self.belowWaterEnts.append(pEnt)           # entities 
            self.fishSplines[fish].autoCalculate=False
            lastPos = ogre.Vector3(0.0,0.0,0.0)
            for waypoint in xrange(0,NUM_FISH_WAYPOINTS):
                pos = ogre.Vector3(ogre.Math.SymmetricRandom() * 700.0, -10.0, ogre.Math.SymmetricRandom() * 700.0)
                if (waypoint > 0):
                    #check this waypoint isn't too far, we don't want turbo-fish ;)
                    #since the waypoints are achieved every 5 seconds, half the length
                    #of the pond is ok
                    while ((lastPos - pos).length > 750.0):
                        pos = ogre.Vector3(ogre.Math.SymmetricRandom() * 700.0, -10.0, ogre.Math.SymmetricRandom() * 700.0)
                self.fishSplines[fish].addPoint(pos)
                lastPos = pos
                #close the spline
            self.fishSplines[fish].addPoint(self.fishSplines[fish].getPoint(0))
            #recalc
            self.fishSplines[fish].recalcTangents()


        # create Render Target Listener's
        self.reflectionListener = ReflectionTextureListener(self.planeEnt,self.reflectionPlane,self.belowWaterEnts,self.camera)
        self.rttTex2.addListener(self.reflectionListener)   
        self.refractionListener = RefractionTextureListener(self.planeEnt,self.aboveWaterEnts)
        self.rttTex.addListener(self.refractionListener)
          		
    def _createFrameListener(self):
        "create FrameListener"
        self.frameListener = FresnelFrameListener(self.renderWindow, self.camera,
                                                                   self.fishNodes,self.fishAnimations,self.fishSplines)
        self.root.addFrameListener(self.frameListener)
        
if __name__ == '__main__':

    try:
        application = FresnelApplication()
        application.go()
    except ogre.Exception, e:
        print e            
            


