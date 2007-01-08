
#        Demonstration of facial animation features, using Pose animation

import Ogre as ogre
import CEGUI as cegui
import SampleFramework


def CreateControl(widget_name,name,parent,pos,size,min,max,text='',showframe=False):

    control = cegui.WindowManager.getSingleton().createWindow(widget_name, name)
    parent.addChildWindow(control)
    control.position = cegui.Point(pos[0],pos[1])
    control.size = cegui.Size(size[0],size[1])
    control.minimumSize = cegui.Size(min[0],min[1])
    control.maximumSize = cegui.Size(max[0],max[1])   
    control.text = text
    control.frameEnabled = showframe
    return control	

def printList(name,ntd):
    print ("------------ %s List Print --------------") % name
    print "Count is ",ntd.count()
    print dir(ntd)
    print type(ntd)
    for i in ntd.iterkeys():
        print "key", i
    for i in ntd.itervalues():
        print "val", i
    for k,v in ntd.iteritems():
        print "item", k,v
    print ("------------ %s End List Print --------------") % name
	
	
def showAnimationLists(animation):

    ntd = animation._getNodeTrackList()
	
    
    # To Do not yet wrapped problem in Ogre1.2RC1	
    #ntd = animation._getVertexTrackList()
    #print "------------ Vertex Track List --------------"
    #print dir(ntd)
    #print type(ntd)
    #print ntd 	

    #ntd = animation._getNumericTrackList()
    #printList("Numeric Track List",ntd) 	

	
    #print "Node TrackList" , dir(ogre.NodeTrackList)
    #iterator = meshList.get()
    #meshes = [ iterator.__getitem__(j) for j in range(iterator.size()) ]
	
	
def showAnimationIterators(animation):

    ntd = animation._getNodeTrackList()
    print "------------ Animation Node Track List --------------"
    print dir(ntd)
    print type(ntd)
    print ntd 	

    ntd = animation._getVertexTrackList()
    print "------------ Vertex Track List --------------"
    print dir(ntd)
    print type(ntd)

    ntd = animation._getNumericTrackList()
    print "------------ Numeric Track List --------------"
    print dir(ntd)
    print type(ntd)
    print ntd 	
	
def showAnimationStates(entity):

    print "------------ ovrelay iterator --------------"
    print dir(ogre.OverlayManager.getSingleton().getOverlayIterator())			

    print "------------ list of Animation States --------------"
    st = entity.getAllAnimationStates()
    print st
    print dir(st)
    print type(st)	
    print "------------ Iterator --------------"
    print "INFO   ",st.getAnimationStateIterator()
    print "TYPE  ",type(st.getAnimationStateIterator())    	
    print "DIR    ",dir(st.getAnimationStateIterator())    	
    print "------------ End Iterator --------------"

    
def getEntityInformation(mesh,anim_type):
    faces = []
    vertices = []

    
    added_shared = False		
    for i in range(mesh.numSubMeshes):
        if mesh.getSubMesh(i).vertexAnimationType==anim_type:        
            if mesh.getSubMesh(i).useSharedVertices:
                if(not added_shared):			
                    vertices+= mesh.getSubMesh(i).vertices
                added_shared = True
                
            faces+= mesh.getSubMesh(i).indices
    return vertices,faces    

        	
SI_COUNT=18

class ScrollbarIndex:
    SI_HAPPY,SI_SAD , SI_ANGRY ,SI_A ,	SI_E ,SI_I ,SI_O ,SI_U ,SI_C ,SI_W ,SI_M ,SI_L ,SI_F, SI_T ,SI_P ,SI_R ,SI_S ,SI_TH ,	SI_COUNT =range(19)
	

def _PointHack(x, y):
    return cegui.Vector2(x, y)
cegui.Point = _PointHack


class CEGUIFrameListener(SampleFramework.FrameListener):

    def __init__(self, renderWindow, camera, sceneManager,speakAnimState,speakManualState,manualKeyFrame):
        self.connections=[]	
        self.scrollbarNames=[]
        self.scrollbars=[ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.poseIndexes=[ 1, 2, 3, 4, 7, 8, 6, 5, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18]
        self.mPlayAnimation = False
        self.speakAnimState = speakAnimState
        self.speakManualState = speakManualState		
        SampleFramework.FrameListener.__init__(self, renderWindow, camera)
        self.renderWindow = renderWindow
        self.speakAnimState=speakAnimState		
        self.manualKeyFrame = manualKeyFrame		
        self.keepRendering = True   # whether to continue rendering or not
	
    def _setupInput(self):
        self.eventProcessor = ogre.EventProcessor()
        self.eventProcessor.initialise(self.renderWindow)
        self.eventProcessor.startProcessingEvents()

        # register as a listener for events
        self.eventProcessor.addKeyListener(self)
        self.eventProcessor.addMouseListener(self)
        self.eventProcessor.addMouseMotionListener(self)


        self.scrollbarNames.append("Facial/Happy_Scroll")
        self.scrollbarNames.append("Facial/Sad_Scroll",)
        self.scrollbarNames.append("Facial/Angry_Scroll")
        self.scrollbarNames.append("Facial/A_Scroll")
        self.scrollbarNames.append("Facial/E_Scroll")
        self.scrollbarNames.append("Facial/I_Scroll")
        self.scrollbarNames.append("Facial/O_Scroll")
        self.scrollbarNames.append("Facial/U_Scroll")
        self.scrollbarNames.append("Facial/C_Scroll")
        self.scrollbarNames.append("Facial/W_Scroll")
        self.scrollbarNames.append("Facial/M_Scroll")
        self.scrollbarNames.append("Facial/L_Scroll")
        self.scrollbarNames.append("Facial/F_Scroll")
        self.scrollbarNames.append("Facial/T_Scroll")
        self.scrollbarNames.append("Facial/P_Scroll")
        self.scrollbarNames.append("Facial/R_Scroll")
        self.scrollbarNames.append("Facial/S_Scroll")
        self.scrollbarNames.append("Facial/TH_Scroll")
		
        print "Names",self.scrollbarNames 
        # window manager singleton
        wmgr = cegui.WindowManager.getSingleton()
        main = wmgr.getWindow("FacialDemo") 
        main.enabled=True		
		
        for i in xrange(0,SI_COUNT):
            print self.scrollbarNames[i] 
            self.scrollbars[i] = wmgr.getWindow(self.scrollbarNames[i])
            self.connections.append(self.scrollbars[i].subscribeEvent(str(self.scrollbars[i].EventThumbTrackEnded),self.handleScrollChanged))			
            self.scrollbars[i].enabled=True

        btn = wmgr.getWindow("Facial/Radio/Play")
        btn.selected=True
        #self.connections.append(btn.subscribeEvent(str(btn.EventSelectStateChanged),self.handleRadioChanged))
        print type(btn.EventSelectStateChanged) 		
        self.connections.append(btn.subscribeEvent(btn.EventSelectStateChanged,self.handleRadioChanged))			
        self.mPlayAnimation = True

		
    def requestShutdown(self):
        self.keepRendering = False

    def frameStarted(self, evt):
        self.speakAnimState.addTime(evt.timeSinceLastFrame)
        if (not self.keepRendering):
            return False
        return True		
        #return SampleFramework.FrameListener.frameStarted(evt)

    def mouseDragged(self, evt):
        self.mouseMoved(evt)

    def mousePressed(self, evt):
        button = self._convertOgreButtonToCegui(evt)
        cegui.System.getSingleton().injectMouseButtonDown(button)

    def mouseReleased(self, evt):
        button = self._convertOgreButtonToCegui(evt)
        cegui.System.getSingleton().injectMouseButtonUp(button)
	
    def mouseClicked(self, evt):
        pass

    def mouseEntered(self, evt):
        pass

    def mouseExited(self, evt):
        pass

    def mouseMoved(self, evt):
        system = cegui.System.getSingleton()
        renderer = system.renderer
        system.injectMouseMove(evt.relX * renderer.width,
                               evt.relY * renderer.height)


    def keyPressed(self, evt):

        if(evt.key == ogre.KC_ESCAPE):
            self.keepRendering = False
            evt.consume()
            self.speakAnimState.enabled = False
            self.speakManualState.enabled =  False
            print "Deleting Connections"			
            while len(self.connections) > 0:
                print "here" 
                c = self.connections.pop()
                try:	
                    c.disconnect()
                except:
                    pass			
                del c
            print "Connections deleted"			

        if (evt.key == ogre.KC_SYSRQ):
            self.renderWindow.writeContentsToTimestampedFile("screenshot", ".png")

        system = cegui.System.getSingleton()
        system.injectKeyDown(evt.key)
        system.injectChar(evt.keyChar)
        evt.consume()
		
		
    def keyReleased(self, evt):
        system = cegui.System.getSingleton()
        system.injectKeyUp(evt.key)
        evt.consume()

    def keyClicked(self, evt):
        evt.consume()

    def _convertOgreButtonToCegui(self,evt):
	# Convert ogre button to cegui button
        if (evt.buttonID & ogre.MouseEvent.BUTTON0_MASK):
            return cegui.LeftButton	
            print "Left Button"
        elif (evt.buttonID & ogre.MouseEvent.BUTTON1_MASK):
            return cegui.RightButton	
        elif (evt.buttonID & ogre.MouseEvent.BUTTON2_MASK):
            return cegui.MiddleButton
        elif (evt.buttonID & ogre.MouseEvent.BUTTON3_MASK):
            return cegui.X1Button
        return cegui.LeftButton

	#Handle the scrollbars changing
    def handleScrollChanged(self,args):
        if (not self.mPlayAnimation):
            #Alter the animation 
            name = args.window.name
            #Find which pose was changed
            for i in xrange(0,SI_COUNT):
                if (self.scrollbarNames[i] == name):
                    break
            if (i != SI_COUNT):
                #Update the pose
                self.manualKeyFrame.updatePoseReference(
                  self.poseIndexes[i], self.scrollbars[i].scrollPosition)
                #Dirty animation state since we're fudging this manually
                self.speakManualState.parent._notifyDirty()
		#return True

    #Handle play animation / manual tweaking event
    def handleRadioChanged(self,args):
        self.mPlayAnimation = not self.mPlayAnimation
        self.speakAnimState.enabled = self.mPlayAnimation
        self.speakManualState.enabled=  not self.mPlayAnimation
        for i in xrange(0,SI_COUNT):
            self.scrollbars[i].enabled = not self.mPlayAnimation
        #return True
    #Handle play animation / manual tweaking event
    def handleQuit(self,args):
        self.requestShutdown
        #return True

		
		
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

	
        # Initiaslise CEGUI Renderer
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RENDER_QUEUE_OVERLAY, False, self.sceneManager)		
        #self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow)
        self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane

        # Load Cegui Scheme
        cegui.SchemeManager.getSingleton().loadScheme("TaharezLookSkin.scheme")
        self.system.defaultMouseCursor = "TaharezLook", "MouseArrow"
        cegui.FontManager.getSingleton().createFont("Iconified-12.font")

        # Load Layout (from media/gui/datafiles/layout)
        sheet = cegui.WindowManager.getSingleton().loadWindowLayout("facial.layout")
        self.system.guiSheet = sheet
        sheet.enablede=True


	
    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = 0.5, 0.5, 0.5

        # Create a light
        l = sceneManager.createLight("MainLight")
        #Accept default settings: point light, white diffuse, just set position
        #NB I could attach the light to a SceneNode if I wanted it to move automatically with
        #other objects, but I don't
        l.position = (20,80,50);
        l.diffuseColour=(1.0, 1.0, 1.0)

        #Create a light
        l = sceneManager.createLight("MainLight2")
        l.position=(-120,-80,-50)
        l.diffuseColour=(0.7, 0.7, 0.6)


        #Pre-load the mesh so that we can tweak it with a manual animation
        mesh = ogre.MeshManager.getSingleton().load("facial.mesh", 'General')
        anim = mesh.createAnimation("manual", 0)
        track = anim.createVertexTrack(4, ogre.VAT_POSE)
        print "Vertex track",dir(track)
        print "VertexPoseKeyFrame",dir(ogre.VertexPoseKeyFrame)		
        self.manualKeyFrame = track.createVertexPoseKeyFrame(0)
        manualKeyFrame=self.manualKeyFrame		
        #create pose references, initially zero
        self.poseIndexes=[ 1, 2, 3, 4, 7, 8, 6, 5, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18]
        for i in xrange(0,SI_COUNT):	
            manualKeyFrame.addPoseReference(self.poseIndexes[i], 0.0)
			
        head = sceneManager.createEntity("Head", "facial.mesh")
        self.speakAnimState = head.getAnimationState("Speak")
        self.speakAnimState.enabled=True
        self.manualAnimState = head.getAnimationState("manual")
        self.manualAnimState.timePosition=0

        headNode = sceneManager.rootSceneNode.createChildSceneNode()
        headNode.attachObject(head)

        self.camera.position=(-20, 50, 150);
        self.camera.lookAt(0,35,0)
			
        self._createGUI()
        #showAnimationStates(head)
        #showAnimationLists(anim)
        
                                 
        
    def _createCamera(self):
        self.camera = self.sceneManager.createCamera("PlayerCam")
        self.camera.nearClipDistance = 5

    def _createFrameListener(self):
        self.frameListener = CEGUIFrameListener(self.renderWindow, self.camera, self.sceneManager,
                            self.speakAnimState,self.manualAnimState,self.manualKeyFrame)
        self.root.addFrameListener(self.frameListener)
        self.frameListener.showDebugOverlay(True)

    def __del__(self):
        "Clear variables, this is needed to ensure the correct order of deletion"
        del self.camera
        del self.sceneManager
        print "Deleting FrameListener"			
        del self.frameListener
        print "Deleting System"			
        del self.system
        print "Deleting guiRenderer"			
        del self.guiRenderer
        print "Deleting root"			
        del self.root
        print "Deleting Renderer Window"			
        del self.renderWindow        
     

if __name__ == '__main__':
    try:
        ta = FacialAnimationApplication()
        ta.go()
    except ogre.OgreException, e:
        print e








