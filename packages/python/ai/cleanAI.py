from module import Module, IModule
from core import implements
from ai.state_machine import state_machine as StateMachine
import ai.Movement as Movement
import time

import ai.AIModel as AIModel

class AI(Module):
    implements(IModule)
    
    def __init__(self, veh, config):
        self.startState = "shutdown"    #state that the vehicle enters after waiting to start
        
        self.aiStates = {
                    "shutdown":self.shutdown,
                    "testFunctionality":self.testFunctionality,
                    "testSpeed":self.testSpeed,
                    "spin360":self.spin360,
                    "spin90":self.spin90,
                    "testDepth":self.testDepth,
                    "initToGate":self.initToGate,
                    "toGate":self.toGate,
                    "headAtGate":self.headAtGate,
                    "gateFound":self.gateFound,
                    "waitForStart":self.waitForStart,
                    "shutdown":self.shutdown,
                    "testFunctionality":self.testFunctionality,
                    "testSpeed":self.testSpeed,
                    "spin360":self.spin360,
                    "spin90":self.spin90,
                    "testDepth":self.testDepth,
                    "initToGate":self.initToGate,
                    "toGate":self.toGate,
                    "headAtGate":self.headAtGate,
                    "gateFound":self.gateFound,
                    "initFirstRedLight":self.initFirstRedLight,
                    "waitForReadyThenScan":self.waitForReadyThenScan,
                    "firstLightScan":self.firstLightScan,
                    "findRedLight":self.findRedLight,
                    "foundFirstRedLight":self.foundFirstRedLight,
                    "initializePipeline":self.initializePipeline,
                    "waitThenPipeScan":self.waitThenPipeScan,
                    "pipeScan":self.pipeScan,
                    "findPipe":self.findPipe,
                    "lostPipe":self.lostPipe,
                    "makeSureBinFound":self.makeSureBinFound,
                    "binFound":self.binFound,
                    "binAimInitialize":self.binAimInitialize,
                    "aimForBin":self.aimForBin,
                    "doneWithBin1":self.doneWithBin1,
                    "intializePipeLight":self.intializePipeLight,
                    "waitThenPipeLight":self.waitThenPipeLight,
                    "scanPipeLight":self.scanPipeLight,
                    "findPipeLight":self.findPipeLight,
                    "lostPipeLight":self.lostPipeLight,
                    "chaseLight":self.chaseLight,
                    "donePipeLight":self.donePipeLight,
                    "zigZagSearchTimeInit":self.zigZagSearchTimeInit,
                    "zigZagSearchTime":self.zigZagSearchTime
                    }

        self.stateMachine = StateMachine()
        self.stateMachine.state = "waitForStart"
        self.stateMachine.set_states(self.aiStates)
        self.model = AIModel.model()
        self.vehicle = veh
        #self.vision = self.model.vision
        self.controller = self.model.controller
        self.complexControl = Movement.control()
        
        Module.__init__(self,config)
        
    def update(self,time):
        self.time = time
        self.stateMachine.operate()
    
    ###############################################################        
    ##                        General States                         ##
    
    def waitForStart(self):
        start = self.vehicle.startStatus()
	print "start status:",start
        self.vehicle.printLine(0,"Waiting to start...")
        if start == 1:
            self.stateMachine.change_state(self.startState)
            self.vehicle.printLine(0,"Fuck you Steve!")

    def shutdown(self):
        self.controller.setDepth(1)
        self.controller.setSpeed(0)
        self.vehicle.safeThrusters()
    
    #                                                              #
    ############################################################### 
    
    ###############################################################        
    ##                        Search Patterns                    ##
    
    def zigZagSearchTimeInit(self):
        self.searchForTime = 50
        
        self.zagTime = 10   #time per zag in seconds  
        self.zagAngle = 45  #zag angle in degrees
        self.searchSpeed = 3
        self.lastZag = time.time()
        self.startZagTime = self.lastZag
        self.turn = 1
        
        self.controller.setSpeed(self.searchSpeed)
        self.stateMachine.change_state("zigZagSearchTime")
        
    def zigZagSearchTime(self):
        if (time.time() - self.startZagTime) >= self.searchForTime:
            self.change_state("shutdown")
        else:
            results = self.complexControl.zigZag(self.lastZag,self.zagTime,self.zazAngle,self.turn)
            self.lastZag = results[0]
            self.turn = results[1]
        
      
    #                                                              #
    ###############################################################  
    
    ###############################################################        
    ##                        Simple Gate                        ##
    
    driveSpeed = 5
    driveTime = 15
    
    def simpleGate(self):
        self.controller.setDepth(5)
        self.stateMachine.change_state("simpleGateWait")
    
    def simpleGateWait(self):
        if self.controller.isReady():
            self.stateMachine.change_state("driveThroughGate")
            self.startDriveTime = time.time()
    
    def driveThroughGate(self):
        self.controller.setSpeed(driveSpeed)
        currentTime = time.time()
        if (currentTime - self.startDriveTime) > driveTime:
            self.stateMachine.change_state("shutdown")
    #                                                             #
    ###############################################################
        
    ###############################################################        
    ##                        Test Code                          ##
    
    def testFunctionality(self):
        self.speed = 0
        self.direction = 1
        self.speedDone = 0
        self.counter1 = 0
        self.counter2 = 0
        
    def testSpeed(self):
        self.controller.setSpeed(self.speed)
        self.speed += self.direction*1
        if self.speed >= 5:
            self.direction = -1
        elif self.speed <= -5:
            self.speedDone = 1
            self.direction = 1
        if self.speed == 0 and self.speedDone == 1:
            self.stateMachine.change_state("testSpin")
    
    def spin360(self):
        self.controller.yawVehicle(360)
        self.change_state("spin90")
    
    def spin90(self):
        if self.counter1 == 0:
            self.controller.yawVehicle(90)
        elif self.counter1 == 1:
            self.controller.yawVehicle(-90)
        elif self.counter1 == 2:
            self.controller.yawVehicle(-90)
        elif self.counter1 == 3:
            self.controller.yawVehicle(90)
        self.counter1+=1
        if self.counter1 >= 4:
            self.stateMachine.change_state("testDepth")
            
    def testDepth(self):
        if self.counter2 == 0:
            self.controller.setDepth(-6)
        elif self.counter2 == 1:
            self.controller.setDepth(-3)
        elif self.counter2 == 2:
            self.controller.setDepth(-8)
        elif self.counter2 == 3:
            self.controller.setDepth(-1)
        self.counter1 += 1
        if self.counter1 >= 4:
            self.stateMachine.change_state("shutdown")
    
    #                                                              #
    ###############################################################
    

    ###############################################################        
    ##                        Gate Code                             ##
    def initToGate(self):
        self.vision.forward.gateDetectOn()
        self.controller.setDepth(10)
        self.controller.setSpeed(5)
        self.gateCount = 0
        self.gateX = 0
        self.gateY = 0
        self.gateNotSeen = 0
        self.stateMachine.change_state("toGate")
    
    def toGate(self):
        gateDetector = self.vision.forward.gateDetector
        
        if (gateDetector.found()):
            self.gateCount += 1
            self.gateX = gateDetector.getX()
            self.gateY = gateDetector.getY()
            self.stateMachine.change_state("headAtGate")
        else:
            if self.gateCount > 3:
                self.gateNotSeen += 1
                self.gateX = -1
                self.gateY = -1
        if self.gateNotSeen > 7:
            self.change_state("gateFound")
    
    def headAtGate(self):
        self.complexControl.navigateTowards(self.gateX,self.gateY)
    
    def gateFound(self):
        self.vision.forward.gateDetectOff()
        self.change_state("shutdown")    #currently lets just leave it at that, shall we?
        #self.change_state("initFirstRedLight")
    #                                                                #
    #################################################################
    
    ###############################################################        
    ##                        First Red Light                         ##

    def initFirstRedLight(self):
        self.lightx = -1.0
        self.lighty = -1.0
        self.scanDegrees = 5
        self.scanCount = 0
        self.seenLight = 0
        self.vision.forward.redLightDetectOn()
        self.vision.downward.orangeDetectOn()
        self.controller.setSpeed(2)
        self.controller.yawVehicle(-45)    #45 degrees to the right
        self.stateMachine.change_state("waitForReadyThenScan")
    
    def waitForReadyThenScan(self):
        if self.controller.isReady():
            self.stateMachine.change_state("firstLightScan")
            
    def firstLightScan(self):
        if self.controller.isReady():
            self.controller.yawVehicle(self.scanDegrees)
            self.scanCount += 1
            if self.scanCount == 18:
                self.scanDegrees = -1*scanDegrees
                self.scanCount = 0
        self.stateMachine.change_state("findRedLight")
        
    def findRedLight(self):
        lightDetector = self.vision.forward.RedLightDetector
        orangeDetector = self.vision.downward.OrangeDetector
        
        if lightDetector.found():
            self.seenLight += 1
            if self.seenLight > 10:
                self.seenLight = 10
            self.lightx = lightDetector.getX()
            self.lighty = lightDetector.getY()
            self.complexControl.navigateToward(self.lightx,self.lighty)
        elif orangeDetector.found():
            self.seenLight = self.seenLight - 1
            if self.seenLight < 5:
                self.stateMachine.change_state("foundFirstRedLight")
        else:
            self.stateMachine.change_state("firstLightScan")
    
    def foundFirstRedLight(self):
        self.vision.forward.redLightDetectOff()
        self.vision.downward.orangeDetectOff()
        self.stateMachine.change_state("initializePipeline")
        
    #                                                                #
    #################################################################
    
    ###############################################################        
    ##                        Pipeline Walk                         ##
    
    def initializePipeline(self):
        self.orangex = -1.0
        self.orangey = -1.0
        self.scanCount = 0
        self.scanDegrees = 5
        self.binCount = 0
        self.orangeCount = 0
        self.countDown = 15
        self.lostPipeCountDown = 40
        self.countOfTimesPipeLost = 0
    
        self.vision.downward.orangeDetectOn()
        self.vision.downward.binDetectOn()
        self.controller.setSpeed(2)
        self.controller.yawVehicle(-30)
        self.stateMachine.change_state("waitThenPipeScan")
    
    def waitThenPipeScan(self):
        if self.controller.isReady():
            self.stateMachine.change_state("pipeScan")

    def pipeScan(self):
        if self.controller.isReady():
            self.controller.yawVehicle(scanDegrees)
            self.scanCount = scanCount + 1
            if self.scanCount == 12:
                self.scanDegrees = -1 * scanDegrees
                self.scanCount = 0
        
        self.ai.stateMachine.change_state("findPipe")
        
    def findPipe(self):
        orangeDetector = self.vision.downward.OrangeDetector
        binDetector = self.vision.downward.BinDetector
        
        if binDetector.found():
            self.binCount += 1
            self.stateMachine.change_state("makeSureBinFound")
        elif orangeDetector.found():
            self.orangeCount += 1
            if self.orangeCount > 15:
                self.orangeCount = 15
            self.complexController.navigateAbove(orangeDetector.getX(),orangeDetector.getY(),orangeDetector.getAngle())
        elif self.orangeCount <= 5:
            self.stateMachine.change_state("pipeScan")
        else:
            self.countDown -= 1
            if self.countDown == 0:
                self.countDown = 15
                self.controller.setSpeed(-5)
                self.stateMachine.change_state("lostPipe")
    
    def lostPipe(self):
        orangeDetector = self.vision.downward.OrangeDetector
        self.lostPipeCountDown -= 1
        if orangeDetector.found() and (not orangeDetector.getAngle() == -10):
            self.controller.setSpeed(0)
            self.complexController.navigateAbove(orangeDetector.getX(),orangeDetector.getY(),orangeDetector.getAngle())
            self.stateMachine.change_state("findPipe")
        elif (self.lostPipeCountDown == 0):
            self.countOfTimesPipeLost += 1
            if (self.countOfTimesPipeLost == 5):    #Its like we're just running off the edge over and over, maybe we got turned around?
                self.countOfTimesPipeLost = 0
                self.controller.yawVehicle(180)
                self.stateMachine.change_state("initializePipeline")
            else:
                self.lostPipeCountDown = 40
                self.controller.setSpeed(2)
                self.ai.stateMachine.change_state("pipeScan")

    def makeSureBinFound(self):
        self.controller.setSpeed(0) 
        binDetector = self.vision.downward.BinDetector
        if binDetector.found():
            self.binCount+=1
        else:
            self.binCount-=1
            
        if (self.binCount<=0):
            self.binCount=0
            self.stateMachine.change_state("pipeFind")
        elif (self.binCount>=5):
            self.stateMachine.change_state("binFound")
        else:
            pass #remain in this state stopped and looking for a bin    

    def binFound(self):
        self.vision.downward.orangeDetectOff()
        #self.vision.downward.binDetectOff() #not turning it off because binDetection is next anyway
        self.stateMachine.change_state("binAimInitializer")

    #                                                                #
    #################################################################
    
    ###############################################################        
    ##                        Bin Aiming                              ##
    
    def binAimInitialize(self):
        self.countDownToLost = 20
        #We should only enter the bin states once we have already
        #found the bin (at least for bin1).  All this needs to do
        #is navigate to the bin.  If we lose it, well... unfortunate
        #but also too bad.  Best we can do is drop marker and move on
        self.controller.setSpeed(0)
        self.controller.setDepth(10)
        self.vision.downward.binDetectOn()
        self.stateMachine.change_state("aimForBin")
    
    def aimForBin(self):
        binDetector=self.vision.downward.BinDetector
        if (binDetector.found()):
            x = binDetector.getX()
            y = binDetector.getY()
            if (x>.48 and x<.52 and y>.48 and y<.52):
                self.vehicle.dropMarker()
                self.stateMachine.change_state("doneWithBin1")
            else:
                self.countDownToLost = 20
                self.complexControl.navigateAboveBin(x,y)
        else:
            self.countDownToLost -= 1
            if self.countDownToLost<=0:    #if we lose the bin for 10 tries frames, start heading upwards
                depth = self.controller.getDepth()
                if (depth > 6):
                    self.controller.setDepth(depth - .2)
                else:
                    #we're as high as I'm willing to risk, the bins just gone, lets drop marker and move on
                    self.controller.setDepth(10)
                    self.stateMachine.change_state("doneWithBin1")
                    
    def doneWithBin1(self):
        self.vision.downward.binDetectOff()
        self.stateMachine.change_state("after bin")
    
    #                                                                #
    #################################################################
    
    ###############################################################        
    ##                        Pipe with Light                         ##
    
    def intializePipeLight(self):
        self.orangex = -1.0
        self.orangey = -1.0
        self.scanCount = 0
        self.scanDegrees = 5
        self.binCount = 0
        self.orangeCount = 0
        self.countDown = 15
        self.lostPipeCountDown = 40
        self.countOfTimesPipeLost = 0
        self.lightFoundCount = 0
        self.lightLostCount = 0
        self.chaseCount = 0
        
        self.vision.downward.orangeDetectOn()
        self.vision.forward.redLightDetectOn()
        self.controller.setSpeed(4)
        self.controller.yawVehicle(-60)
        self.stateMachine.change_state("waitThenPipeLight")
        
    def waitThenPipeLight(self):
        if self.controller.isReady():
               self.stateMachine.change_state("scanPipeLight")
    
    def scanPipeLight(self):
        if self.controller.isReady():
            self.controller.yawVehicle(self.scanDegrees)
            self.scanCount += 1
            if self.scanCount == 24:
                self.scanDegrees = -1*scanDegrees
                self.scanCount = 0
        self.stateMachine.change_state("findPipeLight")

    def findPipeLight(self):
        orangeDetector = self.vision.downward.OrangeDetector
        redDetector = self.vision.forward.RedLightDetector

        if redDetector.found():
            self.lightFoundCount += 1
            self.complexControl.navigateTowards(redDetector.getX(),redDetector.getY())
            if (self.lightFoundCount == 5):
                self.lightFoundCount = 0
                self.stateMachine.change_state("chaseLight")
        elif (orangeDetector.found()):
            self.orangeCount += 1
            if (self.orangeCount > 15):
                self.orangeCount = 15
            self.complexControl.navigateAbove(orangeDetector.getX(),orangeDetector.getY(),orangeDetector.getAngle())
        elif (self.orangeCount <= 5):
            self.stateMachine.change_state("scanPipeLight")
        else:
            self.countDown -= 1
            if (self.countDown == 0):
                self.countDown = 15
                self.controller.setSpeed(-5)
                self.ai.stateMachine.change_state("lostPipeLight")

    def lostPipeLight(self):
        orangeDetector = self.vision.downward.OrangeDetector
        self.lostPipeCountDown -= 1
        if orangeDetector.found() and (not orangeDetector.getAngle() == -10):
            self.controller.setSpeed(0)
            self.complexControl.navigateAbove(orangeDetector.getX(),orangeDetector.getY(),orangeDetector.getAngle())
            self.stateMachine.change_state("findPipeLight")
        elif (self.lostPipeCountDown == 0):
            self.countOfTimesPipeLost += 1
            if (self.countOfTimesPipeLost == 5):    #Its like we're just running off the edge over and over, maybe we got turned around?
                self.countOfTimesPipeLost = 0
                self.controller.yawVehicle(180)
                self.stateMachine.change_state("initializePipeLight")    #flip around and start searching again.
            else:
                self.lostPipeCountDown = 40
                self.controller.setSpeed(2)
                self.stateMachine.change_state("scanPipeLight")

    def chaseLight(self):
        self.chaseCount += 1    #start slow, then speed up
        if (self.chaseCount > 80):
            self.chaseCount = 80 
        redDetector = self.vision.forward.RedLightDetector
        self.controller.setSpeed(chaseCount/10)        #smash hard.... but not TOO hard...
        if (redDetector.found()):
            self.lightLostCount=0
            self.complexControl.navigateTowards(redDetector.getX(),redDetector.getY())
        else:
            self.lightLostCount+=1
            if (lightLostCount==20):
                #Its gone, lets hope we hit it and find that bin
                self.controller.setSpeed(0)
                self.change_state("donePipeLight")

    def donePipeLight(self):
        self.vision.downward.orangeDetectOff()
        self.vision.forward.redLightDetectOff()
        self.stateMachine.change_state("after pipe light")

    #                                                                #
    #################################################################
    