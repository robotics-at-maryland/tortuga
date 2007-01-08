# this code is in the public domain
# 

#
#   Script for Testing Menu Bar/Popup Event
#       EventListContentsChanged
#       EventPopupOpened/Closed
#       EventClicked
#
#   The above works the EventListContentsChanged is triggered when an item
#   associated with menu/popup is changed eg text. I was half expecting it 
#   to be triggered when selecting an item but it wasn't.
#
#   Note on creating Menus:
#   ---------------------------------
#       The Item List Base stuff has the same problem as ListBoxItem i.e:
#       eg.
#           popupmenu.addItem(popupmenuitem1)   
#           popupmenu.removeItem(popupmenuitem1)   
#           etc.
#       Error
#         def addItem(*args): return _cegui.ItemListBase_addItem(*args)
#         TypeError: argument number 2: a 'ItemEntry *' is expected, 
#         'PySwigObject(_p_CEGUI__MenuItem)' is received
#
#
#   You can overcome this by creating Menus as per loading from layouts:
#   ---------------------------------
#    WindowsLook:
#      a) a menubar, creating Menubaritems and partent to the menubar
#      b) For dropdowns create a popup , create PopupMenuItems, parent the popupto to a Menubaritem.
#      c) For further levels create a popup,create PoupMenuitems, parent the popupto a PopupMenuItem
#
#    WindowsLookSkin:
#       same method replace MenuItem for menubaritems,poupMenuitems.
#

import Ogre as ogre
import CEGUI as cegui
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

from CEGUI_framework import *   ## we need the OIS version of the framelistener etc


# Event Handling
def onMenuOpened(args):
    win=args.window
    print win.name,"--------- Opened Menu" 

# Event Handling
def onMenuClosed(args):
    win=args.window
    print win.name,"------- Closed Menu" 

# Event Handling
def onMenuChanged(args):
    win=args.window
    print win.name,"------- The Menu/Popup has Changed" 

# Event Handling
def onMenuSelection(args):
    win=args.window
    print win.name,"------- Poup/Menu Item Selection" 



class GEUIApplication(SampleFramework.Application):

    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.guiRenderer=0
        self.system =0
        self.ec = 0
        self.ec1 = 0
        self.ec2 = 0
        self.ec3 = 0
        self.ec4 = 0
        self.ec5 = 0
        self.ec6 = 0
        self.ec7 = 0
        self.ec8 = 0
        self.ec9 = 0

    def _createGUI(self):
        CEGUI.Imageset.setDefaultResourceGroup("imagesets")
        CEGUI.Font.setDefaultResourceGroup("fonts")
        CEGUI.Scheme.setDefaultResourceGroup("schemes")
        CEGUI.WidgetLookManager.setDefaultResourceGroup("looknfeels")
        CEGUI.WindowManager.setDefaultResourceGroup("layouts")
        CEGUI.ScriptModule.setDefaultResourceGroup("lua_scripts")

        # initiaslise CEGUI Renderer
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RENDER_QUEUE_OVERLAY, False, 4000, self.sceneManager)	
        self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane

        # load WindowsLook Cegui Scheme
        cegui.SchemeManager.getSingleton().loadScheme("WindowsLookSkin.scheme")
        self.system.setDefaultMouseCursor("WindowsLook", "MouseArrow")
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


        # Menu Bar Events Test
        # =================================================
        # MenuBar
        #   menuItem1               - EventPopupOpened/Closed/EventListContentsChanged
        #      p1                   - EventListContentsChanged
        #       popupmenuitem1      - EventClicked       
        #       popupmenuitem2
        #       popupmenuitem3
        #
        #    menuItem2
        #      p2                    - EventPopupOpened/Closed
        #       popupmenuitem4
        #       popupmenuitem5       
        #       p3                   - EventListContentsChanged
        #       popupmenuitem6       - EventClicked
        #       popupmenuitem7
        #    menuItem3
        # =================================================


        # create menu bar
        menubar = CreateControl("WindowsLook/Menubar"    ,  "MenuBar" ,sheet ,[0.0,0.0],[1.0,0.05],[0.01,0.01],[1.0,1.0],"Test")
        # create menu items
        menuitem1= CreateControl("WindowsLook/MenuItem"    ,  "MenuItem1" ,menubar ,[0.15,0.05],[0.5,0.5],[0.01,0.01],[1.0,1.0],"Menu1")
        menuitem2= CreateControl("WindowsLook/MenuItem"    ,  "MenuItem2" ,menubar ,[0.15,0.05],[0.5,0.5],[0.01,0.01],[1.0,1.0],"Menu2")
        menuitem3= CreateControl("WindowsLook/MenuItem"    ,  "MenuItem3" ,menubar ,[0.15,0.05],[0.5,0.5],[0.01,0.01],[1.0,1.0],"Menu3")

        # doesn't work though we have a PopupMenuItem ????
        #mi1= CreateControl("WindowsLook/MenuItem"    ,  "Item1" ,menuitem1 ,[0.15,0.15],[0.2,0.25],[0.01,0.01],[1.0,1.0],"File")


        # create popup menu 1
        p1= CreateControl("WindowsLook/PopupMenu"    ,  "PopupMenu1" ,sheet ,[0.15,0.15],[0.2,0.25],[0.01,0.01],[1.0,1.0],"File")
        popupmenuitem1= CreateControl("WindowsLook/MenuItem"    ,  "Popup1" ,p1 ,[0.15,0.06],[0.2,0.05],[0.01,0.01],[1.0,1.0],"PopItem1")
        popupmenuitem2= CreateControl("WindowsLook/MenuItem"    ,  "Popup2" ,p1 ,[0.15,0.11],[0.2,0.05],[0.01,0.01],[1.0,1.0],"PopItem2")
        poupmenuitem3= CreateControl("WindowsLook/MenuItem"    ,  "Popup3" ,p1 ,[0.15,0.16],[0.2,0.05],[0.01,0.01],[1.0,1.0],"PopItem3")


        # create popup menu 2
        p2= CreateControl("WindowsLook/PopupMenu"    ,  "PopupMenu2" ,sheet ,[0.15,0.15],[0.2,0.25],[0.01,0.01],[1.0,1.0],"File")
        popupmenuitem4= CreateControl("WindowsLook/MenuItem"    ,  "Popup4" ,p2 ,[0.15,0.06],[0.2,0.05],[0.01,0.01],[1.0,1.0],"PopItem4")
        popupmenuitem5= CreateControl("WindowsLook/MenuItem"    ,  "Popup5" ,p2 ,[0.15,0.11],[0.2,0.05],[0.01,0.01],[1.0,1.0],"PopItem5")

        # create popup menu 3
        p3= CreateControl("WindowsLook/PopupMenu"    ,  "PopupMenu3" ,sheet ,[0.15,0.15],[0.2,0.25],[0.01,0.01],[1.0,1.0],"File")
        popupmenuitem6= CreateControl("WindowsLook/MenuItem"    ,  "Popup6" ,p3 ,[0.15,0.06],[0.2,0.05],[0.01,0.01],[1.0,1.0],"PopItem6")
        popupmenuitem7= CreateControl("WindowsLook/MenuItem"    ,  "Popup7" ,p3 ,[0.15,0.11],[0.2,0.05],[0.01,0.01],[1.0,1.0],"PopItem7")


        # attach popup menus to menu bar items
        menuitem1.popupMenu=p1
        menuitem2.popupMenu=p2

        # attach popup menu to popuitem
        popupmenuitem5.popupMenu=p3

        # print event member names
        print menubar.EventPopupOpened
        print menubar.EventPopupClosed
        print menubar.EventListContentsChanged
        print p1.EventPopupOpened 
        print p1.EventPopupClosed
        print p1.EventListContentsChanged

        # events menuitem 
        self.ec1= menubar.subscribeEvent(menubar.EventPopupOpened, onMenuOpened,"")
        self.ec2= menubar.subscribeEvent(menubar.EventPopupClosed, onMenuClosed,"")
        self.ec3= menubar.subscribeEvent(menubar.EventListContentsChanged, onMenuChanged,"")


        # A MenuItem attached to this menu opened a PopupMenu. 
        # An item in popup2 opens popup3
        self.ec4= p2.subscribeEvent(p2.EventPopupOpened, onMenuOpened)
        self.ec5= p2.subscribeEvent(p2.EventPopupClosed, onMenuClosed)

        # events change item in a popup
        self.ec6= p1.subscribeEvent(p1.EventListContentsChanged, onMenuChanged)
        self.ec7= p3.subscribeEvent(p3.EventListContentsChanged, onMenuChanged)


        # events individual popup items
        self.ec8= popupmenuitem1.subscribeEvent(popupmenuitem1.EventClicked, onMenuSelection)
        self.ec9= popupmenuitem6.subscribeEvent(popupmenuitem6.EventClicked, onMenuSelection)


        # test list contents changed popup/menu
        popupmenuitem1.text = "Event Clicked 1"
        popupmenuitem6.text = "Event Clicked 2"
        menuitem3.text="Change 3"

        

    def _createScene(self):
        sceneManager = self.sceneManager
        sceneManager.ambientLight = ogre.ColourValue(0.25, 0.25, 0.25)
        self._createGUI()


    def _createCamera(self):
        self.camera = self.sceneManager.createCamera("PlayerCam")
        self.camera.nearClipDistance = 5

    def _createFrameListener(self):
        self.frameListener = GuiFrameListener(self.renderWindow, self.camera, self.sceneManager)
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
    except ogre.Exception, e:
        print e
