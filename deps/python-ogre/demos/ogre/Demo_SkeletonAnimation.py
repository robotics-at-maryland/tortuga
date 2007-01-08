# This code is in the Public Domain.
import Ogre as ogre
import SampleFramework as sf

NUM_ROBOTS = 10

class SkeletalApplication(sf.Application):

    def _createScene(self):
        sceneManager = self.sceneManager
        camera = self.camera

        ogre.Animation.setDefaultInterpolationMode(ogre.Animation.IM_SPLINE)
        sceneManager.AmbientLight = ogre.ColourValue(0.5, 0.5, 0.5)

        self.animationStates = []
        self.animationSpeeds = []
        for index in xrange(0, NUM_ROBOTS):
            entity = sceneManager.createEntity('robot%d' % index, 'robot.mesh')
            sceneManager.getRootSceneNode().createChildSceneNode(ogre.Vector3(0, 0, index * 50 - NUM_ROBOTS * 50 / 2.0)).attachObject(entity)
            self.animationStates.append(entity.getAnimationState('Walk'))
            self.animationStates[-1].Enabled = True
            self.animationSpeeds.append(ogre.Math.RangeRandom(0.5, 1.5))

        light = sceneManager.createLight('BlueLight')
        light.setPosition (-200, -80, -100)
        light.setDiffuseColour (ogre.ColourValue(0.5, 0.5, 1.0) )

        light = sceneManager.createLight('GreenLight')
        light.setPosition (0, 0, -100)
        light.setDiffuseColour (0.5, 1.0, 0.5)

        camera.setPosition (100, 50, 100)
        camera.lookAt(-50, 50, 0)

        # Report whether hardware skinning is enabled or not
        subEntity = entity.getSubEntity(0)
        #material = subEntity.material
        #technique = material.bestTechnique
        #techniquePass = technique.getPass(0)
        #if techniquePass.hasVertexProgram and techniquePass.vertexProgram.skeletalAnimationIncluded:
            #self.renderWindow.debugText = 'Hardware skinning is enabled'
        #else:
            #self.renderWindow.debugText = 'Software skinning is enabled'

    def _createFrameListener(self):
        self.frameListener = SkeletalAnimationFrameListener(self.renderWindow, self.camera,
                                                             self.animationStates, self.animationSpeeds)
        self.root.addFrameListener(self.frameListener)

class SkeletalAnimationFrameListener(sf.FrameListener):
    def __init__(self, renderWindow, camera, animationStates, animationSpeeds):
        sf.FrameListener.__init__(self, renderWindow, camera)
        self.animationStates = animationStates
        self.animationSpeeds = animationSpeeds

    def frameStarted(self, frameEvent):
        for index in xrange(0,len(self.animationStates)):
            self.animationStates[index].addTime(frameEvent.timeSinceLastFrame * self.animationSpeeds[index])
        return sf.FrameListener.frameStarted(self, frameEvent)

if __name__ == '__main__':
    try:
        application = SkeletalApplication()
        application.go()
    except ogre.Exception, e:
        print e
