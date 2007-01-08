# this code is in the public domain

#------------------------------------------------------------------------------
#  This sample shows very simple debug console,
#     1) a list of commands the user scrolls using up/down arrow keys
#     2) Simple Commands are:
#       self.addNode(mesh_name,mesh,position)
#       self.destroyNode(mesh_name)
#        self.addSkyPlane(True/False)
#       self.sceneManager.setSkyDome(True, 'Examples/CloudySky', 4.0, 8.0)
#     3) Run Script Frame
#        - creates Smoke Particle System 
#
#  To do:
#     find out some way to speed up the edit box (slow)
#     check why the skydome/plane is sometimes locking up geui mouse cursor
#     update to be more generic
#
#
#-----------------------------------------------------------------------------

from pyogre import ogre,cegui
import SampleFramework
 
class CEGUIFrameListener(SampleFramework.FrameListener):

    def __init__(self, renderWindow, camera, sceneManager):

        self.connections=[]     # connections
        self.commands=[]
        self.psm = 0             # Smoke particle
        self.particleSystem2 = 0
        self.node = 0
        self.fountainNode = 0

        self.command_line = 0   # command line number

        SampleFramework.FrameListener.__init__(self, renderWindow, camera)
        self.keepRendering = True   # whether to continue rendering or not
        self.sceneManager = sceneManager

        
        # repeat key ---------------------
        self.repeatkeyPressed=False   # repeatKey flag
        self.keyTime=0.0              # holds time since last key inject 
        self.keyDelay=0.0             # holds current time delay to compare
                                      # against initialdelay

        self.key=0                    # holds repeat key
        self.keyChar=""               # holds repeat char
        self.keydelta=0.035            # rate repeat key, make larger slow down, if too fast
        self.initialdelay=0.30        # delay before repeat key kicks in



    # add node
    def addSkyPlane(self,show):
        plane = ogre.Plane()
        n = plane.normal
        n.x, n.y, n.z = 0, -1, 0
        plane.d = 5000
        self.sceneManager.setSkyPlane(show, plane, 'Examples/SpaceSkyPlane', 10000, 3)
 
 
    # add node
    def addNode(self,name,mesh,posx,posy,posz):
        print name,mesh
        node = self.sceneManager.rootSceneNode.createChildSceneNode(name)
        entity = self.sceneManager.createEntity(name, mesh)
        #node.translate(ogre.Vector3(0,20,-200))
        node.translate(ogre.Vector3(posx,posy,posz))
        node.attachObject(entity)
 
    # set Key repeat rate
    def setRepeatKeyRate(self,rate):
        self.keydelta=rate 

    # set Key repeat rate
    def setRepeatKeyDelay(self,delay):
        self.initialdelay=delay
      
    # destroy node
    def destroyNode(self,name):
        node = self.sceneManager.getSceneNode(name)
        #Assuming there is only a single attached entity
        ent=node.detachObject(0)
        self.sceneManager.removeEntity(ent)
        self.sceneManager.destroySceneNode(name)

    # event user has pressed return edit box
    def onEnterText(self,args):
        try:
            # get the command from the edit box
            editBox = args.window
            # execute the command
            exec editBox.text
            # add command to histroy
            history=cegui.WindowManager.getSingleton().getWindow("Debug/MultiLineEditBox")
            history.text+=">" + editBox.text
            
        except:
            history=cegui.WindowManager.getSingleton().getWindow("Debug/MultiLineEditBox")
            history.text+="Command Line Error" 


    # event user has pressed up/down arrow keys
    def onKeyDown(self,args):
       
        key= args.scancode
        if (key==cegui.Key.ArrowUp):
            self.command_line+=-1
        elif key==cegui.Key.ArrowDown:
            self.command_line+=1
        else:
            return 

        if (self.command_line)<0: 
            self.command_line=len(self.commands)-1
        elif (self.command_line)>=len(self.commands): self.command_line=0

        try:
            editBox = args.window
            editBox.text=self.commands[self.command_line]
            editBox.activate()  # set focus
            editBox.caratIndex=len(editBox.text)

        except:
            print "Event Error"

    # event user has run script
    def onButtonClick(self,args):
        try:
            # get the command from the edit box
            editBox = cegui.WindowManager.getSingleton().getWindow("Script/EditBox")
            print editBox.text
            history=cegui.WindowManager.getSingleton().getWindow("Debug/MultiLineEditBox")

            # execute the command
            exec editBox.text
            # add command to histroy
            history.text+=">" + editBox.text
           
        except:
            history=cegui.WindowManager.getSingleton().getWindow("Debug/MultiLineEditBox")
            history.text+="Command Line Error" 

    def _setupInput(self):
        self.eventProcessor = ogre.EventProcessor()
        self.eventProcessor.initialise(self.renderWindow)
        self.eventProcessor.startProcessingEvents()

        # register as a listener for events
        self.eventProcessor.addKeyListener(self)
        self.eventProcessor.addMouseListener(self)
        self.eventProcessor.addMouseMotionListener(self)

        # have to create here so commands know about self.scenemanager
        edit=cegui.WindowManager.getSingleton().getWindow("Debug/EditBox")
        scriptedit=cegui.WindowManager.getSingleton().getWindow("Script/EditBox")
        scriptbutton=cegui.WindowManager.getSingleton().getWindow("Script/Run")
        
        # events
        self.connections.append(edit.subscribeEvent(edit.EventTextAccepted, self.onEnterText))
        self.connections.append(scriptbutton.subscribeEvent(scriptbutton.EventClicked, self.onButtonClick))
        self.connections.append(edit.subscribeEvent(edit.EventKeyDown, self.onKeyDown))


        
        # list sample commands
        self.commands.append("self.sceneManager.setSkyDome(True, 'Examples/CloudySky', 4.0, 8.0)")
        self.commands.append("self.addNode('head2','ogrehead.mesh',0,0,-200)")
        self.commands.append("self.destroyNode('head2')")
        self.commands.append("self.addSkyPlane(True)")
        self.commands.append("self.setRepeatKeyRate(0.05)")
        self.commands.append("self.setRepeatKeyDelay(0.30)")
 

        # Script Window adds smoke particle effect
        scriptedit.text="" 
        scriptedit.text+="self.fountainNode = self.sceneManager.rootSceneNode.createChildSceneNode() "  
        scriptedit.text+="self.fountainNode.translate(ogre.Vector3(0,0,-200)) " 
        scriptedit.text+="self.psm = ogre.ParticleSystemManager.getSingleton() "
        scriptedit.text+="self.particleSystem2 = self.psm.createSystem('fountain1', 'Examples/Smoke') " 
        scriptedit.text+="self.node = self.fountainNode.createChildSceneNode() " 
        scriptedit.text+="self.node.attachObject(self.particleSystem2) "

        # first command in edit box, put carat at end of line
        edit.text = self.commands[0]
        edit.activate()  # set focus
        edit.caratIndex=len(edit.text)


    def frameStarted(self, evt):

        # repeat key ---------------------
        self.injectRepeatKey(evt.timeSinceLastFrame)

        # disonnect events
        if not self.keepRendering:
            for i in range(len(self.connections)):
                c = self.connections[i]
                c.disconnect    
        return self.keepRendering

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


    # repeat key ---------------------
    def injectRepeatKey(self,time):
 
        # repeat key
        if not (self.repeatkeyPressed): return

        # initial delay for pressing key 
        if not (self.keyDelay<=0):
            self.keyDelay-=time
            return         

        # inject key char every keydelta time
        self.keyTime+=time
        print  "Key",self.keyTime,self.keydelta
        if (self.keyTime>=self.keydelta):
            system = cegui.System.getSingleton()
            system.injectKeyDown(self.key)
            system.injectChar(self.keyChar)
            self.keyTime=0
  
    # Called First
    def keyPressed(self, evt):

        # init repeat key timing ---------------------
        self.repeatkeyPressed=True
        self.keyTime=0.0
        self.keyDelay = self.initialdelay

        #print "Key Pressed Time",evt.key,self.keytime 
        if evt.key == ogre.KC_ESCAPE:
            self.keepRendering = False
        system = cegui.System.getSingleton() 
        system.injectKeyDown(evt.key)
        system.injectChar(evt.keyChar)

        # store our repeat key ---------------------
        self.key = evt.key
        self.keyChar = evt.keyChar      
        evt.consume()


    # Called Second
    def keyReleased(self, evt):

        # disable repeat key ---------------------
        self.repeatkeyPressed=False

        #print "Key Released Time",evt.key,self.keyTime
        system = cegui.System.getSingleton()
        system.injectKeyUp(evt.key)

    # Called Last
    def keyClicked(self, evt):
        pass


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

   
class GEUIApplication(SampleFramework.Application):

    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.guiRenderer=0
        self.system =0
        self.ec = 0

    def _createGUI(self):

        # Initiaslise CEGUI Renderer
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RENDER_QUEUE_OVERLAY, False, self.sceneManager)
	self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane

        # Load Cegui Scheme
        cegui.SchemeManager.getSingleton().loadScheme("WindowsLookSkin.scheme")
        self.system.defaultMouseCursor = "WindowsLook", "MouseArrow"
        cegui.FontManager.getSingleton().createFont("Iconified-12.font")

         # Load Layout root window layout
        sheet = cegui.WindowManager.getSingleton().loadWindowLayout("Default.layout")
        self.system.guiSheet = sheet

        # Load our DebugConsole.layout
        fr1=cegui.WindowManager.getSingleton().loadWindowLayout("DebugConsole.layout","Debug/")

        # Set some additional control characteristics
        edit=cegui.WindowManager.getSingleton().getWindow("Debug/EditBox")
        medit=cegui.WindowManager.getSingleton().getWindow("Debug/MultiLineEditBox")
        medit.horizontalScrollbarEnabled=True
        medit.wordwrapped = True
        medit.text="Press Up/Down arrows at command line for Ogre commands, enter to Run Command"

        # Load our Script.layout
        fr2=cegui.WindowManager.getSingleton().loadWindowLayout("Script.layout","Script/")


        # add layout controls to main window
        sheet.addChildWindow(fr1)
        sheet.addChildWindow(fr2)
        
    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = 0.8, 0.8, 0.8
        #sceneManager.setSkyDome(True, 'Examples/CloudySky', 4.0, 8.0)

        light = sceneManager.createLight('MainLight')
        light.position = 20, 80, 50
        self.viewport.backgroundColour = (0.5,0.5,0.5)

        self._createGUI()


    def _createCamera(self):
        self.camera = self.sceneManager.createCamera("PlayerCam")
        self.camera.nearClipDistance = 5

    def _createFrameListener(self):
        self.frameListener = CEGUIFrameListener(self.renderWindow, self.camera, self.sceneManager)
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
        ta = GEUIApplication()
        ta.go()
    except ogre.OgreException, e:
        print e
