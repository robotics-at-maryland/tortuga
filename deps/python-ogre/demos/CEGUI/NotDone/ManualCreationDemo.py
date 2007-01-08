# this code is in the public domain

#-----------------------------------------
#  Updates:
#     New event attribute names
#     Follows the SampleFramework.FrameListener,SampleFramework.Application format
#     and same structure as pyogres Tutorial5.py  
#     Hopefully got the deletion order correct
#  Could you comment on :  
#     list items within a def , you now must hold onto a reference for any ListboxTextItem you create
#     the implications creating events within a def
#
#
#----------------------------------------

from pyogre import ogre,cegui
#import ogre,cegui
import SampleFramework

def _PointHack(x, y):
    return cegui.Vector2(x, y)
cegui.Point = _PointHack


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
def onQuit(args):
    print '---Quit---'
    print `args`
    
def OnObjectSelection(args):
    print `args`
    window = args.window
    print `window`

class GEUIApplication(SampleFramework.Application):

    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.guiRenderer=0
        self.system =0

        # Events / Item List
        # Is this what we must do ?
        self.ec = 0
        self.item = 0
        self.item1 = 0
        self.item2 = 0
        self.item3 = 0
        self.item4 = 0
        self.item5 = 0
        self.item6 = 0
        self.item7 = 0
        self.item8 = 0

    def _createGUI(self):
        # Initiaslise CEGUI Renderer
        #self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow)
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RENDER_QUEUE_OVERLAY, False, self.sceneManager)
        self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane

        # Load Cegui Scheme
        cegui.SchemeManager.getSingleton().loadScheme("TaharezLookSkin.scheme")
        self.system.defaultMouseCursor = ("TaharezLook", "MouseArrow")
        cegui.FontManager.getSingleton().createFont("Iconified-12.font")

        # Load Default CEGUI Window
        sheet = cegui.WindowManager.getSingleton().createWindow("DefaultWindow", "root_wnd")
        self.system.guiSheet = sheet

        # Control Colours
        blue = cegui.colour(0.0, 0.0, 1.0, 0.5)
        red  = cegui.colour(1.0, 0.0, 0.0, 0.5)
        green  = cegui.colour(0.0, 1.0, 0.0, 1.0)
        blue_back = cegui.colour(0.0, 0.0, 1.0, 1.0)
        red_back = cegui.colour(1.0, 0.0, 0.0, 1.0)


        #Create Control
        # -----------------
        # type,name,parent,pos[x,y],size[x,y],min[x,y],max[x,y],text,border=0 
        # -----------------

        # Push Buttons / Labels
        quitButton = CreateControl("TaharezLook/Button"     , "QuitButton"  ,sheet ,[0.85,0.05],[0.1,0.036],[0.01,0.01],[1.0,1.0],"Quit")

        printButton = CreateControl("TaharezLook/Button"    ,  "PrintButton" ,sheet ,[0.75 ,0.05],[0.1,0.036],[0.01,0.01],[1.0,1.0],"Print")
        CreateControl("TaharezLook/StaticText" , "Multi label" ,sheet ,[0.45,0.10],[0.45,0.05],[0.01,0.01],[1.0,1.0],"MultiLineEditBox")

        # ProgressBar
        CreateControl("TaharezLook/ProgressBar", "a Progress Bar"  ,sheet ,[0.1,0.95],[0.60,0.02],[0.01,0.01],[1.0,1.0],"ProgressBar",0).step = 0.05 

        # Multi Line Edit Box
        editBox = CreateControl("TaharezLook/MultiLineEditbox", "an edit control",sheet,[0.45,0.15],[0.45,0.35],[0.01,0.01],[1.0,1.0],'')
        editBox.id = 1
        editBox.maxTextLength = 10
        editBox.wordwrapped = True	
        editBox.text = ''

        # Multi Column List
        multi = CreateControl("TaharezLook/MultiColumnList", "a MultiColumn List",sheet,[0.05,0.1],[0.35,0.3],[0.01,0.01],[1.0,1.0],'')
        multi.addColumn('Column1',0,0.5)   #name,id,width (%widget)
        multi.addColumn('Column2',1,0.5)   #name,id,width (%widget)
        multi.showVertScrollbar = True

        # Vertical Scroll Bar
        CreateControl("TaharezLook/VerticalScrollbar", "a Vertical Scroll Bar",sheet,[0.00,0.02],[0.02,1.0],[0.01,0.01],[1.0,1.0],'')

        # Frame with Children 
        frame = CreateControl("TaharezLook/FrameWindow", "a Frame Window",sheet,[0.05,0.5],[0.35,0.3],[0.01,0.01],[1.0,1.0],'Test Frame',1)
        CreateControl("TaharezLook/Checkbox"    , "Checkbox1",frame ,[0.10,0.30],[0.30,0.10],[0.01,0.01],[1.0,1.0],"Check Box").enabled=True
        CreateControl("TaharezLook/Editbox", "an edit box",frame,[0.35,0.45],[0.55,0.15],[0.01,0.01],[1.0,1.0],'')
        CreateControl("TaharezLook/StaticText" , "Edit label"  ,frame ,[0.10,0.45],[0.25,0.15],[0.01,0.01],[1.0,1.0],"Edit Me")

        # Tab Control
        tabControl= CreateControl("TaharezLook/TabControl" ,  "TabControl" ,sheet ,[0.40,0.50],[0.55,0.40],[0.01,0.01],[1.0,1.0],"a TabControl")
        tabControl.tabHeight = 0.15
        page1= CreateControl("TaharezLook/TabPane"    ,  "Page1" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"Radio Button")
        page2= CreateControl("TaharezLook/TabPane"    ,  "Page2" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"Combo Box")
        page3= CreateControl("TaharezLook/TabPane"    ,  "Page3" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"List Box")
        page4= CreateControl("TaharezLook/TabPane"    ,  "Page4" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"Static")
        page5= CreateControl("TaharezLook/TabPane"    ,  "Page5" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"Spinner")
        page6= CreateControl("TaharezLook/TabPane"    ,  "Page6" ,tabControl ,[0.01,0.01],[0.99,0.99],[0.01,0.01],[1.0,1.0],"ScrolledPane")

        # Page 1
        button = CreateControl("TaharezLook/RadioButton"    , "Button1",page1 ,[0.10,0.20],[0.30,0.10],[0.01,0.01],[1.0,1.0],"Option1")
        button.groupID=1
        button = CreateControl("TaharezLook/RadioButton"    , "Button2",page1 ,[0.10,0.30],[0.30,0.10],[0.01,0.01],[1.0,1.0],"Option2")
        button.groupID=1
        button.selected = True


        # Page 2
        combobox=CreateControl("TaharezLook/Combobox" , "A Combo Box"  ,page2 ,[0.05,0.15],[0.8,0.5],[0.01,0.01],[1.0,1.0],"a Combo Box")
        CreateControl("TaharezLook/ComboEditbox" , "Combo Edit Box"  ,page2 ,[0.05,0.55],[0.8,0.15],[0.01,0.01],[1.0,1.0],"a Combo Edit Box")

        # Page 3 List Box
        CreateControl("TaharezLook/StaticText" , "List label"  ,page3 ,[0.05,0.05],[0.80,0.15],[0.01,0.01],[1.0,1.0],"Editless ListBox",0)
        listbox = CreateControl("TaharezLook/Listbox", "a List Box"  ,page3 ,[0.05,0.20],[0.80,0.70],[0.01,0.01],[1.0,1.0],"ListBox",1)

        # Page 4 Static Text / Static Image

        # Static Text Background / Horiz Bars
        staticText=CreateControl("TaharezLook/StaticText", "TestLabel"  ,page4 ,[0.50,0.50],[0.4,0.35],[0.01,0.01],[1.0,1.0],"Test Horizontal Bar on Static Text")
        staticText.frameColours = green
        staticText.frameEnabled = True
        img = cegui.ImagesetManager.getSingleton().getImageset("TaharezLook").getImage("ListboxSelectionBrush")
        staticText.backgroundImage = img
        staticText.backgroundColours = cegui.ColourRect(blue,red,blue,red) #colrect
        staticText.horizontalScrollbarEnabled = True 
        staticText.textColours= cegui.colour(1.0,0.0,0.0) # red Text
        staticText.requestRedraw

        # Static Image 
        staticImage=CreateControl("TaharezLook/StaticImage", "TestImage"  ,page4 ,[0.10,0.10],[0.3,0.85],[0.01,0.01],[1.0,1.0],"Test Image")
        staticImage.frameColours = green
        staticImage.frameEnabled = True       
        img = cegui.ImagesetManager.getSingleton().getImageset("TaharezLook").getImage("ListboxSelectionBrush")
        staticImage.image = img
        staticImage.imageColours = blue_back 
        # Static Image Alignment LeftAligned,RightAligned,Centred,Justified
        staticImage.horizontalFormatting=cegui.Centred      #OK
        staticImage.requestRedraw

        # Page 5, cant use createcontrol text is numeric
        CreateControl("TaharezLook/StaticText" , "Label2s"  ,page5 ,[0.10,0.10],[0.40,0.15],[0.01,0.01],[1.0,1.0],"Float Pt")
        spinner = cegui.WindowManager.getSingleton().createWindow("TaharezLook/Spinner", "Test2")
        page5.addChildWindow(spinner)
        spinner.position = cegui.Point(0.5,0.1)
        spinner.size = cegui.Size(0.20,0.15)

        colourframe= CreateControl("TaharezLook/FrameWindow" , "colour_Frame" ,page6 ,[0.15 ,0.15],[5.8,5.86],[0.01,0.01],[1.0,1.0],"Colours",1)
        scrolledPanel = CreateControl("TaharezLook/ScrollablePane" , "ScrolledPanel" ,page6 ,[0.20,0.40],[0.50,0.50],[0.01,0.01],[1.0,1.0],"Test",1)
        scrolledPanel.vertScrollBars=True
        scrolledPanel.horzScrollBars=True
        scrolledPanel.addChildWindow(colourframe)
        scrolledPanel.contentPaneAutoSized = True
        scrolledPanel.horzStepSize=2.0
        scrolledPanel.autoSize=False
    

        # Should this be self.item, self.item2 etc ?
        # Add items to Listbox
        item = cegui.ListboxTextItem("Test",1)
        item.selectionColours = blue
        item.selectionBrushImage = "TaharezLook", "ListboxSelectionBrush"
        listbox.addItem(item)
        # you must hold a copy of this so it isn't destroyed while CEGUI uses it
        self.item = item

        item2 = cegui.ListboxTextItem("Test2",2)
        item2.selectionColours = blue
        item2.selectionBrushImage = "TaharezLook", "ListboxSelectionBrush"
        listbox.addItem(item2)
        # you must hold a copy of this so it isn't destroyed while CEGUI uses it
        self.item2 = item2
        listbox.showHorzScrollbar=True

        # Add items to the combobox
        item3 = cegui.ListboxTextItem("Combo Test1",1)
        item3.selectionColours = red
        item3.selectionBrushImage = "TaharezLook", "ListboxSelectionBrush"
        combobox.addItem(item3)
        combobox.subscribeEvent("ListSelectionAccepted", OnObjectSelection)
        # you must hold a copy of this so it isn't destroyed while CEGUI uses it
        self.item3 = item3

        item4 = cegui.ListboxTextItem("Combo Test1",2)
        item4.selectionColours = red
        item4.selectionBrushImage = "TaharezLook", "ListboxSelectionBrush"
        combobox.addItem(item4)
        # you must hold a copy of this so it isn't destroyed while CEGUI uses it
        self.item4 = item4


        # Add items to the MultiCoulumList
        item5 = cegui.ListboxTextItem("Multi Col 1")
        item5.selectionBrushImage = "TaharezLook", "ListboxSelectionBrush"
        item5.selectionColours = red
        row = multi.addRow(item5,0)
        # you must hold a copy of this so it isn't destroyed while CEGUI uses it
        self.item5 = item5

        item6 = cegui.ListboxTextItem("Multi Col 2")
        item6.selectionBrushImage = "TaharezLook", "ListboxSelectionBrush"
        item6.selectionColours = blue
        multi.setItem(item6, 1, row)
        # you must hold a copy of this so it isn't destroyed while CEGUI uses it
        self.item6 = item6

        item7 = cegui.ListboxTextItem("Next Row ")
        item7.selectionBrushImage = "TaharezLook", "ListboxSelectionBrush"
        item7.selectionColours = red
        row = multi.addRow(item7,0)
        # you must hold a copy of this so it isn't destroyed while CEGUI uses it
        self.item7 = item7

        item8 = cegui.ListboxTextItem("Next Col")
        item8.selectionBrushImage = "TaharezLook", "ListboxSelectionBrush"
        item8.selectionColours = blue
        multi.setItem(item8, 1, row)
        # you must hold a copy of this so it isn't destroyed while CEGUI uses it
        self.item8 = item8

        #Event Handling
        # Which one should it be?
        self.ec= quitButton.subscribeEvent(quitButton.EventClicked, onQuit)

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
