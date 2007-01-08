# this code is in the public domain


import Ogre as ogre
import CEGUI as cegui
#import ogre
#import cegui
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


# event column moved
def onEventListColumnMoved(args):
    #print args.window.name
    #print args.window.columnCount
    pass


class CEGUIFrameListener(SampleFramework.FrameListener):

    def __init__(self, renderWindow, camera, sceneManager):

        self.raySceneQuery=0
        self.connections=[] 
 
        SampleFramework.FrameListener.__init__(self, renderWindow, camera)

        self.sceneManager = sceneManager
        self.keepRendering = True   # whether to continue rendering or not
        self.mCamera = camera
        self.mCount = 0
        self.mCurrentObject = None
        self.mLMouseDown = False
        self.mRMouseDown = False

        # Reduce move speed
        self.mMoveSpeed= 50
        self.mRotateSpeed=100
        self.meshes=[]
        self.items=[]
        self.mesh_name="robot.mesh" 

        self.raySceneQuery = sceneManager.createRayQuery(ogre.Ray(self.mCamera.position,
                                                                    ogre.Vector3.NEGATIVE_UNIT_Y))
        self.blue = cegui.colour(0.0, 0.0, 1.0, 0.5)
        self.red  = cegui.colour(1.0, 0.0, 0.0, 1.0)
        self.green  = cegui.colour(0.0, 1.0, 0.0, 1.0)
        self.blue_back = cegui.colour(0.0, 0.0, 1.0, 1.0)
        self.red_back = cegui.colour(1.0, 0.0, 0.0, 1.0)
        self.loadMesList()

    # Event Handling
    def onMenuSelection(self,args):
        pass
        #win=args.window
        #print win.name,"------- Poup/Menu Item Selection" 

    def _setupInput(self):
        self.eventProcessor = ogre.EventProcessor()
        self.eventProcessor.initialise(self.renderWindow)
        self.eventProcessor.startProcessingEvents()

        # register as a listener for events
        self.eventProcessor.addKeyListener(self)
        self.eventProcessor.addMouseListener(self)
        self.eventProcessor.addMouseMotionListener(self)

        p1 = cegui.WindowManager.getSingleton().getWindow("Move")
        self.connections.append(p1.subscribeEvent(p1.EventClicked, self.onMenuSelection))
        l=cegui.WindowManager.getSingleton().getWindow("AddedItems")
        self.connections.append(l.subscribeEvent(l.EventSelectionChanged, self.onSelectAddedItem))
        l=cegui.WindowManager.getSingleton().getWindow("MeshList")
        self.connections.append(l.subscribeEvent(l.EventSelectionChanged, self.onSelectItem))

    # load meshes in combo
    def loadMesList(self):
        try:
            red  = cegui.colour(1.0, 0.0, 0.0, 0.5)
            # get list meshes 
            meshList = ogre.ResourceGroupManager.getSingleton().findResourceNames("General", "*.mesh")
        #print "Ptr is",dir(ogre.StringVectorPtr)		
        #print "Ptr is",dir(ogre.StringVector)	
        #print type(meshList)		
        #print "MeshList is",meshList.size()
        #print dir(meshList)
            iterator = meshList.get()
            meshes = [ iterator.__getitem__(j) for j in range(iterator.size()) ]
            # iterate through list and add to combo
            for i, v in enumerate(meshes):
                # add Row to specified MCList
                self.addRow("MeshList",v,"Mesh")
        except:
            print "Error Loading Mesh List"

    # generic routine to add a row with 2 columns to a multi column list box 
    def addRow(self,MCcontrol_name,col1,col2):

        # get the MCListBox
        multi=cegui.WindowManager.getSingleton().getWindow(MCcontrol_name)

        # Add items to the MultiCoulumList
        item = cegui.ListboxTextItem(col1)
        item.selectionBrushImage = "WindowsLook", "Background"
        item.selectionColours = self.red
        item.textColours =self.blue_back
        row = multi.addRow(item,0)
        # you must hold a copy of this so it isn't destroyed while CEGUI uses it
        self.items.append(item)

        item2 = cegui.ListboxTextItem(col2)
        item2.selectionBrushImage = "WindowsLook", "Background"
        item2.selectionColours = self.red
        item2.textColours =self.blue_back
        multi.setItem(item2, 1, row)
        # you must hold a copy of this so it isn't destroyed while CEGUI uses it
        self.items.append(item2)

    # check if our mouse over a gui control
    def isMouseGUI(self):
        # get window mouse cureently is in
        win_mouse = cegui.System.getSingleton().windowContainingMouse
        # defualt window is where mesh is displayed
 	if (win_mouse):
            if (win_mouse.type=="DefaultWindow"): return False
        return True


    def frameStarted(self, evt):
    
        camPos = self.mCamera.position
        cameraRay=ogre.Ray( ogre.Vector3(camPos.x, 5000.0, camPos.z), ogre.Vector3.NEGATIVE_UNIT_Y )
        self.raySceneQuery.ray=cameraRay
        self.raySceneQuery.sortByDistance= False
         
        # perform the scene query 
        for queryResult in self.raySceneQuery.execute():
            if queryResult.worldFragment is not None:

                terrainHeight = queryResult.worldFragment.singleIntersection.y
                if ((terrainHeight + 10.0) > camPos.y):
                    self.mCamera.position=( camPos.x, terrainHeight + 10.0, camPos.z )
                break
        if not (self.keepRendering):
            # disconnect events that we created in listener to avoid memory leaks
            for i in range(len(self.connections)):
                c = self.connections[i]
                c.disconnect    

        return self.keepRendering
    
		
    def onLeftPressed(self, evt):
        if ( self.mCurrentObject ):
            self.mCurrentObject.showBoundingBox=False

        mouseRay = self.mCamera.getCameraToViewportRay( evt.x, evt.y)
        self.raySceneQuery.ray = mouseRay
        self.raySceneQuery.sortByDistance=True

        multi=cegui.WindowManager.getSingleton().getWindow("MeshList")
        #popup=cegui.WindowManager.getSingleton().getWindow("PopupMenu1")
        #popup.position=cegui.Point(evt.x,evt.y)
        #popup.alpha=0.7 

        for queryResult in self.raySceneQuery.execute():
            if ( queryResult.movable and queryResult.movable.name!= "tile[0][0,2]"):
                #print "Moveable",queryResult.movable.name
                self.mCurrentObject = queryResult.movable.parentSceneNode
                #popup.visible=True
                break
            elif ( queryResult.worldFragment ):
                #print queryResult.worldFragment.singleIntersection
                #print queryResult.worldFragment.fragmentType                    
                self.mCount+=1
                name=self.mesh_name 
                ent = self.sceneManager.createEntity( "Robot%d" % self.mCount, name )
                self.mCurrentObject = self.sceneManager.rootSceneNode.createChildSceneNode( name + "%d" % self.mCount + "Node", queryResult.worldFragment.singleIntersection )
                self.mCurrentObject.attachObject(ent)
                self.mCurrentObject.scale = ( 0.1, 0.1, 0.1)
                #trans = ent.boundingBox.center + queryResult.worldFragment.singleIntersection 
                #self.mCurrentObject.translate(ent.boundingBox.minimum	)
                #self.mCurrentObject.translate(ogre.Vector3(0.0,50.0,0))
                #print "query",queryResult.worldFragment.singleIntersection,
                #print "Centre",ent.boundingBox.center				
                #print "Minimum",ent.boundingBox.minimum				
				
                # add Row to specified MCList
                self.addRow('AddedItems',self.mCurrentObject.name,"Mesh")
                break


        # if an object is selected show its bounding box
        if ( self.mCurrentObject ):
            self.mCurrentObject.showBoundingBox=True

    def onRightPressed(self, evt):
        cegui.MouseCursor.getSingleton().hide( )

    def onRightReleased(self, evt):
        cegui.MouseCursor.getSingleton().show( )

    def onLeftReleased(self, evt):
        popup=cegui.WindowManager.getSingleton().getWindow("PopupMenu1")
        #popup.visible=False


    def onRightDragged(self, evt):
        self.mCamera.yaw(ogre.Degree(-evt.relX * self.mRotateSpeed) )
        self.mCamera.pitch(ogre.Degree(-evt.relY * self.mRotateSpeed))

    def onLeftDragged(self, evt):
        #print "Events X Y",evt.x,evt.y
        mouseRay = self.mCamera.getCameraToViewportRay( evt.x, evt.y)
        self.raySceneQuery.ray=mouseRay
        self.raySceneQuery.sortByDistance=False
        popup=cegui.WindowManager.getSingleton().getWindow("PopupMenu1")
        popup.position=cegui.Point(evt.x,evt.y)

        for queryResult in self.raySceneQuery.execute():
            if (queryResult.worldFragment):
                self.mCurrentObject.position=( queryResult.worldFragment.singleIntersection )
                #print self.mCurrentObject.position
                break

    def mouseDragged(self, evt):
        self.mouseMoved(evt)
        # dragging the left/right mouse button.
        if ( self.mLMouseDown ):  self.onLeftDragged(evt)
        elif ( self.mRMouseDown ): self.onRightDragged(evt)

    def mousePressed(self, evt):
        # gui control selected
        if (self.isMouseGUI()):
            button = self._convertOgreButtonToCegui(evt)
            cegui.System.getSingleton().injectMouseButtonDown(button)
            return
        # left mouse pressed
        if (evt.buttonID & ogre.MouseEvent.BUTTON0_MASK):
            self.mLMouseDown=True
            self.onLeftPressed(evt)
        # right mouse pressed
        if (evt.buttonID & ogre.MouseEvent.BUTTON1_MASK):
            self.mRMouseDown=True
            self.onRightPressed(evt)

    def mouseReleased(self, evt):
        # gui control selected
        if (self.isMouseGUI()):
            button = self._convertOgreButtonToCegui(evt)
            cegui.System.getSingleton().injectMouseButtonUp(button)
            return
        if (evt.buttonID & ogre.MouseEvent.BUTTON0_MASK):
            self.mLMouseDown=False
            self.onLeftReleased(evt)
        # Right Mouse released
        if (evt.buttonID & ogre.MouseEvent.BUTTON1_MASK):
            self.mRMouseDown=False
            self.onRightReleased(evt)
	
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
            #print "ESCAPE IS PRESSED"
            # do or own cleaning up, clear out MultiColumn Lists
            multi=cegui.WindowManager.getSingleton().getWindow("AddedItems")
            multi.resetList() 
            multi=cegui.WindowManager.getSingleton().getWindow("MeshList")
            multi.resetList() 
            # delete MC items we stored earlier
            del self.items

            # disconnect events that we created in listener to avoid memory leaks
            for i in range(len(self.connections)):
                c = self.connections[i]
                c.disconnect    
            # stop rendering
            self.keepRendering = False

        #if evt.key == ogre.KC_PGUP:
        #    self.camera.moveRelative(ogre.Vector3(0.0,0.0,-5.0))
        #if evt.key == ogre.KC_PGDOWN:
        #    self.camera.moveRelative(ogre.Vector3(0.0,0.0,5.0))

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
	# Convert ogre button to cegui button
        if (evt.buttonID & ogre.MouseEvent.BUTTON0_MASK):
            print "Left Button"
        elif (evt.buttonID & ogre.MouseEvent.BUTTON1_MASK):
            return cegui.RightButton	
        elif (evt.buttonID & ogre.MouseEvent.BUTTON2_MASK):
            return cegui.MiddleButton
        elif (evt.buttonID & ogre.MouseEvent.BUTTON3_MASK):
            return cegui.X1Button
        return cegui.LeftButton
    #--------------------------------------------------------------------------------------------------
    #    Using this routine instead causes an invalid  page fault message requiring
	#   reboot to get rid of it 
    #---------------------------------------------------------------------------------------------------	
    #def onSelectAddedItem(self,args):
    #    print "Text is",args.window.text
    #    if (args.win.getFirstSelectedItem()):
    #        print "here"
		
    #--------------------------------------------------------------------------------------------------
    # This is the correct code and works fine
    #---------------------------------------------------------------------------------------------------	
    def onSelectAddedItem(self,args):
        #print "Text is",args.window.text
        try:
            if (args.window.getFirstSelectedItem()):
                name=args.window.getFirstSelectedItem().text
                #print "NAME IS",name
                self.mCurrentObject.showBoundingBox=False		
                self.mCurrentObject=self.sceneManager.getSceneNode(name)
                self.mCurrentObject.showBoundingBox=True
        except:		
		    print "Error Selecting Item"

    def onSelectItem(self,args):
        t=self.mesh_name
        try:
            if (args.window.getFirstSelectedItem()):
                self.mesh_name=args.window.getFirstSelectedItem().text
        except:		
            print "Error Selecting Item"
            self.mesh_name=t
			
class GEUIApplication(SampleFramework.Application):

    def __init__(self):
        SampleFramework.Application.__init__(self)
        self.guiRenderer=0
        self.system =0
        self.item=0 
        self.item2=0
        #self.connections=[] 

    def _createGUI(self):

 ##       self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow,ogre.RENDER_QUEUE_OVERLAY, False, self.sceneManager)
       ## setup GUI system
        self.guiRenderer = cegui.OgreCEGUIRenderer(self.renderWindow, 
            ogre.RENDER_QUEUE_OVERLAY, False, 3000, self.sceneManager) 
  
        self.system = cegui.System(self.guiRenderer)
        cegui.Logger.getSingleton().loggingLevel = cegui.Insane

        # Load Cegui Scheme
        cegui.SchemeManager.getSingleton().loadScheme("WindowsLookSkin.scheme")
        self.system.setDefaultMouseCursor ( "WindowsLook", "MouseArrow")
##        font=cegui.FontManager.getSingleton().createFont("Iconified-12.font")

#         # scale our fonts for native resolution 
#         font.autoScalingEnabled =True
#         font.nativeResolution=cegui.Size(self.renderWindow.width,self.renderWindow.height)

        # Load Layout root window layout
        sheet = cegui.WindowManager.getSingleton().loadWindowLayout("Default.layout")
        #sheet = cegui.WindowManager.getSingleton().loadWindowLayout("Demo8.layout")
        self.system.guiSheet = sheet

#         # Load our DebugConsole.layout
#         fr1=cegui.WindowManager.getSingleton().loadWindowLayout("DebugConsole.layout","Debug/")
#         fr2=cegui.WindowManager.getSingleton().loadWindowLayout("Script.layout","Script/")

#         # Set some additional control characteristics
#         edit=cegui.WindowManager.getSingleton().getWindow("Debug/EditBox")
#         medit=cegui.WindowManager.getSingleton().getWindow("Debug/MultiLineEditBox")
#         medit.horizontalScrollbarEnabled=True
#         medit.wordwrapped = True
#         medit.text="Press Up/Down arrows at command line for Ogre commands, enter to Run Command"

#         # Multi Column List Items Added
#         multi = CreateControl("WindowsLook/MultiColumnList", "AddedItems",sheet,[0.05,0.1],[0.35,0.3],[0.01,0.01],[1.0,1.0],'')
#         multi.addColumn('Column1',0,0.5)   #name,id,width (%widget)
#         multi.addColumn('Column2',1,0.5)   #name,id,width (%widget)
#         #multi.addColumn('Column3',1,0.5)   #name,id,width (%widget)
#         multi.showVertScrollbar = True
#         multi.alpha=0.6 
#        
#         # Multi Column List Mesh
#         multi = CreateControl("WindowsLook/MultiColumnList", "MeshList",sheet,[0.55,0.1],[0.35,0.3],[0.01,0.01],[1.0,1.0],'')
#         multi.addColumn('Object',0,0.5)   #name,id,width (%widget)
#         multi.addColumn('Type',1,0.5)   #name,id,width (%widget)
#         multi.showVertScrollbar = True
#         multi.alpha=0.6 

#         # create menu bar
#         menubar = CreateControl("WindowsLook/Menubar"    ,  "MenuBar" ,sheet ,[0.0,0.0],[1.0,0.05],[0.01,0.01],[1.0,1.0],"Test")
#         # create menu items
#         menuitem1= CreateControl("WindowsLook/MenuItem"    ,  "MenuItem1" ,menubar ,[0.15,0.05],[0.5,0.5],[0.01,0.01],[1.0,1.0],"Menu1")
#         menuitem2= CreateControl("WindowsLook/MenuItem"    ,  "MenuItem2" ,menubar ,[0.15,0.05],[0.5,0.5],[0.01,0.01],[1.0,1.0],"Menu2")
#         menuitem3= CreateControl("WindowsLook/MenuItem"    ,  "MenuItem3" ,menubar ,[0.15,0.05],[0.5,0.5],[0.01,0.01],[1.0,1.0],"Menu3")

#         # create popup menu 1
#         p1= CreateControl("WindowsLook/PopupMenu"    ,  "PopupMenu1" ,sheet ,[0.15,0.15],[0.2,0.25],[0.01,0.01],[1.0,1.0],"Mesh")
#         #popupmenuitem1= CreateControl("WindowsLook/PopupMenuItem"    ,  "Move" ,p1 ,[0.15,0.06],[0.2,0.05],[0.01,0.01],[1.0,1.0],"Move")
#         popupmenuitem1= CreateControl("WindowsLook/MenuItem"    ,  "Move" ,p1 ,[0.15,0.06],[0.2,0.05],[0.01,0.01],[1.0,1.0],"Move")
#         popupmenuitem2= CreateControl("WindowsLook/MenuItem"    ,  "Scale" ,p1 ,[0.15,0.11],[0.2,0.05],[0.01,0.01],[1.0,1.0],"Scale")
#         #p1.alpha=0.6
#         #popupmenuitem1.alpha=0.4
#         #p1.openPopupMenu()
#         menuitem1.popupMenu=p1
		
        #self.guiRenderer.setTargetSceneManager(0)

    def _chooseSceneManager(self):
        self.sceneManager = self.root.createSceneManager(ogre.ST_EXTERIOR_CLOSE,"Test")
 
    def _createScene(self):
        sceneManager = self.sceneManager

        #set ambient light 
        sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)
        sceneManager.setSkyDome(True, 'Examples/CloudySky', 4.0, 8.0)
        sceneManager.setWorldGeometry( "terrain.cfg" )

        light = sceneManager.createLight('MainLight')
        light.position = ogre.Vector3(20, 80, 50)

        self.camera.position=ogre.Vector3( 40, 100, 580 )
        self.camera.pitch( ogre.Degree(-30) )
        self.camera.yaw( ogre.Degree(-45) )

#         # infinte far clip plane?
#         if self.root.renderSystem.capabilities.hasCapability(ogre.RSC_INFINITE_FAR_PLANE):
#             self.camera.farClipDistance = 0

#         else:
# 	        self.camera.farClipDistance = 1000 
        self._createGUI()


    def _createCamera(self):
        self.camera = self.sceneManager.createCamera("PlayerCam")
        self.camera.nearClipDistance = 5
        #self.camera.yaw(ogre.Degree(60.0))

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
