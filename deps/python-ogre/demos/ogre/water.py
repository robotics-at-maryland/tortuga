# /*
# -----------------------------------------------------------------------------
# This source file is part of OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http:##www.ogre3d.org/

# Copyright (c) 2000-2006 Torus Knot Software Ltd
# Also see acknowledgements in Readme.html

# You may use self sample code for anything you like, it is not covered by the
# LGPL like the rest of the engine.
# -----------------------------------------------------------------------------
# */
# /* Static water simulation by eru
#  * Started 29.05.2003, 20:54:37
#  */

#include "ExampleApplication.h"
import Ogre
import WaterMesh
import math
import SampleFramework as sf
import random

AnimationState* self.mAnimState 

## Mesh stuff
MESH_NAME ="self.WaterMesh"
ENTITY_NAME ="self.waterEntity"
MATERIAL_PREFIX ="Examples/Water"
MATERIAL_NAME ="Examples/Water0"
COMPLEXITY =64      ## watch out - number of polys is 2*ACCURACY*ACCURACY !
PLANE_SIZE =3000.0

CIRCLES_MATERIAL ="Examples/Water/Circles"

# /* Some global variables */

#   MeshPtr mesh ;
#   SubMesh *subMesh ; 
#   float *vertexBuffers[3] ; // we need 3 vertex buffers
#   int currentBuffNumber ;
#   int complexity ;
#   String meshName ;
#   int numFaces ;
#   int numVertices ;
#   Vector3* vNormals ;

#   HardwareVertexBufferSharedPtr posVertexBuffer ;
#   HardwareVertexBufferSharedPtr normVertexBuffer ;
#   HardwareVertexBufferSharedPtr texcoordsVertexBuffer ;
#   HardwareIndexBufferSharedPtr indexBuffer ;

#   Real lastTimeStamp ;
#   Real lastAnimationTimeStamp;
#   Real lastFrameTime ;

#   bool useFakeNormals ;




headNode  = None
waterOverlay  = None
particleSystem  = None
particleEmitter  = None
sceneMgr  = None

##
## Note that this function makes use of CTypes and def ptr casting to access Ogre Library functions
##
def prepareCircleMaterial():
    global CIRCLES_MATERIAL
    
    storageclass = ctypes.c_float * (256 * 256 * 4)
    bmap=storageclass(1.1) # you just need to put some value in this to get it started...
    ctypes.memset ( bmap, 127, 256 * 256 * 4 )
    bmap[22]=1
    bmap[256]=1.445
        
    for  b in range(16):
        x0 = b % 4  
        y0 = b >> 2  
        radius = 4.0 + 1.4 * b  
        for x in range(64):
            for y in range (64) :
                dist = math.sqrt((x-32)*(x-32)+(y-32)*(y-32))  ## 0..ca.45
                dist = math.fabs(dist-radius-2) / 2.0  
                dist = dist * 255 
                if (dist>255):
                    dist=255  
                colour = 255-dist  
                colour = ((15-b))/15.0 * colour 

                bmap[4*(256*(y+64*y0)+x+64*x0)+0]=colour  
                bmap[4*(256*(y+64*y0)+x+64*x0)+1]=colour  
                bmap[4*(256*(y+64*y0)+x+64*x0)+2]=colour  
                bmap[4*(256*(y+64*y0)+x+64*x0)+3]=colour  
                
    ### Need to convert bmap to ctype array... then pass the address to MemoryDataStream
    ##  
    imgstream = ogre.MemoryDataStream(ogre.CastVoidPtr(ctypes.addressof(bmap)),  (256 * 256 * 4 ) )  
    
    ogre.TextureManager.getSingleton().loadRawData(CIRCLES_MATERIAL,
        ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
        imgstream, 256, 256  , ogre.PixelFormat.PF_A8R8G8B8 ) 
                    
    material = ogre.MaterialManager.getSingleton().create( CIRCLES_MATERIAL,
        ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
    texLayer = material.getTechnique(0).getPass(0).createTextureUnitState( CIRCLES_MATERIAL ) 
    texLayer.setTextureAddressingMode( ogre.TextureUnitState.TAM_CLAMP ) 
    material.setSceneBlending( ogre.SceneBlendType.SBT_ADD ) 
    material.setDepthWriteEnabled( False )  
    material.load() 



# /* =========================================================================*/
# /*               WaterCircle class                                          */
# /* =========================================================================*/
CIRCLE_SIZE = 500.0
CIRCLE_TIME = 0.5
class WaterCircle:
    def __init_(self):
        self.name = ""
#         SceneNode *node  
#         MeshPtr mesh  
#         SubMesh *subMesh  
#         Entity *entity  
#         Real tm  
#         static bool first  
#         ## some buffers shared by all circles
#         static HardwareVertexBufferSharedPtr posnormVertexBuffer  
#         static HardwareIndexBufferSharedPtr indexBuffer   ## indices for 2 faces
#         static HardwareVertexBufferSharedPtr *texcoordsVertexBuffers  
    
#         float *texBufData 
        _prepareMesh()
    
        mesh = Ogre.MeshManager.getSingleton().createManual(name,
                Ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME)  
        subMesh = mesh.createSubMesh() 
        subMesh.useSharedVertices=False 
        numVertices = 4  
        first = True
        
        if (first) : ## first Circle, create some static common data:
            first = False  
    
            ## static buffer for position and normals
            size = 2 * ogre.VertexElement.getTypeSize(ogre.VertexElementType.VET_FLOAT3)    ## 6 * float
            posnormVertexBuffer = \
                Ogre.HardwareBufferManager.getSingleton().createVertexBuffer(
                    size, ## size of one vertex data 6 * float
                    4, ## number of vertices
                    Ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY, ## usage
                    False)  ## no shadow buffer
            posnormBufData = posnormVertexBuffer.lock(Ogre.HardwareBuffer.HBL_DISCARD) 
            
            buff=[]
            for i in range(numVertices):
                buf.append(((i%2)-0.5)*CIRCLE_SIZE ) ## pos X
                buf.append(0)  ## pos Y
                buf.append(((i/2)-0.5)*CIRCLE_SIZE ) ## pos Z
                buf.append(0)   ## normal X
                buf.append(1)   ## normal Y
                buf.append(0)   ## normal Z
            ogre.setFloat( posnormBufData , buff )    # write unsigned ints...
            posnormVertexBuffer.unlock() 

            ## static buffers for 16 sets of texture coordinates
            texcoordsVertexBuffers = [] 
            for lvl in range (16):
                texcoordsVertexBuffers.append(
                    Ogre.HardwareBufferManager.getSingleton().createVertexBuffer(
                        Ogre.VertexElement.getTypeSize(ogre.VertexElementType.VET_FLOAT2), ## size of one vertex data
                        numVertices, ## number of vertices
                        Ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY, ## usage
                        False)  ## no shadow buffer
                        )
                texcoordsBufData = texcoordsVertexBuffers[lvl].lock(Ogre.HardwareBuffer.HBL_DISCARD) 
                x0 = (lvl % 4) * 0.25  
                y0 = (lvl / 4) * 0.25  
                y0 = 0.75-y0   ## upside down
                for i in range (4):
                    texcoordsBufData[i*2 + 0]= \
                        x0 + 0.25 * (i%2)  
                    texcoordsBufData[i*2 + 1]= \
                        y0 + 0.25 * (i/2)  

                texcoordsVertexBuffers[lvl].unlock() 


            ## Index buffer for 2 faces
            faces[6] = (2,1,0,  2,3,1)
            indexBuffer = \
                Ogre.HardwareBufferManager.getSingleton().createIndexBuffer(
                    Ogre.HardwareIndexBuffer.IT_16BIT,
                    6,
                    Ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY) 
            indexBuffer.writeData(0,
                indexBuffer.getSizeInBytes(),
                faces,
                True)  ## true?

    
            ## Initialize vertex data
            subMesh.vertexData = Ogre.VertexData() 
            subMesh.vertexData.vertexStart = 0 
            subMesh.vertexData.vertexCount = 4 
            ## first, set vertex buffer bindings
            vbind = subMesh.vertexData.vertexBufferBinding  
            vbind.setBinding(0, posnormVertexBuffer) 
            vbind.setBinding(1, texcoordsVertexBuffers[0]) 
            ## now, set vertex buffer declaration
            vdecl = subMesh.vertexData.vertexDeclaration  
            vdecl.addElement(0, 0, Ogre.VET_FLOAT3, VOgre.ES_POSITION) 
            vdecl.addElement(0, 3*4, Ogre.VET_FLOAT3, Ogre.VES_NORMAL) 
            vdecl.addElement(1, 0, Ogre.VET_FLOAT2, Ogre.VES_TEXTURE_COORDINATES) 
    
            ## Initialize index data
            subMesh.indexData.indexBuffer = indexBuffer 
            subMesh.indexData.indexStart = 0 
            subMesh.indexData.indexCount = 6 
    
            ## set mesh bounds
            circleBounds(-CIRCLE_SIZE/2.0, 0, -CIRCLE_SIZE/2.0,
                CIRCLE_SIZE/2.0, 0, CIRCLE_SIZE/2.0) 
            mesh._setBounds(circleBounds) 
            mesh.load() 
            mesh.touch() 

    def setTextureLevel():
        subMesh.vertexData.vertexBufferBinding.setBinding(1, texcoordsVertexBuffers[lvl]) 

    def WaterCircle( name,  x,  y):

        self.name = name  
        _prepareMesh() 
        node = sceneMgr.getRootSceneNode().createChild(name)
        node.translate(x*(PLANE_SIZE/COMPLEXITY), 10, y*(PLANE_SIZE/COMPLEXITY)) 
        entity = sceneMgr.createEntity(name, name) 
        entity.setMaterialName(CIRCLES_MATERIAL) 
        node.attachObject(entity) 
        tm = 0  
        lvl = 0  
        setTextureLevel() 

    def __del__(self, ):
        Ogre.MeshManager.getSingleton().remove(mesh.getHandle()) 
        sceneMgr.destroyEntity(entity.getName()) 
        sceneMgr.getRootSceneNode().removeChildnode.getName()

    def animate(self, timeSinceLastFrame):
        lastlvl = lvl  
        tm += timeSinceLastFrame  
        lvl = (int) ( (Real)(tm)/CIRCLE_TIME * 16 ) 
        if (lvl<16 and lvl!=lastlvl) :
            setTextureLevel() 


    def clearStaticBuffers(self):
        posnormVertexBuffer = Ogre.HardwareVertexBufferSharedPtr()  
        indexBuffer = Ogre.HardwareIndexBufferSharedPtr()  
        for i in range (16):
            texcoordsVertexBuffers[i] = Ogre.HardwareVertexBufferSharedPtr()  

        del texcoordsVertexBuffers 


# bool WaterCircle.first = True  
# HardwareVertexBufferSharedPtr WaterCircle.posnormVertexBuffer =
#     HardwareVertexBufferSharedPtr()  
# HardwareIndexBufferSharedPtr WaterCircle.indexBuffer =
#     HardwareIndexBufferSharedPtr()  
# HardwareVertexBufferSharedPtr* WaterCircle.texcoordsVertexBuffers = 0  

# /* =========================================================================*/
# /*               WaterListener class                                          */
# /* =========================================================================*/
## Event handler
class WaterListener(sf.FrameListener):

# # # protected:
# # #     self.WaterMesh = none  
# # #     Entity *self.waterEntity  
# # #     int materialNumber  
# # #     bool skyBoxOn  
# # #     Real timeoutDelay  

# # # #define RAIN_HEIGHT_RANDOM 5
# # # #define RAIN_HEIGHT_CONSTANT 5
# #         self.sines[4] =(0,100,200,300) 
# #         self.adds[4] = (0.3,-1.6,1.1,0.5) 
# #         self.oldPos = Vector3.UNIT_Z 
# #         self.pindex = 0  
# #         self.headDepth  =0.0
       

# # # #     typedef std.vector<WaterCircle*> WaterCircles  
# # # #     WaterCircles circles  

    def processCircles(self,timeSinceLastFrame):

        for  i in range (circles.size() ) :
            circles[i].animate(timeSinceLastFrame) 

# # #         do :
# # #             found = False  
# # #             for(WaterCircles.iterator it = circles.begin()  
# # #                     it != circles.end() 
# # #                     ++it) {
# # #                 if ((*it).lvl>=16) {:
# # #                     delete (*it) 
# # #                     circles.erase(it) 
# # #                     found = True  
# # #                     break  


# # #         while (found)  


    def processParticles(self):
        global PLANE_SIZE, COMPLEXITY, RAIN_HEIGHT_RANDOM, RAIN_HEIGHT_CONSTANT
        pit = Ogre.particleSystem._getIterator()  
        while not pit.end():
            particle = pit.getNext() 
            ppos = particle.position 
            if ppos.y<=0 and particle.timeToLive>0 : ## hits the water!:
                ## delete particle
                particle.timeToLive = 0.0 
                ## push the water
                x = ppos.x / PLANE_SIZE * COMPLEXITY  
                y = ppos.z / PLANE_SIZE * COMPLEXITY  
                h = random.random() % RAIN_HEIGHT_RANDOM + RAIN_HEIGHT_CONSTANT  
                if (x<1): 
                    x=1 
                if (x>COMPLEXITY-1):
                    x=COMPLEXITY-1 
                if (y<1):
                     y=1 
                if (y>COMPLEXITY-1):
                    y=COMPLEXITY-1
                self.WaterMesh.push(x,y,-h)  
                circle = WaterCircle(
                    "Circle#"+Ogre.StringConverter.toString(self.pindex),
                    x, y)
                self.pindex+=1 
                circles.push_back(circle) 




    ## Head animation */
    def animateHead(self, timeSinceLastFrame):

        ## sine track? :)
        for i in range(4):
            self.sines[i]+=self.adds[i]*timeSinceLastFrame 

        tx = ((sin(self.sines[0]) + sin(self.sines[1])) / 4 + 0.5 ) * (COMPLEXITY-2) + 1  
        ty = ((sin(self.sines[2]) + sin(self.sines[3])) / 4 + 0.5 ) * (COMPLEXITY-2) + 1  
        self.WaterMesh.push(tx,ty, -self.headDepth) 
        step = PLANE_SIZE / COMPLEXITY  
        headNode.resetToInitialState() 
        headNode.scale(3,3,3) 
        newPos = Ogre.Vector3(step*tx, self.headDepth, step*ty) 
        diffPos = newPos - self.oldPos  
        headRotation = Vector3.UNIT_Z.getRotationTo(diffPos) 
        self.oldPos = newPos  
        headNode.translate(newPos) 
        headNode.rotate(headRotation) 


    ## GUI updaters
    def updateInfoParamC(self):
        Ogre.OverlayManager.getSingleton().getOverlayElement("Example/Water/Param_C") \
            .setCaption("[1/2]Ripple speed: "+Ogre.StringConverter.toString(self.WaterMesh.PARAM_C)) 

    def updateInfoParamD(self):
        Ogre.OverlayManager.getSingleton().getOverlayElement("Example/Water/Param_D") \
            .setCaption("[3/4]Distance: "+Ogre.StringConverter.toString(self.WaterMesh.PARAM_D)) 

    def updateInfoParamU(self):
        Ogre.OverlayManager.getSingleton().getOverlayElement("Example/Water/Param_U") \
            .setCaption("[5/6]Viscosity: "+Ogre.StringConverter.toString(self.WaterMesh.PARAM_U)) 

    def updateInfoParamT(self):
        Ogre.OverlayManager.getSingleton().getOverlayElement("Example/Water/Param_T") \
            .setCaption("[7/8]Frame time: "+Ogre.StringConverter.toString(self.WaterMesh.PARAM_T)) 

    def updateInfoNormals(self):
        Ogre.OverlayManager.getSingleton().getOverlayElement("Example/Water/Normals") \
            .setCaption(String("[N]Normals: ")+((self.WaterMesh.useFakeNormals)?"fake":"real")) 

    def switchNormals(self):
        self.WaterMesh.useFakeNormals =  not self.WaterMesh.useFakeNormals  
        updateInfoNormals()  

    def updateInfoself.headDepth(self):
        Ogre.OverlayManager.getSingleton().getOverlayElement("Example/Water/Depth") \
            .setCaption(String("[U/J]Head depth: ")+Ogre.StringConverter.toString(self.headDepth))
             
    def updateInfoSkyBox(self):
        Ogre.OverlayManager.getSingleton().getOverlayElement("Example/Water/SkyBox")
            .setCaption(String("[B]SkyBox: ")+String((skyBoxOn)?"On":"Off") ) 

    def updateMaterial(self):
        global MATERIAL_PREFIX
        materialName = MATERIAL_PREFIX+Ogre.StringConverter.toString(materialNumber) 
        material = Ogre.MaterialManager.getSingleton().getByName(materialName) 
        if (material.isNull()):
            if(materialNumber):
                materialNumber = 0  
                updateMaterial() 
                return  
            else:
                OGRE_EXCEPT(Exception.ERR_INTERNAL_ERROR,
                    "Material "+materialName+"doesn't exist!",
                    "WaterListener.updateMaterial") 

        self.waterEntity.setMaterialName(materialName) 
        Ogre.OverlayManager.getSingleton().getOverlayElement("Example/Water/Material") \
            .setCaption(String("[M]Material: ")+materialName) 


    def switchMaterial(self):
        materialNumber++ 
        updateMaterial() 

    def switchSkyBox(self):
        skyBoxOn = !skyBoxOn 
        sceneMgr.setSkyBox(skyBoxOn, "Examples/SceneSkyBox2") 
        updateInfoSkyBox() 


    def __init__(self, renderWindow, camera, WaterMesh, self.waterEntity ):
    
        sf.FrameListener.__init__(self, renderWindow, camera)
        self.camera = camera
        self.WaterMesh = WaterMesh  
        self.self.waterEntity = self.waterEntity  
        materialNumber = 8 
        timeoutDelay = 0.0 
        self.headDepth = 2.0 
        skyBoxOn = False  

        updateMaterial() 
        updateInfoParamC() 
        updateInfoParamD() 
        updateInfoParamU() 
        updateInfoParamT() 
        updateInfoNormals() 
        updateInfoself.headDepth() 
        updateInfoSkyBox() 


    def __del__ ( self ):
        ## If when you finish the application is still raining there
        ## are water circles that are still being processed
        activeCircles = self.circles.size () 

        ## Kill the active water circles
        for i in range ( activeCircles ):
            del (self.circles[i]) 


    def frameStarted(self, frameEvent):
        result = sf.FrameListener.frameStarted(self,frameEvent)

        if( result == False ):
            ## check if we are exiting, if so, clear static HardwareBuffers to adef segfault
            WaterCircle.clearStaticBuffers() 
            return False 

        self.mAnimState.addTime(evt.timeSinceLastFrame) 

        ## process keyboard events
        changeSpeed = evt.timeSinceLastFrame  

        ## adjust keyboard speed with SHIFT (increase) and CONTROL (decrease)
        if (self.Keyboard.isKeyDown(OIS.KC_LSHIFT) or self.Keyboard.isKeyDown(OIS.KC_RSHIFT)):
            changeSpeed *= 10.0  

        if (self.Keyboard.isKeyDown(OIS.KC_LCONTROL)):
            changeSpeed /= 10.0  

        ## rain
        processCircles(evt.timeSinceLastFrame) 
        if (self.Keyboard.isKeyDown(OIS.KC_SPACE)):
            particleEmitter.setEmissionRate(20.0) 
        else:
            particleEmitter.setEmissionRate(0.0) 

        processParticles() 

        ## adjust values (some macros for faster change
        def ADJUST_RANGE(_value,_plus,_minus,_minVal,_maxVal,_change,_macro):
            if (self.Keyboard.isKeyDown(_plus)):
                _value+=_change
                if (_value>=_maxVal):
                     _value = _maxVal
                _macro 
            if (self.Keyboard.isKeyDown(_minus)):
                _value-=_change  
                if (_value<=_minVal):
                     _value = _minVal
                _macro ; }


        ADJUST_RANGE(headDepth, OIS.KC_U, OIS.KC_J, 0, 10, 0.5*changeSpeed, updateInfoHeadDepth()) 

        ADJUST_RANGE(self.WaterMesh.PARAM_C, OIS.KC_2, OIS.KC_1, 0, 10, 0.1*changeSpeed, updateInfoParamC())  

        ADJUST_RANGE(self.WaterMesh.PARAM_D, OIS.KC_4, OIS.KC_3, 0.1, 10, 0.1*changeSpeed, updateInfoParamD())  

        ADJUST_RANGE(self.WaterMesh.PARAM_U, OIS.KC_6, OIS.KC_5, -2, 10, 0.1*changeSpeed, updateInfoParamU())  

        ADJUST_RANGE(self.WaterMesh.PARAM_T, OIS.KC_8, OIS.KC_7, 0, 10, 0.1*changeSpeed, updateInfoParamT())  

        timeoutDelay-=evt.timeSinceLastFrame  
        if (timeoutDelay<=0):
            timeoutDelay = 0 

        def SWITCH_VALUE(_key,_timeDelay, _macro):
            if (self.Keyboard.isKeyDown(_key) and timeoutDelay==0) :
                timeoutDelay = _timeDelay   
                _macro 

        SWITCH_VALUE(KC_N, 0.5, switchNormals()) 

        SWITCH_VALUE(KC_M, 0.5, switchMaterial()) 

        SWITCH_VALUE(KC_B, 0.5, switchSkyBox()) 

        animateHead(evt.timeSinceLastFrame) 

        self.WaterMesh.updateMesh(evt.timeSinceLastFrame) 

        return True 



class WaterApplication(sf.Application,ogre.RenderTargetListener):
    
## Just override the mandatory create scene method
    def _createScene(void)
        global PLANE_SIZE, MESH_NAME, COMPLEXITY, ENTITY_NAME
        sceneManager = self.sceneManager
        camera = self.camera
        ## Set ambient light
        sceneManager.setAmbientLight(ColourValue(0.75, 0.75, 0.75)) 

        ## Create a light
        l = sceneManager.createLight("MainLight") 
        ## Accept default settings: point light, white diffuse, just set position
        l.setPosition(200,300,100) 

        ## Create water mesh and entity
        self.WaterMesh = WaterMesh.WaterMesh(MESH_NAME, PLANE_SIZE, COMPLEXITY) 
        self.waterEntity = sceneManager.createEntity(ENTITY_NAME, MESH_NAME) 
        
        ## self.waterEntity.setMaterialName(MATERIAL_NAME) 
        waterNode = sceneManager.getRootSceneNode().createChildSceneNode() 
        waterNode.attachObject(self.waterEntity) 

        ## Add a head, give it it's own node
        headNode = waterNode.createChildSceneNode() 
        ent = sceneManager.createEntity("head", "ogrehead.mesh") 
        headNode.attachObject(ent) 

        ## Make sure the camera track self node
        ## self.camera.setAutoTracking(True, headNode) 

        ## Create the camera node, set its position & attach camera
        camNode = sceneManager.getRootSceneNode().createChildSceneNode() 
        camNode.translate(0, 500, PLANE_SIZE) 
        camNode.yaw(Ogre.Degree(-45)) 
        camNode.attachObject(self.camera) 

        ## Create light node
        lightNode = sceneManager.getRootSceneNode().createChildSceneNode() 
        lightNode.attachObject(l) 

        ## set up spline animation of light node
        anim = sceneManager.createAnimation("WaterLight", 20) 
         
        ## create a random spline for light
        track = anim.createNodeTrack(0, lightNode) 
        key = track.createNodeKeyFrame(0) 
        for  ff in range (1,20):
            key = track.createNodeKeyFrame(ff) 
            lpos Ogre.Vector3(
                random.random()%PLANE_SIZE , ##- PLANE_SIZE/2,
                random.random()%300+100,
                random.random()%PLANE_SIZE ##- PLANE_SIZE/2
                ) 
            key.setTranslate(lpos) 

        key = track.createNodeKeyFrame(20) 

        ## Create a new animation state to track self
        self.mAnimState = sceneManager.createAnimationState("WaterLight") 
        self.mAnimState.setEnabled(True) 

        ## Put in a bit of fog for the hell of it
        ##sceneManager.setFog(FOG_EXP, ColourValue.White, 0.0002) 

        ## show overlay
        waterOverlay = Ogre.OverlayManager.getSingleton().getByName("Example/WaterOverlay") 
        waterOverlay.show() 

        ## Let there be rain
        particleSystem = sceneManager.createParticleSystem("rain",
            "Examples/Water/Rain") 
        particleEmitter = particleSystem.getEmitter(0) 
        rNode = sceneManager.getRootSceneNode().createChildSceneNode() 
        rNode.translate(PLANE_SIZE/2.0, 3000, PLANE_SIZE/2.0f) 
        rNode.attachObject(particleSystem) 
        ## Fast-forward the rain so it looks more natural
        particleSystem.fastForward(20) 
        ## It can't be set in .particle file, and we need it  )
#         static_cast<BillboardParticleRenderer*>(particleSystem.getRenderer()).setBillboardOrigin(BBO_BOTTOM_CENTER) 

        prepareCircleMaterial() 


    ## Create new frame listener
    def _createFrameListener(void)
        self.frameListener = WaterListener(self.renderWindow, self.camera,
                                                                   self.WaterMesh, self.waterEntity)
        self.root.addFrameListener(self.frameListener)

 

if __name__ == '__main__':
    try:
        application = WaterApplication()
        application.go()
    except ogre.Exception, e:
        print e

