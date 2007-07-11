import module.Module as Module
import ai.state_machine as StateMachine
import module.ModuleManager as ModuleManager
import ai.Start.GoGate as Gate
import Pipeline.GoPipeline as Pipeline
import ModifiedPipeline.GoPipeLight as ModifiedPipeline
import Bin.GoBin as Bin
import SpiralToBin.GoCoveredBin as Bin2
import Treasure.GoTreasure as Treasure
class AI(Module):
    def __init__(self, config):
        self.aiStates={
                     "toGate":self.toGate,
                     "toRedLight1":self.toRedLight1,
                     "toPipeline":self.toPipeline,
                     "toBin1":self.toBin1,
                     "toPipelineAndLight":self.toPipelineAndLight,
                     "toBin2":self.toBin2,
                     "toTreasure":self.toTreasure,
                     "missionComplete":self.missionComplete
                     }
        self.state="toGate"
        self.stateMachine=StateMachine(vehicle)
#        self.moduleManager=ModuleManager.get()
        self.vehicle=ModuleManager.get().get_module("Vehicle")
        self.controller=ModuleManager.get().get_module("Controller")
        self.vision=ModuleManager.get().get_module("Vision");
        self.speed=0
        self.relinquishingControl=false
        #Everything else should have already been initialized.
        Module.__init__(self,config)
    
    def setSpeed(self, newSpeed):
        self.vehicle.setSpeed(newSpeed)
    
    def navigateAboveBin(self,x,y):
        #aim for x and y between .48 and .52, may have to aim differently
        #depending on where the marker droppers are located.
        if (not self.controller.isReady()):
            return
        
        if (x<0 or y<0 or x>1 or y>1):
            print "NavigateAboveBin is being called with out of range x or y coordinates, this is an error, they should be 0.0 to 1.0 in image coordinates from the DOWNWARD FACING CAMERA ONLY"
            return 
        
        if (y<.1):
            self.controller.setSpeed(5)
        elif (y<.2):
            self.controller.setSpeed(4)
        elif (y<.3):
            self.controller.setSpeed(3)
        elif (y<.4):
            self.controller.setSpeed(2)
        elif (y<.48):
            self.controller.setSpeed(1)            
        elif (y<.52):
            self.controller.setSpeed(0)
            depth=self.controller.getDepth()
            if (depth<11):
                self.vehicle.setDepth(depth+.2)#move down a little bit, make sure we're directly above whatever it is we're lookin at, Its supposed to be a bin
        elif (y<.6):
            self.vehicle.setSpeed(-1)
        elif (y<.7):
            self.vehicle.setSpeed(-2)
        elif (y<.8):
            self.vehicle.setSpeed(-2)
        elif (y<.9):
            self.vehicle.setSpeed(-2)
        elif (y<=1.0):
            self.vehicle.setSpeed(-3)
            depth=self.controller.getDepth()
            if (depth>8):
                self.vehicle.setDepth(depth-.2)#move up a little bit to increase our field of view, hopefully preventing us from losing this thing

        if (not self.controller.isReady()):#don't mess with angles while we're trying to turn.
            return
        
        guessChange=0.0       
        if (x<.1):
            guessChange+=3
        if (x<.2):
            guessChange+=2
        if (x<.3):
            guessChange+=2
        if (x<.4):
            guessChange+=2
        if (x<.48):
            guessChange+=.5
        
        if (x>.52):
            guessChange-=.5
        if (x>.6):
            guessChange-=2
        if (x>.7):
            guessChange-=2
        if (x>.8):
            guessChange-=2
        if (x>.9):
            guessChange-=3
        self.controller.yawVehicle(guessChange)    

    def navigateAbove(self, x, y, angle):
        #angle should be a number from 0 to pi for normal operation
        #y will be used to determine speed settings
        #x and angle will be combined to determine how to yaw the vehicle.
        
        if (x<0 or y<0 or x>1 or y>1):
            print "NavigateAbove is being called with out of range x or y coordinates, this is an error, they should be 0.0 to 1.0 in image coordinates from the DOWNWARD FACING CAMERA ONLY"
            return 
        
        if (angle==-10):
            print "Angle is equal to error code for orange pipeline, but x and y are not -1, this is likely a SERIOUS error, and so I will just return."
            return
        
        if (angle<-3.13 and angle>-3.142):
            print "Angle was -approximately- negative pi, a negative angle should never have occured, but I will treat it as though it were positive pi (which negative pi is)"
            angle=(-angle)
            
        if (angle<0 or angle > 3.142):
            print "Angle was less than zero, or greater than pi only use the angle as calculated by vision's hough transform wrapping, this function automatically rearranges start and end of discovered "
            print "lines such that the one further from the camera is the end, thus giving only positive angles"
            return
        if (y<.1):
            self.controller.setSpeed(7)
        elif (y<.2):
            self.controller.setSpeed(6)
        elif (y<.3):
            self.controller.setSpeed(5)
        elif (y<.4):
            self.controller.setSpeed(4)
        elif (y<.5):
            self.controller.setSpeed(3)
        elif (y<.6):
            self.vehicle.setSpeed(2)
        elif (y<.7):
            self.vehicle.setSpeed(1)
        elif (y<.8):
            self.vehicle.setSpeed(0)
        elif (y<.9):
            self.vehicle.setSpeed(-1)
        elif (y<=1.0):
            self.vehicle.setSpeed(-1)
            depth=self.controller.getDepth()
            if (depth>7):
                self.vehicle.setDepth(depth-.2)#move up a little bit to increase our field of view, hopefully preventing us from losing this thing, and maybe even bringing into view the next pipeline, which must be awfully far away :(
        
        if (not self.controller.isReady()):#don't mess with angles while we're trying to turn.
            return

        guessChange=0
        angleDeg=angle*180.0/3.142
        guessChange=((angleDeg-90.0)/ 5.0)
        
        if (x<.1):
            guessChange+=3
        if (x<.2):
            guessChange+=2
        if (x<.3):
            guessChange+=2
        if (x<.4):
            guessChange+=2
            
        if (x>.6):
            guessChange-=2
        if (x>.7):
            guessChange-=2
        if (x>.8):
            guessChange-=2
        if (x>.9):
            guessChange-=3
        
        self.controller.yawVehicle(guessChange)
        
    def navigateToward(self, x, y):
        #yawVehicle(double degrees) Positive is left, negative is right
        if (not self.controller.isReady()):
            return
        
        if (x<0 or y<0):
            print "NavigateToward is being called with negative x and y in error, these should be 0.0 to 1.0 in image coordinates FROM THE FORWARD FACING CAMERA ONLY"
        
        if (0<=y and y<.25):
            print "NavigateToward has been passed a y at the very top of the screen, this is likely in error, so I am ignoring it."
            return
             #This y is likely invalid, as is our x probably
     
        if (0.0<=x and x<0.20):
            self.controller.yawVehicle(5.0)
            self.controller.setSpeed(0)
        elif (0.20<=x and x<.40):
            self.controller.yawVehicle(2.5)
            self.controller.setSpeed(1)
        elif (.40<=x and x<.46):
            self.controller.yawVehicle(1.0)
            self.controller.setSpeed(3)
        elif (.46<=x and x<.49):
            self.controller.yawVehicle(.5)
            self.controller.setSpeed(5)
        elif (.49<=x and x<.50):
            self.controller.yawVehicle(.1)
            self.controller.setSpeed(10)
        elif (.50< x and x<.51):
            self.controller.yawVehicle(-.1)
            self.controller.setSpeed(10)
        elif (.51<=x and x<.54):
            self.controller.yawVehicle(-.5)
            self.controller.setSpeed(5)
        elif (.54<=x and x<.60):
            self.controller.yawVehicle(-1.0)
            self.controller.setSpeed(3)
        elif (.60<=x and x<.80):
            self.controller.yawVehicle(-2.5)
            self.controller.setSpeed(1)
        elif (.80<=x and x<1.0):
            self.controller.yawVehicle(-5.0)
            self.controller.setSpeed(0)
            
        if (.75<=y and y<1.00):
            self.controller.setDepth(self.controller.getDepth()+.2)#positive is downwards, this sets us to go deeper
        else:
            self.controller.setDepth(10.0)#This better be feet

    def nextState(self):
        if (self.state=="none"):
            self.state="toGate"
        elif (self.state=="toGate"):
            self.state="toRedLight1"
        elif (self.state=="toRedLight1"):
            self.state="toPipeline"
        elif (self.state=="toPipeline"):
            self.state="toBin1"
        elif (self.state=="toBin1"):
            self.state="toPipelineAndLight"
        elif (self.state=="toPipelineAndLight"):
            self.state="toBin2"
        elif (self.state=="toBin2"):
            self.state="toTreasure"
        if (self.state=="toTreasure"):
            self.state="missionComplete"
            
    def update(self,timeStep):
        self.stateMachine.operate()
    
    def returnControl(self):
        self.nextState()
        self.stateMachine.set_states(self.aiStates)
        self.stateMachine.change_state("self.state")
        
    def toGate(self):
        if (not self.state=="toGate"):
            print "Was toGate called in error?  The state was not set to toGate, setting state to toGate and running anyway."
            print "state was "
            print self.state
        self.state="toGate"
        gate = Gate()
        
    def toRedLight1(self):
        light=RedLight()
        
    def toPipeline(self):
        pipeline=Pipeline()
    
    def toBin1(self):
        bin=Bin()
        
    def toPipelineAndLight(self):
        pipeLight=ModifiedPipeline()
        
    def toBin2(self):
        bin2=CoveredBin()
        
    def toTreasure(self):
        treasure=Treasure()
    
    def missionComplete(self):
        self.vehicle.turnOff() #hehe or just crash if this is unimplemented, same difference
        
    
