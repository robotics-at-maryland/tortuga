# This code is in the Public Domain
import  Ogre as ogre
import SampleFramework as sf
import ctypes, math

class SmokeApplication(sf.Application):
    def _createScene(self):
        sceneManager = self.sceneManager
        print dir(sceneManager)
        print "============="
        camera = self.camera

        sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)
        sceneManager.setSkyDome(True, 'Examples/CloudySky', 5.0, 8.0)

        self.fountainNode = sceneManager.getRootSceneNode().createChildSceneNode()
#         print self.fountainNode
#         print dir ( self.fountainNode )
#         self.fountainNode.showBoundingBox( True )
#         print self.fountainNode.getShowBoundingBox()
        
        print "++++++++++++"
        
        psm = ogre.ParticleSystemManager.getSingleton()
        particleSystem2 = sceneManager.createParticleSystem('fountain1', 'Examples/Smoke')
        node = self.fountainNode.createChildSceneNode()
        node.attachObject(particleSystem2)
        self.prepareCircleMaterial()
###        self.testManualObjectNonIndexed()
#         entiter = self.sceneManager.getMovableObject("test", ogre.ManualObjectFactory.FACTORY_TYPE_NAME)
#         print entiter
#         entiter = self.sceneManager.getMovableObjectIterator("test")
#         print entiter
#         print dir(entiter)
#         print entiter.hasMoreElements()
#         print entiter.peekNextKey()
#         print entiter.peekNextValue()
#         print entiter.getNext()
        
    def prepareCircleMaterial(self):
        MESH_NAME ="WaterMesh"
        ENTITY_NAME ="WaterEntity"
        MATERIAL_PREFIX ="Examples/Water"
        MATERIAL_NAME ="Examples/Water0"
        COMPLEXITY =64      ## watch out - number of polys is 2*ACCURACY*ACCURACY !
        PLANE_SIZE =3000.0
        CIRCLES_MATERIAL ="Examples/Water/Circles"
    #   bmap =[]
        storageclass = ctypes.c_float * (256 * 256 * 4)
        bmap=storageclass(1.1)
        ctypes.memset ( bmap, 127, 256 * 256 * 4 )
        bmap[22]=1
        bmap[256]=1.445
    #   for x in range (256 * 256 * 4):
    #       bmap.append(127)
            
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
        print bmap
        print ctypes.addressof(bmap)        
        imgstream = ogre.MemoryDataStream(ogre.CastVoidPtr(ctypes.addressof(bmap)),  (256 * 256 * 4 ) )  
#         print ms
#         imgstream = ms
        #imgstream = ogre.DataStream ( ms )
#         imgstream = ogre.DataStream()
        ##~ Image img 
        ##~ img.loadRawData( imgstream, 256, 256, PF_A8R8G8B8 ) 
        ##~ TextureManager.getSingleton().loadImage( CIRCLES_MATERIAL , img ) 
        ogre.TextureManager.getSingleton().loadRawData(CIRCLES_MATERIAL,
            ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME,
            imgstream, 256, 256  , ogre.PixelFormat.PF_A8R8G8B8 ) # , 
#             ogre.TextureType.TEX_TYPE_2D, -1, 1.0 ) 

#   DataStreamPtr imgstream(new MemoryDataStream(bmap, 256 * 256 * 4));
#   //~ Image img;
#   //~ img.loadRawData( imgstream, 256, 256, PF_A8R8G8B8 );
#   //~ TextureManager::getSingleton().loadImage( CIRCLES_MATERIAL , img );
#   TextureManager::getSingleton().loadRawData(CIRCLES_MATERIAL,
#         ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
#       imgstream, 256, 256, PF_A8R8G8B8);
            
                        
        material = ogre.MaterialManager.getSingleton().create( CIRCLES_MATERIAL,
            ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME) 
        texLayer = material.getTechnique(0).getPass(0).createTextureUnitState( CIRCLES_MATERIAL ) 
        texLayer.setTextureAddressingMode( ogre.TextureUnitState.TAM_CLAMP ) 
        material.setSceneBlending( ogre.SceneBlendType.SBT_ADD ) 
        material.setDepthWriteEnabled( False )  
        material.load() 
        ## finished with bmap so release the memory
        del bmap 
        
        
    def testManualObjectNonIndexed(self):
            self.sceneManager.setAmbientLight(ogre.ColourValue(0.5, 0.5, 0.5))
            dir1=ogre.Vector3(-1, -1, 0.5)
            dir1.normalise()
            l = self.sceneManager.createLight("light1")
            l.setType(ogre.Light.LT_DIRECTIONAL)
            l.setDirection(dir1)
            print dir(l)
            n = l.getAnimableValueNames()
            print n
            print n.count()
            print dir(n)
            v = l.createAnimableValue ('VALUE')
            print v
            print dir(v)
    
            plane = ogre.Plane()
            plane.normal = ogre.Vector3.UNIT_Y
            plane.d = 100
            ogre.MeshManager.getSingleton().createPlane("Myplane",
                ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
                1500,1500,10,10,True,1,5,5,ogre.Vector3.UNIT_Z)
            pPlaneEnt = self.sceneManager.createEntity( "plane", "Myplane" )
            pPlaneEnt.setMaterialName("2 - Default")
            pPlaneEnt.setCastShadows(False)
            self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(pPlaneEnt)
    
            man = self.sceneManager.createMovableObject("test", ogre.ManualObjectFactory.FACTORY_TYPE_NAME)
    
            man.begin("Examples/OgreLogo")
            ## Define a 40x40 plane, non-indexed
            man.position(-20, 20, 20)
            man.normal(0, 0, 1)
            man.textureCoord(0, 0)
    
            man.position(-20, -20, 20)
            man.normal(0, 0, 1)
            man.textureCoord(0, 1)
    
            man.position(20, 20, 20)
            man.normal(0, 0, 1)
            man.textureCoord(1, 0)
    
            man.position(-20, -20, 20)
            man.normal(0, 0, 1)
            man.textureCoord(0, 1)
    
            man.position(20, -20, 20)
            man.normal(0, 0, 1)
            man.textureCoord(1, 1)
    
            man.position(20, 20, 20)
            man.normal(0, 0, 1)
            man.textureCoord(1, 0)
    
            man.end()
    
            man.begin("Examples/BumpyMetal")
    
            ## Define a 40x40 plane, non-indexed
            man.position(-20, 20, 20)
            man.normal(0, 1, 0)
            man.textureCoord(0, 0)
    
            man.position(20, 20, 20)
            man.normal(0, 1, 0)
            man.textureCoord(0, 1)
    
            man.position(20, 20, -20)
            man.normal(0, 1, 0)
            man.textureCoord(1, 1)
    
            man.position(20, 20, -20)
            man.normal(0, 1, 0)
            man.textureCoord(1, 1)
    
            man.position(-20, 20, -20)
            man.normal(0, 1, 0)
            man.textureCoord(1, 0)
    
            man.position(-20, 20, 20)
            man.normal(0, 1, 0)
            man.textureCoord(0, 0)
    
            man.end()
    
    
            self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(man)
        
if __name__ == '__main__':
    try:
        application = SmokeApplication()
        application.go()
    except ogre.Exception, e:
        print e
