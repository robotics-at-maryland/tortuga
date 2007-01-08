# this code is in the public domain
# 

#
#   Script for Testing Tab Control Properties

from pyogre import ogre,cegui
import SampleFramework



def _PointHack(x, y):
    return cegui.Vector2(x, y)
cegui.Point = _PointHack


def CreateControl(widget_name,name,parent,pos,size,min,max,text='',showframe=False):

    control = cegui.WindowManager.getSingleton().createWindow(widget_name, name)
    #control.metricsMode = cegui.Absolute #Metrics are specified as whole pixels
    parent.addChildWindow(control)
    control.position = cegui.Point(pos[0],pos[1])
    control.size = cegui.Size(size[0],size[1])
    control.minimumSize = cegui.Size(min[0],min[1])
    control.maximumSize = cegui.Size(max[0],max[1])   
    control.text = text
    control.frameEnabled = showframe
    return control	


class CEGUIFrameListener(SampleFramework.FrameListener):

    def __init__(self, renderWindow, camera, sceneManager):
        SampleFramework.FrameListener.__init__(self, renderWindow, camera)

        self.keepRendering = True   # whether to continue rendering or not

    def _setupInput(self):
        self.eventProcessor = ogre.EventProcessor()
        self.eventProcessor.initialise(self.renderWindow)
        self.eventProcessor.startProcessingEvents()

        # register as a listener for events
        self.eventProcessor.addKeyListener(self)
        self.eventProcessor.addMouseListener(self)
        self.eventProcessor.addMouseMotionListener(self)

    def frameStarted(self, evt):
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


    def keyPressed(self, evt):
        if evt.key == ogre.KC_ESCAPE:
            self.keepRendering = False
        system = cegui.System.getSingleton()
        system.injectKeyDown(evt.key)
        system.injectChar(evt.keyChar)
        evt.consume()

    def keyReleased(self, evt):
        system = cegui.System.getSingleton()
        system.injectKeyUp(evt.key)

    def keyClicked(self, evt):
        pass

    def _convertOgreButtonToCegui(self,evt):
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



# Event Handling
def onTabSelection(args):
    win=args.window
    print win.name,"------- Tab Control Selected Tab Index is ",win.selectedTabIndex 

    # Return the Window which is the first child of the tab at index position index. 
    childwin =  win.getTabContentsAtIndex(win.selectedTabIndex)
    print "Child Name is i.e Tab ", childwin.name, childwin.text


class GEUIApplication(SampleFramework.Application):

    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.guiRenderer=0
        self.system =0
        self.ec = 0

    def _createGUI(self):

        # initiaslise CEGUI Renderer
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RENDER_QUEUE_OVERLAY, False, self.sceneManager)		
        self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane

        # load WindowsLook Cegui Scheme
        cegui.SchemeManager.getSingleton().loadScheme("WindowsLookSkin.scheme")
        self.system.defaultMouseCursor=("WindowsLook", "MouseArrow")
        cegui.FontManager.getSingleton().createFont("Iconified-12.font")

        # load layout
        sheet = cegui.WindowManager.getSingleton().createWindow("DefaultWindow", "root_wnd")
        self.system.guiSheet = sheet

        # control colours
        blue = cegui.colour(0.0, 0.0, 1.0, 0.5)
        red  = cegui.colour(1.0, 0.0, 0.0, 0.5)
        green  = cegui.colour(0.0, 1.0, 0.0, 1.0)
        blue_back = cegui.colour(0.0, 0.0, 1.0, 1.0)
        red_back = cegui.colour(1.0, 0.0, 0.0, 1.0)




        # Tab Control
        tabControl= CreateControl("WindowsLook/TabControl" ,  "TabControl" ,sheet ,[0.40,0.50],[0.55,0.40],[0.01,0.01],[1.0,1.0],"a TabControl")
        tabControl.tabHeight=0.15
        page1= CreateControl("WindowsLook/TabPane"    ,  "Page1" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"Radio Button")
        page2= CreateControl("WindowsLook/TabPane"    ,  "Page2" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"Combo Box")
        page3= CreateControl("WindowsLook/TabPane"    ,  "Page3" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"List Box")
        page4= CreateControl("WindowsLook/TabPane"    ,  "Page4" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"Static")
        page5= CreateControl("WindowsLook/TabPane"    ,  "Page5" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"Spinner")
        page6= CreateControl("WindowsLook/TabPane"    ,  "Page6" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"ScrolledPane")

        # Page 1
        button = CreateControl("WindowsLook/RadioButton"    , "Button1",page1 ,[0.10,0.20],[0.30,0.10],[0.01,0.01],[1.0,1.0],"Option1")
        button.groupID=1
        button = CreateControl("WindowsLook/RadioButton"    , "Button2",page1 ,[0.10,0.30],[0.30,0.10],[0.01,0.01],[1.0,1.0],"Option2")
        button.groupID=1
        button.selected = True


        # Page 2
        combobox=CreateControl("WindowsLook/Combobox" , "A Combo Box"  ,page2 ,[0.05,0.15],[0.8,0.5],[0.01,0.01],[1.0,1.0],"a Combo Box")

        # Page 3 List Box
        CreateControl("WindowsLook/StaticText" , "List label"  ,page3 ,[0.05,0.05],[0.80,0.15],[0.01,0.01],[1.0,1.0],"Editless ListBox",0)
        listbox = CreateControl("WindowsLook/Listbox", "a List Box"  ,page3 ,[0.05,0.20],[0.80,0.70],[0.01,0.01],[1.0,1.0],"ListBox",1)

        # Page 5, cant use createcontrol text is numeric
        CreateControl("WindowsLook/StaticText" , "Label2s"  ,page5 ,[0.10,0.10],[0.40,0.15],[0.01,0.01],[1.0,1.0],"Float Pt")
        spinner = cegui.WindowManager.getSingleton().createWindow("WindowsLook/Spinner", "Test2")
        page5.addChildWindow(spinner)
        spinner.position = cegui.Point(0.5,0.1)
        spinner.size = cegui.Size(0.20,0.15)



        # ---------------------- Events Test Change Tab ------------------------ 

        page1.id = 1
        page2.id = 3
        page3.id = 5
        page4.id = 7
        page5.id = 9
        page6.id = 11


        self.ec1= tabControl.subscribeEvent(tabControl.EventSelectionChanged, onTabSelection)

        # Set Tab Selection by Index OK
        tabControl.selectedTabIndex =2  #OK

        # Set Tab Selection by ID, Name OK
        tabControl.setSelectedTab("Page6") # OK
        tabControl.setSelectedTab(9)  # OK

        win = tabControl.getTabContents(9)  # OK
        print win.name
        #win = tabControl.getTabContents("Page6")  # OK
        print win.name
         
        # ---------------------- Events Test Change Tab ------------------------ 


    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = 0.25, 0.25, 0.25
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
