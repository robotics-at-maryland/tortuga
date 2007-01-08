# This code is in the Public Domain
import  Ogre as ogre
import SampleFramework as sf

class ParticleApplication(sf.Application):
    def _createScene(self):
        sceneManager = self.sceneManager
        camera = self.camera
        
        sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)

        entity = sceneManager.createEntity('head', 'ogrehead.mesh')
        entity.setMaterialName('Examples/EnvMappedRustySteel')

        sceneManager.getRootSceneNode().createChildSceneNode().attachObject(entity)

        particleSystemManager = ogre.ParticleSystemManager.getSingleton()
        particleSystem1 = sceneManager.createParticleSystem('Nimbus', 'Examples/GreenyNimbus')
        sceneManager.getRootSceneNode().createChildSceneNode().attachObject(particleSystem1)

        self.fountainNode = sceneManager.getRootSceneNode().createChildSceneNode()

        particleSystem2 = sceneManager.createParticleSystem('fountain1', 'Examples/PurpleFountain')
        node = self.fountainNode.createChildSceneNode()
        node.translate(200.0, -100.0, 0.0,ogre.Node.TS_PARENT)
        node.rotate(ogre.Vector3(0, 0, -1), ogre.Radian(ogre.Degree(20)),ogre.Node.TS_PARENT)
        node.attachObject(particleSystem2)

        particleSystem3 = sceneManager.createParticleSystem('fountain2', 'Examples/PurpleFountain')
        node = self.fountainNode.createChildSceneNode()
        node.translate(-200.0, -100.0, 0.0,ogre.Node.TS_PARENT)
        node.rotate(ogre.Vector3(0, 0, -1), ogre.Radian(ogre.Degree(-20)),ogre.Node.TS_PARENT)
        node.attachObject(particleSystem3)

        particleSystem4 = sceneManager.createParticleSystem('rain', 'Examples/Rain')
        node = sceneManager.getRootSceneNode().createChildSceneNode()
        node.translate(0.0, 1000.0, 0.0,ogre.Node.TS_PARENT)
        node.attachObject(particleSystem4)
        particleSystem4.fastForward(5)
#         
        # Aureola around Ogre perpendicular to the ground
        ## AJM This media is part of OgreSDK not in the current python_ogre set..
        try:
            particleSystem5 = sceneManager.createParticleSystem('Aureola', 'Examples/Aureola')
            sceneManager.getRootSceneNode().createChildSceneNode().attachObject(particleSystem5)
        except:
            pass

		# Set nonvisible timeout
        ogre.ParticleSystem.defaultNonVisibleUpdateTimeout=5

        
    def _createFrameListener(self):
        self.frameListener = ParticleListener(self.renderWindow, self.camera, self.fountainNode)
        self.root.addFrameListener(self.frameListener)

class ParticleListener(sf.FrameListener):
    def __init__(self, renderWindow, camera, fountainNode):
        sf.FrameListener.__init__(self, renderWindow, camera)
        self.fountainNode = fountainNode

    def frameStarted(self, frameEvent):
        dd = ogre.Degree(frameEvent.timeSinceLastFrame * 30.0)
        rad = ogre.Radian(dd)
        self.fountainNode.yaw(rad, ogre.Node.TS_LOCAL)
        return sf.FrameListener.frameStarted(self, frameEvent)

if __name__ == '__main__':
    try:
        application = ParticleApplication()
        application.go()
    except ogre.Exception, e:
        print e
