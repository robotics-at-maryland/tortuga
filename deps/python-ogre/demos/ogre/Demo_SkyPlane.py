# This code is in the Public Domain.
import Ogre as ogre
import SampleFramework as sf

class SkyPlaneApplication(sf.Application):
    def _createScene( self ):
        sceneManager = self.sceneManager
        camera = self.camera
        
        sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)

        plane = ogre.Plane()
        n = plane.normal
        n.x, n.y, n.z = 0, -1, 0
        plane.d = 5000
        sceneManager.setSkyPlane(True, plane, 'Examples/SpaceSkyPlane', 10000, 3)
 
        light = sceneManager.createLight('MainLight')
        light.setPosition (20, 80, 50)

        entity = sceneManager.createEntity('dragon', 'dragon.mesh')
        sceneManager.getRootSceneNode().attachObject(entity)

if __name__ == '__main__':
    try:
        application = SkyPlaneApplication()
        application.go()
    except ogre.Exception, e:
        print e
