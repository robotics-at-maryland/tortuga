from aiStateMachine import aiStateMachine
import math

framesChecked = 50
searchAngle = 5
creepSpeed = 1
depthChanges = 0.1

class buoyMachine(aiStateMachine):
    def __init__(self):
        aiStateMachine.__init__(self)
        self.vision.forward.redLightDetectOn()
        self.lightDetector = self.vision.forward.RedLightDetector
        
    def startState(self,args,interFuncs,interStates):
        self.changeState(self.moveIn,None,[self.noRed],[self.redLost])
        
    def moveIn(self,args,interFuncs,interStates):
        x = lightDetector.getX()
        y = lightDetector.getY()       
    
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
        
    def initPointAtRed(self,args,interFuncs,interStates):
        x = lightDetector.getX()
        self.lastX = x
        self.direction = 1
        self.changeState(self.pointAtRed,None,[self.noRed],[self.redLost])
        self.controller.setSpeed(creepSpeed)
        
    def pointAtRed(self,args,interFuncs,interStates):
        x = lightDetector.getX()
        if x < 0.5:
            self.controller.yawVehicle(searchAngle)
        else if x > 0.5:
            self.controller.yawVehicle(-1 * searchAngle)
        else if x == 0.5:
            self.changeState(self.redDepth,None,[self.noRed],[self.redLost])
            
    def redDepth(self,args,interFuncs,interStates):
        y = lightDetector.getY()
        depth = self.controller.getDepth()
        if y < 0.5:
            newDepth = depth - depthChanges
            self.controller.setDepth(newDepth)
        else if y > 0.5:
            newDept = depth + depthChanges
            self.controller.setDepth(newDepth)
        else if x == 0.5:
            self.exit()
        
    def redLost(self):
        self.lostCount += 1
        if self.lightDetector.found():
            self.changeState(self.pointAtRed,None,[self.noRed],[self.redLost])
        else:
            self.count += 1
        if self.lostCount >= framesChecked:
            self.exit()
        
    def noRed(self):
        if not self.lightDetector.found():
            self.lostCount = 0
            self.controller.setSpeed(0)
            return True
        else:
            return False