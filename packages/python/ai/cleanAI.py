from module import Module, IModule
from core import implements
from ai.state_machine import state_machine as StateMachine
import ai.Movement as Movement
import time as clock

import ai.AIModel as AIModel

class AI(Module):
    implements(IModule)
    
    def __init__(self, veh, config):
        self.startState = config["start_state"]    #state that the vehicle enters after waiting to start
        
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
                    "zigZagSearchTime":self.zigZagSearchTime,
                    "reset":self.reset,
		    "countDown":self.countDown,
		    "hang":self.hang,
                    "confirmReset":self.confirmReset,
                    "diveAndGo":self.diveAndGo,
		    "waitForDive":self.waitForDive,
		    "driveStraight":self.driveStraight,
		    "prepareScout":self.prepareScout,
		    "pointIn":self.pointIn,
		    "spiralTillDeath":self.spiralTillDeath,
		    "driveToMiddle":self.driveToMiddle,
		    "driveAndSpiral":self.driveAndSpiral,
		    "waitThroughGate":self.waitThroughGate,
		    "headToMidle":self.headToMiddle,
		    "waitToMiddle":self.waitToMiddle,
		    "spiralPattern":self.spiralPattern
                    }
	
        self.stateMachine = StateMachine()
        
        self.stateMachine.state = "waitForStart"
        self.stateMachine.set_states(self.aiStates)
        self.model = AIModel.model()
        self.vehicle = self.model.vehicle
        #self.vision = self.model.vision
        self.controller = self.model.controller
        self.complexControl = Movement.control()
	
	self.vehicle.printLine(0," ")
	self.vehicle.printLine(1," ")
	self.ignoreReset = True
	        
        Module.__init__(self,config)
        
    def update(self,timeStep):
        self.timeStep = timeStep
	if self.vehicle.startStatus() == 0 or self.ignoreReset == True:
            self.stateMachine.operate()
        else:
            self.stateMachine.change_state("reset")
	    
    ###############################################################        
    ##                        Simple Gate                        ##
    
    operateTime = 5 * 60
    
    driveSpeed = 5
    driveTime = 15
    searchForTime = 50
    zagTime = 10
    zagAngle = 45
    searchSpeed = 3
    
    pointAngle = 45
    
    def simpleGate(self):
        self.vehicle.startDriveTime = clock.time()
        self.controller.setDepth(5)
        self.stateMachine.change_state("simpleGateWait")
    
    def simpleGateWait(self):
        if self.controller.isReady():
            self.stateMachine.change_state("driveThroughGate")
            self.startDriveTime = clock.time()
    
    def driveThroughGate(self):
        self.controller.setSpeed(driveSpeed)
        currentTime = clock.time()
        if (currentTime - self.startDriveTime) > driveTime:
            self.stateMachine.change_state("scout")
            
    def prepareScout(self):
        self.controller.yawVehicle(zagAngle/2)
        self.controller.setDepth(10)
        if self.controller.isReady():
            self.controller.setSpeed(searchSpeed)
            self.stateMachine.change_state("zigZagSearchTimeInit")
            
    def zigZagSearchTimeInit(self):
        self.lastZag = time.time()
        self.startZagTime = self.lastZag
        self.turn = 1
        
        self.controller.setSpeed(searchSpeed)
        self.stateMachine.change_state("zigZagSearchTime")
        
    def zigZagSearchTime(self):
        if (time.time() - self.startZagTime) >= searchForTime:
            self.stateMachine.change_state("pointIn")
        else:
            results = self.complexControl.zigZag(self.lastZag,zagTime,zagAngle,self.turn)
            self.lastZag = results[0]
            self.turn = results[1]
            
    def pointIn(self):
        self.controller.yawVehicle(45)
        if self.controller.isReady():
            self.controller.setSpeed(driveSpeed)
            self.startDriveTime = clock.time()
            self.vehicle.change_state(driveToMiddle)
            
    def driveToMiddle(self):
        currentTime = clock.time()
        elapsed = currentTime - self.startDriveTime
        if elapsed > 15:
            self.stateMachine.change_state("spiralTillDeath")
            self.startDriveTime = clock.time()
            
    def spiralTillDeath(self):
        spiralTime = clock.time() - self.startDriveTime
        self.controller.yawVehicle(5)
        if spiralTime < 10:
            self.controller.setSpeed(3)
        elif spiralTime < 20:
            self.controller.setSpeed(4)
        elif spiralTime < 30:
            self.controller.setSpeed(5)
        elif spiralTime < 40:
            self.controller.setSpeed(6)
        elif spiralTime < 50:
            self.controller.setSpeed(7)
        elif spiralTime < 60:
            self.controller.setSpeed(8)
        else:
            self.controller.setSpeed(8)
            
        if clock.time() - self.vehicleStartTime > operateTime:
            self.stateMachine.change_state("shutdown")
    #                                                             #
    ###############################################################
	
    ###############################################################        
    ##                        Drive then Spiral                  ##
	    
    operateTime = 5 * 60    #operate for 5 minutes    
    driveSpeed = 7
    throughGateTime = 30
    toCenterAngle = 45
    toMiddleTime = 30
    spiralAngle = 3
    gateDepth = 4
    spiralDepth = 10
    lookDepth = 14
        
    def driveAndSpiral(self):
        self.controller.setDepth(gateDepth)
        if self.controller.isReady():
	    self.controller.setSpeed(driveSpeed)
	    self.changeTime = clock.time()
	    self.stateMachine.change_state("waitThroughGate")
	
    def waitThroughGate(self):
	elapsed = clock.time() - self.changeTime
	if elapsed > throughGateTime:
	    self.stateMachine.change_state("headToMiddle")
	    self.controller.setSpeed(0)
	
    def headToMiddle(self):
	self.controller.yawVehicle(toCenterAngle)
	self.controller.setDepth(spiralDepth)
	if self.controller.isReady():
	    self.controller.setSpeed(driveSpeed)
	    self.changeTime = clock.time()
	    self.stateMachine.change_state("waitToMiddle")
	        
    def waitToMiddle(self):
	elapsed = clock.time() - self.changeTime
	if elapsed > toMiddleTime:
	    self.controller.setSpeed(0)
	    self.changeTime = clock.time()
	    self.stateMachine.change_state("spiralPattern")
	        
    def spiralPattern(self):
	spiralTime = clock.time() - self.changeTime
        self.controller.yawVehicle(spiralAngle)
        if spiralTime < 10:
            self.controller.setSpeed(3)
        elif spiralTime < 20:
            self.controller.setSpeed(4)
        elif spiralTime < 30:
            self.controller.setSpeed(5)
        elif spiralTime < 40:
            self.controller.setSpeed(6)
        elif spiralTime < 50:
            self.controller.setSpeed(7)
        elif spiralTime < 60:
            self.controller.setSpeed(8)
        else:
            self.controller.setSpeed(8)
            
        if clock.time() - self.vehicleStartTime > operateTime:
            self.controller.setSpeed(0)
            self.iter = 0
            self.depth = lookDepth
	    self.controller.setDepth(lookDepth)
            self.stateMachine.change_state("lookAround")

    def lookAround(self):
        if self.controller.isReady():
            if self.depth == 0:
                self.stateMachine.change_state("shutdown")
	    else:
                self.controller.yawVehicle(lookAngle)
                self.iter += 1
            if self.iter >= 360/lookAngle:
	        self.stateMachine.change_state("rise")

    def rise(self):
        if self.depth == 0:
            self.stateMachine.change_state("shutdown")
        elif self.controller.isReady():
            self.controller.setDepth(self.depth - 2)
            self.depth = self.depth - 2
            self.iter = 0
	    self.stateMachine.changeState("lookAround")
	    
    ###############################################################        
    ##                        General States                         ##
    
    def waitForStart(self):
	self.ignoreReset = True
        start = self.vehicle.startStatus()
        self.vehicle.printLine(0,"Ready to operate...")
        self.vehicle.printLine(1,"Tap start wand!")
        if start == 1:
            self.stateMachine.change_state("countDown")
            self.pushTime = clock.time()
            self.lastMessage = ""

    def countDown(self):
        tillTime = int(clock.time() - self.pushTime)
        message = "Starting in " + str(5 - tillTime)
	if message != self.lastMessage:
            self.vehicle.printLine(0,message)
	    self.vehicle.printLine(1,"please wait...")
	self.lastMessage = message
        if clock.time() - self.pushTime >= 5:
            self.vehicle.printLine(0,"Vehicle Operating!")
            self.vehicle.unsafeThrusters()
            self.stateMachine.change_state(self.startState)
            self.ignoreReset = False

    def shutdown(self):
        self.vehicle.printLine(0,"halting vehicle")
        self.controller.setDepth(1)
        self.controller.setSpeed(0)
        self.vehicle.safeThrusters()
        
    def reset(self):
        self.controller.setSpeed(0)
	self.vehicle.printLine(0,"Reset engaged")
        self.stateMachine.change_state("confirmReset")

    def confirmReset(self):
	self.ignoreReset = True
    	self.vehicle.printLine(0,"Tap wand to")
	self.vehicle.printLine(1,"confirm reset")
	if self.vehicle.startStatus() == 1:
	    self.stateMachine.change_state("waitForStart")

    def hang(self):
	self.vehicle.printLine(0,"hanging...")
	self.vehicle.printLine(1,"")
	
    #                                                              #
    ############################################################### 
    
    ###############################################################        
    ##                        Search Patterns                    ##
    
    def diveAndGo(self):
	self.wantedDepth = 2
	self.controller.setDepth(self.wantedDepth)
	self.stateMachine.change_state("waitForDive")

    def waitForDive(self):
	depth = self.vehicle.getDepth()
	if depth > self.wantedDepth - 0.2 and depth < self.wantedDepth + 0.2:
	    self.controller.setSpeed(7)
	    self.startDriveTime = clock.time()
	    self.stateMachine.change_state("driveStraight")

    def driveStraight(self):
	
	driveForTime = 10 

        elapsed = clock.time() - self.startDriveTime
	if elapsed > driveForTime:
	    self.stateMachine.change_state("hang") 
	    
	
      
    #                                                              #
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
            self.controller.setDepth(6)
        elif self.counter2 == 1:
            self.controller.setDepth(3)
        elif self.counter2 == 2:
            self.controller.setDepth(8)
        elif self.counter2 == 3:
            self.controller.setDepth(1)
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
    