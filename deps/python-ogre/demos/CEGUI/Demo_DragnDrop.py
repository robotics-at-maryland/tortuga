# this code is in the public domain
# 
#  This demo shows simple drag and drop. Two frames each contain a grid
#  of Static Images. Each (slot) static image is assigned dragndrop events.
#  Move the static image containing the icon to different slots.
#
#  This is based on a dragndrop example provided by Crazy Eddie:
#  http://www.cegui.org.uk/modules/newbb/viewtopic.php?topic_id=1040&forum=2&post_id=5852#forumpost5852
#
#
#  Shows:  
#      Simple DragnDrop
#      Creating a list of events (hope this is correct)
#      Mouse Enter/Leaves Events & Change Mouse Cursor       
#      Setting properties by Strings     
#      Example control creation using UDIM
#      Simple Tooltip Text / injectTimePulse


import Ogre as ogre
import CEGUI as cegui
import SampleFramework

from CEGUI_framework import *


CONTAINER_POS = 0.05
CONTAINER_SIZE = 0.90

def createUVector2( x, y):
    return CEGUI.UVector2(cegui.UDim(x,0), cegui.UDim(y,0))
    
def CreateControl(widget_name,name,parent,pos,size,text='',showframe=False):

    control = cegui.WindowManager.getSingleton().createWindow(widget_name, name)
    parent.addChildWindow(control)
    control.setPosition (createUVector2( pos[0],pos[1]) )
    control.setSize(createUVector2(size[0], size[1]))
    control.minimumSize = cegui.Size(0.01,0.01)
    control.maximumSize = cegui.Size(1.0,1.0)
    control.text = text
    control.frameEnabled = showframe
    return control	



# Drag and Drop Events
def handleDragEnter(args):

    # set frame colour for window on drag enter
    args.window.setProperty("FrameColours", "tl:FF00FF00 tr:FF00FF00 bl:FF00FF00 br:FF00FF00")
    return True

def handleDragLeave(args):
    # change to mouse move cursor 
    cegui.System.getSingleton().setDefaultMouseCursor("TaharezLook", "MouseMoveCursor")

    # reset frame colours
    args.window.setProperty("FrameColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
    return True

def handleDragDropped(args):

    # change to normal mouse cursor 
    cegui.System.getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow")
    # set frame colour and add image to frame static grid
    args.window.setProperty("FrameColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
    args.window.addChildWindow(args.dragDropItem)
    args.dragDropItem.position = CEGUI.UVector2(cegui_reldim (CONTAINER_POS), cegui_reldim (CONTAINER_POS))
    args.dragDropItem.setSize(CEGUI.UVector2(cegui_reldim(CONTAINER_SIZE), cegui_reldim(CONTAINER_SIZE)))
    return True

def onMouseEnters(args):
    #print "You have Mouse Entery" 
    # change to mouse move cursor 
    cegui.System.getSingleton().setDefaultMouseCursor("TaharezLook", "MouseMoveCursor")
    

def onMouseLeaves(args):
    #print "You have Mouse Left" 
    # change to mouse move cursor 
    cegui.System.getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow")




class GEUIApplication(SampleFramework.Application):

    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.guiRenderer=0
        self.system =0
        self.img = []    # static images for frames
        self.ec = []     # events for each static image
        self.ec1 = 0     # events for icon image change mouse cursor
        self.ec2 = 0

    def _createGUI(self):
        ## set the default resource groups to be used
        CEGUI.Imageset.setDefaultResourceGroup("imagesets")
        CEGUI.Font.setDefaultResourceGroup("fonts")
        CEGUI.Scheme.setDefaultResourceGroup("schemes")
        CEGUI.WidgetLookManager.setDefaultResourceGroup("looknfeels")
        CEGUI.WindowManager.setDefaultResourceGroup("layouts")
        CEGUI.ScriptModule.setDefaultResourceGroup("lua_scripts")


        # initiaslise CEGUI Renderer
        print self.renderWindow
        print self.sceneManager
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RenderQueueGroupID.RENDER_QUEUE_OVERLAY, False, 3000, self.sceneManager)
        self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane

        # load TaharezLook scheme
        cegui.SchemeManager.getSingleton().loadScheme("TaharezLook.scheme")
        self.system.setDefaultMouseCursor("TaharezLook", "MouseArrow")
        cegui.FontManager.getSingleton().createFont("Iconified-12.font")

        # default layout
        sheet = cegui.WindowManager.getSingleton().createWindow("DefaultWindow", "root_wnd")
        self.system.setGUISheet (sheet)

        # control colours
        blue = cegui.colour(0.0, 0.0, 1.0, 0.5)
        red  = cegui.colour(1.0, 0.0, 0.0, 0.5)
        green  = cegui.colour(0.0, 1.0, 0.0, 1.0)
        blue_back = cegui.colour(0.0, 0.0, 1.0, 1.0)
        red_back = cegui.colour(1.0, 0.0, 0.0, 1.0)


        # setup tooltip
        self.system.setDefaultTooltip("TaharezLook/Tooltip" )
        tip = self.system.getDefaultTooltip()

        # displayTime
        #     the number of seconds the tooltip should be displayed for before it automatically
        #     de-activates itself. 
        #     0 indicates that the tooltip should never timesout and auto-deactivate.
        # hoverTime
        #     the number of seconds the mouse should hover stationary over the target window 
        #     before the tooltip gets activated. 
        # fadeTime
        #     number of seconds that should be taken to fade the tooltip into and out of 
        #     visibility.  

        #tip.displayTime=0.80
        tip.hoverTime = 0.10
        tip.fadeTime=1.0

        # an edit box 
        editBox = CreateControl("TaharezLook/MultiLineEditbox", "an edit control",sheet,[0.10,0.80],[0.80,0.80],'')
        editBox.text="The above static text control was created using UDIM"
        editBox.text+="UDIM is the new 'unified' co-ord system replacement for relative/absolute coords"
        editBox.text+=" The basic concept is UDIM(scale,offset) where:"
        editBox.text+="         scale is the relative component usually 0.0 to 1.0"
        editBox.text+="         offset is the absolute component in pixels "
        editBox.readOnly=True 

        # create static text box UDIM
        stat =cegui.WindowManager.getSingleton().createWindow("TaharezLook/StaticText", "Label")
        stat.windowHeight=cegui.UDim(0.05,0)         # %window parents height,offset  pixels parent 
        stat.windowWidth=cegui.UDim(0.80,0)          # %window parents width, offset  pixels parent 
        stat.windowXPosition = cegui.UDim(0.10,0)    # %position parent window width,offset  pixels parent 
        stat.windowYPosition = cegui.UDim(0.05,0)    # %position parent window height,offset  pixels parent  
        stat.text = "Drag Above Icon to different cells in frame window"
        img = cegui.ImagesetManager.getSingleton().getImageset("TaharezLook").getImage("ListboxSelectionBrush")
        stat.backgroundImage = img
        stat.backgroundColours = cegui.ColourRect(red,red,red,red) #colrect
        stat.textColours =  (1.0,1.0,1.0) # white
        stat.frameColours = blue
        stat.horizontalFormatting=cegui.Centred # LeftAligned 
  
        #sheet.addChildWindow(stat)

        # creates first frame window
        frs=cegui.WindowManager.getSingleton().createWindow("TaharezLook/FrameWindow", "Rucksack")
        frs.setPosition (CEGUI.UVector2(cegui_reldim (0.03), cegui_reldim (0.05)))
        frs.setSize(CEGUI.UVector2(cegui_reldim(0.45), cegui_reldim(0.6)))
        frs.text = "Test Frame"
        sheet.addChildWindow(frs)

        # creates second frame window
        feq=cegui.WindowManager.getSingleton().createWindow("TaharezLook/FrameWindow", "Equipped Items")
        feq.setPosition (CEGUI.UVector2(cegui_reldim (0.5), cegui_reldim (0.05)))
        feq.setSize(CEGUI.UVector2(cegui_reldim(0.45), cegui_reldim(0.6)))
        feq.text = "Test Frame 2"
        sheet.addChildWindow(feq)
       
        # add a number of 'slots' to each frame window
        # the slots will act as targets for drag/drop  
        # the slots ust normal StaticText widgets with the relevant event handlers attached

        # create 4x4 staic image grid for each frame
        cols = 4
        rows = 4
        deltax=1.0/cols
        deltay=1.0/rows

        for x in [frs,feq]: 
            for xp in range(cols):
                for yp in range(rows):
                    name = "Slot" + str(xp) + str(yp) + x.name.c_str()
                    self.img.append(CreateControl("TaharezLook/StaticImage",name,x,[xp*deltax,yp*deltay],[deltax,deltay],'Image1',1)) 

        # create events for each slot
        for slot in self.img: 
            self.ec.append(slot.subscribeEvent(slot.EventDragDropItemEnters, handleDragEnter,""))
            self.ec.append(slot.subscribeEvent(slot.EventDragDropItemLeaves, handleDragLeave,""))
            self.ec.append(slot.subscribeEvent(slot.EventDragDropItemDropped, handleDragDropped,""))


        # create a drag/drop container
        item=cegui.WindowManager.getSingleton().createWindow("DragContainer", "theItem")
        item.position = CEGUI.UVector2(cegui_reldim (CONTAINER_POS), cegui_reldim (CONTAINER_POS))
        item.setSize(CEGUI.UVector2(cegui_reldim(CONTAINER_SIZE), cegui_reldim(CONTAINER_SIZE)))



        # create a static iamge as drag container's contents and parent to drag container
        itemIcon=cegui.WindowManager.getSingleton().createWindow("TaharezLook/StaticImage", "theContainer")
        itemIcon.setPosition (CEGUI.UVector2(cegui_reldim (0.0), cegui_reldim (0.0)) )
        itemIcon.setSize(CEGUI.UVector2(cegui_reldim(1.0), cegui_reldim(1.0)))

        # set image
        itemIcon.setProperty("Image", "set:TaharezLook image:CloseButtonNormal")
        # disable to allow inputs to pass through.
        itemIcon.disable
        itemIcon.tooltipText ="Drag Me"
       
        # add itemIcon to drag drop container 
        item.addChildWindow(itemIcon)

        # events to change mouse cursor
        self.ec1= item.subscribeEvent(item.EventMouseEnters,onMouseEnters, "")        
        self.ec2= item.subscribeEvent(item.EventMouseLeaves,onMouseLeaves, "")        

        # set starting slot for the item
        startslot=self.img[0]
        startslot.addChildWindow(item)  


    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = ogre.ColourValue(0.25, 0.25, 0.25)
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
     
class CEGUIFrameListener(GuiFrameListener):

    def __init__(self, renderWindow, camera, sceneManager):
        GuiFrameListener.__init__(self, renderWindow, camera, sceneManager)

        self.keepRendering = True   # whether to continue rendering or not

    def frameStarted(self, evt):
        # injectTimePulse needed when using tooltips
	cegui.System.getSingleton().injectTimePulse( evt.timeSinceLastFrame)
        return GuiFrameListener.frameStarted(self,evt)



if __name__ == '__main__':
    try:
        ta = GEUIApplication()
        ta.go()
    except ogre.Exception, e:
        print e
