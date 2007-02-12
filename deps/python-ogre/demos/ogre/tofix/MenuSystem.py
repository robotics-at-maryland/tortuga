import Ogre as ogre
import OIS as OIS

''' 
this is intended as simple menu system, appropiate for games where the need is for simple
button menus, basically you register a cursor when you create the mouse listener
        self.MouseListener = ms.MouseListener( self.cursorpanel ) 
Then you create MouseTarget:
        target=ms.MouseTarget('Button1', 
                                ogre.OverlayManager.getSingleton().getOverlayElement('Menu/Button1', False), 
                                self.Button1Click )
then you register the targets, basically areas you want call backs on
        self.MouseListener.registerContainer( target )
        
it is up to you to create and display the overlay (the menu itself) and to disable the 'camera' movement 
- ie. set MenuMode=True (in the sample framework)
'''

# 	self.startButton = DirectButton(image = "Textures/StartButton.png", 
#                        command = self.startGame, scale = (0.6, 1, 0.04), relief = None)
# 	self.startButton.setTransparency(1)	
# 	self.startButton.setPos(0, 0, 0.03)



# we need a class to hold the panel that will respond to mouse clicks
#
class MouseTarget():
    def __init__ (self, name, container,  clickfunction ):
        self.name=name
        self.container = container
        self.x=self.container.getLeft()+self.container.getParent().getLeft()
        self.y=self.container.getTop()+self.container.getParent().getTop()
        self.h=self.container.getHeight()
        self.w=self.container.getWidth()
        self.x1=self.x+self.w
        self.y1=self.y+self.h
        self.status=-1
        self.ClickFunction = clickfunction  # call this when we get clicked..
        
    def mouseMoved ( self, Event):
        if self.status == 0:  # means we are getting a mouseMoved event that is outside our space
            self.pressed = False
            self.status = -1
        else:
            self.status= 1  # set it so we know that we are now in our region
        return True
    
    def mousePressed( self, Event, Button):
        self.pressed=True
        return True
    
    def mouseReleased ( self, Event, Button):
        if self.pressed==True:
            # it's a click event, down and up
            self.ClickFunction()
            self.pressed = False
        return True
        
#
# here are our mouse listeners
#
class MouseListener ( OIS.MouseListener ):
    def __init__ (self, cursor):
        self.cursor = cursor
        OIS.MouseListener.__init__(self)
        self.containers = {}
        
    def PrintEvent ( self, Msg, Event, Button ):
        State = Event.get_state()
        print Msg, "abX=%s abY=%s abZ=%s" % (  State.abX, State.abY, State.abZ)
        print dir(State)
        if Button:
            print Button, dir(Button) # only works on Right button??
    def registerContainer ( self, mtarget) :
        self.containers[mtarget.name] = mtarget
           
    def mouseMoved( self, Event ):
        state =Event.get_state()
        left =  state.width - (state.abX)
        top = state.height - (state.abY)
        left =  (state.abX)
        top = (state.abY)
        self.cursor.setPosition (left, top)
        for k in self.containers: 
            c = self.containers[k]
            if (c.x <= left and c.x1 >= left) and (c.y <= top and c.y1 >= top ):
                c.mouseMoved(Event)
            elif c.status == 1 : # the mouse target still thinks it has focus, which means we just exited
                c.status=0
                c.mouseMoved(Event)
        return True
        
    def mousePressed( self, Event, Button ):
        state =Event.get_state()
        left =  (state.abX)
        top = (state.abY)
        for k in self.containers: 
            c = self.containers[k]
            if (c.x <= left and c.x1 >= left) and (c.y <= top and c.y1 >= top ):
                c.mousePressed(Event, Button)
        return True
        
    def mouseReleased( self, Event, Button ):
        state =Event.get_state()
        left =  (state.abX)
        top = (state.abY)
        for k in self.containers: 
            c = self.containers[k]
            if (c.x <= left and c.x1 >= left) and (c.y <= top and c.y1 >= top ):
                c.mouseReleased(Event, Button)
        return True