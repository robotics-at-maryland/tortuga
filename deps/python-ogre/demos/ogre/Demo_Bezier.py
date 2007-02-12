
# 
# #     \file 
# #         Bezier.h
# #     \brief
# #         Specialisation of OGRE's framework application to show off
# #         the bezier patch support.
# 

import Ogre as ogre
import SampleFramework as sf
import ctypes, math

##patch = ogre.PatchMesh()
##Pass = ogre.Pass()

## Event handler to add ability to alter subdivision
class BezierListener(sf.FrameListener):
    def __init__(self, renderWindow, camera ):
        self.timeLapse = 0.0 
        self.factor = 0.0 
        self.wireframe = 0 
        self.renderWindow = renderWindow
        sf.FrameListener.__init__(self, renderWindow, camera)
        
    def frameStarted(self, frameEvent):
        global patchPass, patch
        if( sf.FrameListener.frameStarted(self, frameEvent) == False ):
            return False 
        self.timeLapse += frameEvent.timeSinceLastFrame 
        ## Prgressively grow the patch
        if (self.timeLapse > 1.0):
            self.factor += 0.2 
            if (self.factor > 1.0) :
                self.wireframe = not self.wireframe 
                ##self.camera.setPolygonMode(wireframe ? PM_WIREFRAME : PM_SOLID) 
                if self.wireframe:
                    patchPass.setPolygonMode(ogre.PolygonMode.PM_WIREFRAME )
                else:
                    patchPass.setPolygonMode(ogre.PolygonMode.PM_SOLID)
                self.factor = 0.0 
            patch.setSubdivision(self.factor) 
            sf.Application.debugText  = "Bezier subdivision factor: " + str(self.factor)
            self.timeLapse = 0.0 

        ## Call default
        return True 


class BezierApplication(sf.Application):
    def __init__(self):
        sf.Application.__init__(self)
        patchDecl = ogre.VertexDeclaration()
        self.patchCtlPoints =0
        
    def __del__(self):
        global pVert
        if (self.patchCtlPoints):
            del self.patchCtlPoints
        del pVert
        sf.Application.__del__(self) 

           
    def _createScene(self):
        global patchPass, patch, pVert
        sceneManager = self.sceneManager

        ## Set ambient light
        sceneManager.setAmbientLight( (0.2, 0.2, 0.2) ) 

        ## Create a point light
        l = sceneManager.createLight("MainLight") 
        l.setType(ogre.Light.LT_DIRECTIONAL) 
        l.setDirection(-0.5, -0.5, 0) 

        ## Create patch - here we are creating a memoryblock that is 8 floats (x,y,z,nx,ny,nz,u,v)
        ## Notice how we increment the offset for each new element
        offset = 0
        
        patchDecl = ogre.HardwareBufferManager.getSingleton().createVertexDeclaration() 
        patchDecl.addElement(0, 0, ogre.VertexElementType.VET_FLOAT3, ogre.VertexElementSemantic.VES_POSITION)
        offset += ogre.VertexElement.getTypeSize(ogre.VertexElementType.VET_FLOAT3) 

        patchDecl.addElement(0, offset, ogre.VertexElementType.VET_FLOAT3, ogre.VertexElementSemantic.VES_NORMAL) 
        offset += ogre.VertexElement.getTypeSize(ogre.VertexElementType.VET_FLOAT3) 

        patchDecl.addElement(0, offset, ogre.VertexElementType.VET_FLOAT2, ogre.VertexElementSemantic.VES_TEXTURE_COORDINATES, 0) 
        offset += ogre.VertexElement.getTypeSize(ogre.VertexElementType.VET_FLOAT2) 

        ## Make a patch for test - there are 9 'patches, each with 8 floats as above
        storageclass = ctypes.c_float * (8*9)
        pVert=storageclass(1.1)
        ctypes.memset ( pVert, 0, 8*9 )      # just showing how this can be done

         ## Patch data
        inc = 0
        pVert[inc+0] = -500.0
        pVert[inc+1] = 200.0
        pVert[inc+2] = -500.0
        pVert[inc+3] = -0.5  
        pVert[inc+4] = 0.5 
        pVert[inc+5] = 0.0
        pVert[inc+6] = 0.0  
        pVert[inc+7] = 0.0
        inc=8
        pVert[inc+0] = 0.0  
        pVert[inc+1] = 500.0 
        pVert[inc+2] = -750.0
        pVert[inc+3] = 0.0  
        pVert[inc+4] = 0.5 
        pVert[inc+5] = 0.0
        pVert[inc+6] = 0.5  
        pVert[inc+7] = 0.0
        inc = 16 
        pVert[inc+0] = 500.0  
        pVert[inc+1] = 1000.0 
        pVert[inc+2] = -500.0
        pVert[inc+3] = 0.5  
        pVert[inc+4] = 0.5 
        pVert[inc+5] = 0.0
        pVert[inc+6] = 1.0  
        pVert[inc+7] = 0.0
        inc = 24 
        pVert[inc+0] = -500.0  
        pVert[inc+1] = 0.0 
        pVert[inc+2] = 0.0
        pVert[inc+3] = -0.5  
        pVert[inc+4] = 0.5 
        pVert[inc+5] = 0.0
        pVert[inc+6] = 0.0  
        pVert[inc+7] = 0.5
        inc = 32
        pVert[inc+0] = 0.0  
        pVert[inc+1] = 500.0 
        pVert[inc+2] = 0.0
        pVert[inc+3] = 0.0  
        pVert[inc+4] = 0.5 
        pVert[inc+5] = 0.0
        pVert[inc+6] = 0.5  
        pVert[inc+7] = 0.5
        inc = 40 
        pVert[inc+0] = 500.0  
        pVert[inc+1] = -50.0 
        pVert[inc+2] = 0.0
        pVert[inc+3] = 0.5  
        pVert[inc+4] = 0.5 
        pVert[inc+5] = 0.0
        pVert[inc+6] = 1.0  
        pVert[inc+7] = 0.5
        inc = 48
        pVert[inc+0] = -500.0  
        pVert[inc+1] = 0.0 
        pVert[inc+2] = 500.0
        pVert[inc+3] = -0.5  
        pVert[inc+4] = 0.5 
        pVert[inc+5] = 0.0
        pVert[inc+6] = 0.0  
        pVert[inc+7] = 1.0
        inc = 56 
        pVert[inc+0] = 0.0  
        pVert[inc+1] = 500.0 
        pVert[inc+2] = 500.0
        pVert[inc+3] = 0.0  
        pVert[inc+4] = 0.5 
        pVert[inc+5] = 0.0
        pVert[inc+6] = 0.5  
        pVert[inc+7] = 1.0
        inc = 64 
        pVert[inc+0] = 500.0  
        pVert[inc+1] = 200.0 
        pVert[inc+2] = 800.0
        pVert[inc+3] = 0.5  
        pVert[inc+4] = 0.5 
        pVert[inc+5] = 0.0
        pVert[inc+6] = 1.0  
        pVert[inc+7] = 1.0

        ##
        ## NOTE: Python-Ogre Special :)
        ## Function (createBezierPatch) requires a void * to a buffer.  The buffer we created with Ctypes
        ## and we convert the address of the raw data to a void * with a utility function ( ogre.CastVoidPt )
        ##
        patch = ogre.MeshManager.getSingleton().createBezierPatch(
            "Bezier1", ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
            ogre.CastVoidPtr(ctypes.addressof(pVert)), patchDecl, 3,3, 5, 5, ogre.PatchSurface.VS_BOTH,
            ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY, ogre.HardwareBuffer.HBU_DYNAMIC_WRITE_ONLY,
            True, True) 

        ## Start patch at 0 detail
        patch.setSubdivision(0.0) 
        ## Create entity based on patch
        patchEntity = sceneManager.createEntity("Entity1", "Bezier1") 

        pMat = ogre.MaterialManager.getSingleton().create("TextMat", 
            ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
        pMat.getTechnique(0).getPass(0).createTextureUnitState( "BumpyMetal.jpg" ) 
        patchEntity.setMaterialName("TextMat") 
        patchPass = pMat.getTechnique(0).getPass(0) 

        ## Attach the entity to the root of the scene
        sceneManager.getRootSceneNode().attachObject(patchEntity) 

        self.camera.setPosition(500,500, 1500) 
        self.camera.lookAt(0,200,-300) 
        sf.Application.debugText  = "STARTED"
        print "SETUP FINISHED!!!!"


    def _destroyScene(self):

        ## free up the pointer before we shut down OGRE
        patch.setNull() 

    def _createFrameListener(self):
        self.frameListener = BezierListener(self.renderWindow, self.camera)
        self.root.addFrameListener(self.frameListener)
        self.frameListener.showDebugOverlay( True )
        
        
        
if __name__ == '__main__':
    try:
        application = BezierApplication()
        application.go()
    except ogre.Exception, e:
        print e


