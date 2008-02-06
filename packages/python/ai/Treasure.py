class GoTreasure():
    def __init__(self):
        self.vehicle=ModuleManager.get().get_module("Vehicle")
        self.controller=ModuleManager.get().get_module("Controller")
        self.vision=ModuleManager.get().get_module("Vision")
        self.ai=ModuleManager.get().get_module("AI")
        
        treasureStates={
                   "initialize":self.initialize,
                   "done":self.done,
                   "pollSonar":self.pollVision
                  }
        self.ai.stateMachine.set_states(treasureStates)
        self.ai.stateMachine.change_state("initialize")
        self.countDownToFailedSonar=0
        self.spiralSpeed=0
        self.dontSpiral=0
        self.neverGiveUp=30
        
    def initialize(self):
        self.controller.setSpeed(0)
        self.controller.setDepth(10)
        self.ai.stateMachine.change_state("pollVision")
                
    def pollSonar(self):
        if (not self.controller.isReady()):
            return
        
        sonar=self.vehicle.Sonar
        if (sonar.found()):
            angle=sonar.getYaw()
            self.controller.yawVehicle(angle)#One big turn to start us off as soon as sonar finds pinger
            self.ai.stateMachine.change_state("waitThenCharge")
        else:
            self.countDownToFailedSonar+=1
            if (self.countDownTofailedSonar==100):
                self.ai.stateMachine.change_state("treasureSpiral")
                self.countDownToFailedSonar=0

    def treasureSpiral(self):
        self.vision.forward.RedDetectorOn()#Use the red detector as a treasure sensor, hopefully we can pick it up
        redDetector=self.vision.forward.RedDetector
        sonar=self.vehicle.Sonar
        self.controller.setDepth(10)
        if (sonar.found()):
            self.spiralSpeed=0
            self.ai.stateMachine.change_state("pollSonar")
        elif (redDetector.found()):
            self.spiralSpeed=0
            self.dontSpiral=5
            self.ai.navigateToward(redDetector.getX(),redDetector.getY())
        else:
            self.dontSpiral-=1
            if (self.dontSpiral<=0):
                self.spiralSpeed+=1
                self.controller.setSpeed(self.spiralSpeed/25)
                self.controller.yawVehicle(4)
                
    def waitThenCharge(self):
        if (not self.controller.isReady()):
            return
        sonar=self.vehicle.Sonar
        if (sonar.found()):
            magicDistance=sonar.getDistance()
            self.controller.setSpeed(magicDistance/2)
            angle=sonar.getYaw() 
            #Positive and Negative 180 checkers, to see if we passed it
            if (angle<=200 and angle>=160):
                self.controller.setSpeed(-2)
            elif (angle>=-200 and angle<=-160):
                self.controller.setSpeed(-2)
            #Other angles
            elif (angle>=10 or angle<=-10):
                #correct for these
                self.controller.setSpeed(0)
                self.controller.yawVehicle(angle)

        else:
            #Sonar lost!
            #Not a whole ton we can do about it.
            self.neverGiveUp-=1
            if (self.neverGiveUp==0):#alright alright, give up
                self.ai.stateMachine.change_state("surface")
        if (magicDistance<=2):
            self.ai.stateMachine.change_state("surface")
        
    def surface(self):
        sonar=self.vehicle.Sonar
        if (sonar.found()):
            magicDistance=sonar.getDistance()
            if (magicDistance<=2):
                self.controller.setDepth(self.controller.getDepth()-.2)
                if (self.controller.getDepth()<=2):
                    self.controller.setDepth(0)
                    self.ai.returnControl();
                    self.vision.forward.redLightDetectOff()
            if (magicDistance>5):#aww crud, try again
                self.ai.stateMachine.change_state("pollSonar")
                
    
    def done(self):
        self.vision.downward.binDetectOff()
        self.ai.returnControl()