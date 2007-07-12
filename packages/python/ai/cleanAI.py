import ai.state_machine as StateMachine
import ai.Movement as Movement

import ai.AIModel as AIModel

class AI(Module):
    def __init__(self, config):
	
       	self.startState = "toGate"

 		self.aiStates = {
                     	"toGate":Start.initialize
                     	"toRedLight1":self.toRedLight1,
                     	"toPipeline":self.toPipeline,
                     	"toBin1":self.toBin1,
                     	"toPipelineAndLight":self.toPipelineAndLight,
                     	"toBin2":self.toBin2,
                     	"toTreasure":self.toTreasure,
                     	"missionComplete":self.missionComplete
                     	}

        self.stateMachine = StateMachine()
		self.stateMachine.state = self.startState
		self.model = AIModel.model()
		self.vehicle = model.vehicle
		self.vision = model.vision
		self.controller = self.model.controllerler
		self.complexControl = Movement.control()
		
	
        Module.__init__(self,config)
 	
	###############################################################		
	##						Gate Code							 ##
	def initToGate(self):
		self.vision.forward.gateDetectOn()
		self.controller.setDepth(10)
		self.controller.setSpeed(5)
		self.stateMachine.change_state("toGate")
		self.gateCount = 0
		self.gateX = 0
		self.gateY = 0
		self.gateNotSeen = 0
	
	def toGate(self):
		gateDetector = self.vision.forward.gateDetector
		
		if (gateDetector.found()):
			self.gateCount += 1
			self.gateX = gateDetector.getX()
			self.gateY = gateDetector.getY()
			self.stateMachine.change_state("headAtGate")
		else:
			if self.gateCount > 3
				self.gateNotSeen += 1
				self.gateX = -1
				self.gateY = -1
		if self.gateNotSeen > 7:
			self.change_state("gateFound")
	
	def headAtGate(self):
		self.complexControl.navigateTowards(self.gateX,self.gateY)
	
	def gateFound(self):
		self.vision.forward.gateDetectOff()
		self.change_state("initFirstRedLight")
	#																#
	#################################################################
	
	###############################################################		
	##						First Red Light						 ##

	def initFirstRedLight(self):
		self.lightx=-1.0
        self.lighty=-1.0
		self.scanDegrees=5
        self.scanCount=0
        self.seenLight=0
		self.vision.forward.redLightDetectOn()
        self.vision.downward.orangeDetectOn()
        self.controller.setSpeed(2)
        self.controller.yawVehicle(-45)	#45 degrees to the right
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
		
	#																#
	#################################################################
	
	###############################################################		
	##						Pipeline Walk					     ##
	
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
            if (self.countOfTimesPipeLost == 5):	#Its like we're just running off the edge over and over, maybe we got turned around?
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
        self.stateMachine.change_state("afterBinFound")

	#																#
	#################################################################
	
	
	
	
	
	
	def pointTowardsBin(self,x,y):
		max_search_speed = 5
		center_resolution = 0.1
		
		if x < 0 or y < 0 or x > 1 or y > 1:
			print "Get Above Bin called with arguments that are out of range, this is an error"
			return
		
		offset_x = x - 0.5	#if offset_x is positive location is left of center target
		offset_y = y - 0.5	#if offset_y is positive location is below center target
		
		angle = math.atan(offset_y/offset_x)
		
		if self.controller.isReady():
			pass
			#do something	
				
		
    
    
