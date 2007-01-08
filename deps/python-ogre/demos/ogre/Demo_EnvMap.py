# this code is in the public domain
import Ogre as ogre
import SampleFramework as sf

class EnvMapApplication(sf.Application):
    def _createScene( self ):
        sceneManager = self.sceneManager
        camera = self.camera
        
        sceneManager.ambientLight = ogre.ColourValue (0.5, 0.5, 0.5)

        light = sceneManager.createLight('MainLight')
        light.setPosition (20, 80, 50)

        entity = sceneManager.createEntity('head', 'ogrehead.mesh')
        entity.setMaterialName("Examples/EnvMappedRustySteel")

        sceneManager.getRootSceneNode().createChildSceneNode().attachObject(entity)

if __name__ == '__main__':
    try:
        application = EnvMapApplication()
        application.go()
    except ogre.Exception, e:
        print e
