# ===============================================================
#
# TODO: 
#
#
# ===============================================================


import Ogre as ogre
import CEGUI as cegui
import SampleFramework

from CEGUI_framework import *

SI_COUNT=18
poseIndexes=[ 1, 2, 3, 4, 7, 8, 6, 5, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18]

scrollbarNames = [ "Facial/Happy_Scroll",
                   "Facial/Sad_Scroll",
                   "Facial/Angry_Scroll",
                   "Facial/A_Scroll",
                   "Facial/E_Scroll",
                   "Facial/I_Scroll",
                   "Facial/O_Scroll",
                   "Facial/U_Scroll",
                   "Facial/C_Scroll",
                   "Facial/W_Scroll",
                   "Facial/M_Scroll",
                   "Facial/L_Scroll",
                   "Facial/F_Scroll",
                   "Facial/T_Scroll",
                   "Facial/P_Scroll",
                   "Facial/R_Scroll",
                   "Facial/S_Scroll",
                   "Facial/TH_Scroll"]

class ScrollbarIndex:
    SI_HAPPY,SI_SAD , SI_ANGRY ,SI_A ,	SI_E ,SI_I ,SI_O ,SI_U ,SI_C ,SI_W ,SI_M ,SI_L ,SI_F, SI_T ,SI_P ,SI_R ,SI_S ,SI_TH ,	SI_COUNT =range(19)


class CEGUIFrameListener(GuiFrameListener):

    def __init__(self, renderWindow, camera, sceneManager,speakAnimState,speakManualState,manualKeyFrame):

        self.scrollbars=[0]*18
        self.connections = []

        self.mPlayAnimation = False
        self.speakAnimState = speakAnimState
        self.speakManualState = speakManualState		
        self.manualKeyFrame   = manualKeyFrame		

        GuiFrameListener.__init__(self, renderWindow, camera, sceneManager)
        self._setupGUI()
        self._setupConnections()
        self.MenuMode = True


    def _setupGUI(self):
		
        # get main cegui anim window 
        wmgr = cegui.WindowManager.getSingleton()
        main = wmgr.getWindow("FacialDemo") 
        main.enabled=True		
        # set up radio button event handling, initial state (Animation)
        self.mPlayAnimation = True

    def _setupConnections(self):
		
        wmgr = cegui.WindowManager.getSingleton()
        # set up anim slider event handling, initial state (disabled)
        for i in xrange(0,SI_COUNT):
            self.scrollbars[i] = wmgr.getWindow(scrollbarNames[i])
            self.connections.append(self.scrollbars[i].subscribeEvent(self.scrollbars[i].EventThumbTrackEnded, self, "handleScrollChanged"))
            self.scrollbars[i].setEnabled(False)
        btn = wmgr.getWindow("Facial/Radio/Play")
        btn.selected=True
        self.connections.append(btn.subscribeEvent(btn.EventSelectStateChanged, self, "handleRadioChanged"))

    def frameStarted(self, evt):
        ok = GuiFrameListener.frameStarted(self,evt)
        if ok: 
            self.speakAnimState.addTime(evt.timeSinceLastFrame)
        return ok

    # scrollbars event handler
    def handleScrollChanged(self,args):
        if (not self.mPlayAnimation):
            #Alter the animation 
            name = args.window.name.c_str()   ## Note that you need to extract the string
            #Find which pose was changed
            for i in xrange(0,SI_COUNT):
                if (scrollbarNames[i] == name):
                    break
            if (i != SI_COUNT):
                #Update the pose
                self.manualKeyFrame.updatePoseReference(
                  poseIndexes[i], self.scrollbars[i].getScrollPosition() ) #scrollPosition)
                #Dirty animation state since we're fudging this manually
                self.speakManualState.getParent()._notifyDirty()

        return True

    # radio button event handler switch betweens manual/auto animation
    def handleRadioChanged(self,args):
        self.mPlayAnimation = not self.mPlayAnimation
        self.speakAnimState.setEnabled(self.mPlayAnimation)
        self.speakManualState.setEnabled(not self.mPlayAnimation)
        for i in xrange(0,SI_COUNT):
            self.scrollbars[i].setEnabled(not self.mPlayAnimation)
        return True
		
		
class FacialAnimationApplication(SampleFramework.Application):

    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.guiRenderer=0
        self.system =0
        self.speakAnimState = None		
        self.manualAnimState = None
        self.manualKeyFrame = None
        self.mesh = None

		
    def _createGUI(self):

        ## set the default resource groups to be used
        CEGUI.Imageset.setDefaultResourceGroup("imagesets")
        #CEGUI.Font.setDefaultResourceGroup("fonts")
        CEGUI.Scheme.setDefaultResourceGroup("schemes")
        CEGUI.WidgetLookManager.setDefaultResourceGroup("looknfeels")
        CEGUI.WindowManager.setDefaultResourceGroup("layouts")
        CEGUI.ScriptModule.setDefaultResourceGroup("lua_scripts")

        # Initiaslise CEGUI Renderer
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RENDER_QUEUE_OVERLAY, False, 3000, self.sceneManager)

        # create cegui system and log name facial.log
        self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane

        # Load Cegui Scheme
        cegui.SchemeManager.getSingleton().loadScheme("TaharezLook.scheme")
        self.system.setDefaultMouseCursor("TaharezLook", "MouseArrow")
        cegui.FontManager.getSingleton().createFont("bluehighway-12.font")

        # Load Layout 
        sheet = cegui.WindowManager.getSingleton().loadWindowLayout("facial.layout")
        self.system.setGUISheet (sheet)
        sheet.enabled=True
	
    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = ogre.ColourValue(0.25, 0.25, 0.25)

        # Create a light
        l = sceneManager.createLight("MainLight")
        #Accept default settings: point light, white diffuse, just set position
        #NB I could attach the light to a SceneNode if I wanted it to move automatically with
        #other objects, but I don't
        l.setPosition(ogre.Vector3(20,80,50))
        l.setDiffuseColour(ogre.ColourValue(1.0, 1.0, 1.0))

        # create a light
        l = sceneManager.createLight("MainLight2")
        l.setPosition (ogre.Vector3(-120,-80,-50))
        l.setDiffuseColour(ogre.ColourValue(0.7, 0.7, 0.6))


        # pre-load the mesh so that we can tweak it with a manual animation
        mesh = ogre.MeshManager.getSingleton().load("facial.mesh", 'General')
        anim = mesh.createAnimation("manual", 0)
        track = anim.createVertexTrack(4, ogre.VAT_POSE)
        self.manualKeyFrame = track.createVertexPoseKeyFrame(0)
        manualKeyFrame=self.manualKeyFrame		

        # create pose references, initially zero
        for i in xrange(0,SI_COUNT):	
            manualKeyFrame.addPoseReference(poseIndexes[i], 0.0)
			
        head = sceneManager.createEntity("Head", "facial.mesh")
        self.speakAnimState = head.getAnimationState("Speak")
        self.speakAnimState.enabled=True
        self.manualAnimState = head.getAnimationState("manual")
        self.manualAnimState.timePosition=0

        headNode = sceneManager.rootSceneNode.createChildSceneNode()
        headNode.attachObject(head)

        self.camera.setPosition( ogre.Vector3(-20, 50, 150) )
        self.camera.lookAt( ogre.Vector3(0,35,0) )
			
        self._createGUI()
        
                                 
        
    def _createCamera(self):
        self.camera = self.sceneManager.createCamera("PlayerCam")
        self.camera.setNearClipDistance(5)

    def _createFrameListener(self):
        self.frameListener = CEGUIFrameListener(self.renderWindow, self.camera, self.sceneManager,
                            self.speakAnimState,self.manualAnimState,self.manualKeyFrame)
        self.root.addFrameListener(self.frameListener)
        self.frameListener.showDebugOverlay(True)

    def __del__(self):
        "Clear variables, this is needed to ensure the correct order of deletion"
        del self.camera
        del self.sceneManager
        del self.frameListener
        del self.system
        del self.guiRenderer
        del self.root
        del self.renderWindow        
     

if __name__ == '__main__':
    try:
        ta = FacialAnimationApplication()
        ta.go()
    except ogre.OgreException, e:
        print e




