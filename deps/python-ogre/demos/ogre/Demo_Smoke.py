# This code is in the Public Domain
import  Ogre as ogre
import SampleFramework as sf
import ctypes, math

class SmokeApplication(sf.Application):
    def _createScene(self):
        sceneManager = self.sceneManager
        camera = self.camera

        sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)
        sceneManager.setSkyDome(True, 'Examples/CloudySky', 5.0, 8.0)

        self.fountainNode = sceneManager.getRootSceneNode().createChildSceneNode()
        
        psm = ogre.ParticleSystemManager.getSingleton()
        particleSystem2 = sceneManager.createParticleSystem('fountain1', 'Examples/Smoke')
        node = self.fountainNode.createChildSceneNode()
        node.attachObject(particleSystem2)
###        self.testManualObjectNonIndexed()
#         entiter = self.sceneManager.getMovableObject("test", ogre.ManualObjectFactory.FACTORY_TYPE_NAME)
#         entiter = self.sceneManager.getMovableObjectIterator("test")
        
        
        
    def testManualObjectNonIndexed(self):
            self.sceneManager.setAmbientLight(ogre.ColourValue(0.5, 0.5, 0.5))
            dir1=ogre.Vector3(-1, -1, 0.5)
            dir1.setNormalise()
            l = self.sceneManager.createLight("light1")
            l.setType(ogre.Light.LT_DIRECTIONAL)
            l.setDirection(dir1)
            n = l.getAnimableValueNames()
            v = l.createAnimableValue ('VALUE')
    
            plane = ogre.Plane()
            plane.setNormal ( ogre.Vector3.UNIT_Y )
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
            man.setPosition(-20, 20, 20)
            man.setNormal(0, 0, 1)
            man.textureCoord(0, 0)
    
            man.setPosition(-20, -20, 20)
            man.setNormal(0, 0, 1)
            man.textureCoord(0, 1)
    
            man.setPosition(20, 20, 20)
            man.setNormal(0, 0, 1)
            man.textureCoord(1, 0)
    
            man.setPosition(-20, -20, 20)
            man.setNormal(0, 0, 1)
            man.textureCoord(0, 1)
    
            man.setPosition(20, -20, 20)
            man.setNormal(0, 0, 1)
            man.textureCoord(1, 1)
    
            man.setPosition(20, 20, 20)
            man.setNormal(0, 0, 1)
            man.textureCoord(1, 0)
    
            man.end()
    
            man.begin("Examples/BumpyMetal")
    
            ## Define a 40x40 plane, non-indexed
            man.setPosition(-20, 20, 20)
            man.setNormal(0, 1, 0)
            man.textureCoord(0, 0)
    
            man.setPosition(20, 20, 20)
            man.setNormal(0, 1, 0)
            man.textureCoord(0, 1)
    
            man.setPosition(20, 20, -20)
            man.setNormal(0, 1, 0)
            man.textureCoord(1, 1)
    
            man.setPosition(20, 20, -20)
            man.setNormal(0, 1, 0)
            man.textureCoord(1, 1)
    
            man.setPosition(-20, 20, -20)
            man.setNormal(0, 1, 0)
            man.textureCoord(1, 0)
    
            man.setPosition(-20, 20, 20)
            man.setNormal(0, 1, 0)
            man.textureCoord(0, 0)
    
            man.end()
    
    
            self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(man)
        
if __name__ == '__main__':
    try:
        application = SmokeApplication()
        application.go()
    except ogre.Exception, e:
        print e
