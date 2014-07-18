import ram.ai.state as state
import ram.motion as motion
import ext.core as core
import ext.vision as vision
import ext.math as math
from ram.motion.basic import Frame
from ext.control import yawVehicleHelper


DONE = core.declareEventType('DONE')
YAWED = core.declareEventType('YAWED')

#parameter lookup function
#you is self, name is parameter to look up, default is default output
def getConfig(you,name,default):
    return you.ai.data['config'].get(name,default)

#dive to a specified depth
def dive(you, depth, rate):
            # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = you.stateEstimator.getEstimatedDepth(),
            finalValue = depth,
            initialRate = you.stateEstimator.getEstimatedDepthRate(),
            avgRate = rate)
        # Dive
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)
        you.motionManager.setMotion(diveMotion)

#hold the current position in xy
def freeze(you):
    you.motionManager._controller.moveVel(0,0,0)
    traj = motion.trajectories.Vector2CubicTrajectory(math.Vector2.ZERO,math.Vector2.ZERO)
    dive(you, you.stateEstimator.getEstimatedDepth(), 0.15)
    currentOrientation = you.stateEstimator.getEstimatedOrientation()
    yawTrajectory = motion.trajectories.StepTrajectory(
        initialValue = currentOrientation,
        finalValue = currentOrientation,
        initialRate = you.stateEstimator.getEstimatedAngularRate(),
        finalRate = math.Vector3.ZERO)
    yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
    you.motionManager.setMotion(yawMotion)
    mot = motion.basic.Translate(traj,Frame.LOCAL)
    you.motionManager.setMotion(mot)

#this class contains routines to handle motions
#thus allowing for easy manuevers(sp?)
#it sends a done event when it finishes
#this publishes a done event when finished
class MotionState(state.State):
    #dive to a specified depth
    def dive(self, depth, rate):
        # Compute trajectories
        diveTrajectory = motion.trajectories.ScalarCubicTrajectory(
            initialValue = self.stateEstimator.getEstimatedDepth(),
            finalValue = depth,
            initialRate = self.stateEstimator.getEstimatedDepthRate(),
            avgRate = rate)
        # Dive
        diveMotion = motion.basic.ChangeDepth(
            trajectory = diveTrajectory)
        self.motionManager.setMotion(diveMotion)
        self._mYaw = False
        
#translate locally x,y at rate rate
    def translate(self,x,y,rate):
        translateTrajectory = motion.trajectories.Vector2CubicTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(y, x),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            avgRate = rate)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)
        self.motionManager.setMotion(translateMotion)
        self._mYaw = False
            
#rotate to global orientation of deg degrees, ending after time t has passed
    def yawGlobal(self,deg,t):
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = math.Quaternion(math.Degree(deg), 
                                         math.Vector3.UNIT_Z),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        self.motionManager.setMotion(yawMotion)
        self.timer = self.timerManager.newTimer(YAWED, t)
        self.timer.start()
        self._mYaw = True
            
#rotate to local orientation of deg degrees, ending after time t has passed
    def yaw(self,deg,t):
        currentOrientation = self.stateEstimator.getEstimatedOrientation()
        yawTrajectory = motion.trajectories.StepTrajectory(
            initialValue = currentOrientation,
            finalValue = yawVehicleHelper(currentOrientation, 
                                          deg),
            initialRate = self.stateEstimator.getEstimatedAngularRate(),
            finalRate = math.Vector3.ZERO)
        yawMotion = motion.basic.ChangeOrientation(yawTrajectory)
        self.motionManager.setMotion(yawMotion)
        self.timer = self.timerManager.newTimer(YAWED, t)
        self.timer.start()
        self._mYaw = True
        
    def FINISHED(self,event):
            if(self._mYaw == False):
                self.publish(DONE,core.Event())
    def YAWED(self,event):
            self.publish(DONE,core.Event())
            
#end motionState

#specialized version of state for doing intitializations
#and other things that are just busywork
#this sends a DONE event when finished
#code for this class should be put in run, NOT enter
#you must still define transitions for this state
class FiniteState(state.State):
    #DONE = core.declareEventType('DONE')
    def run(self):
        pass

    def enter(self):
        self.run()
        self.publish(DONE,core.Event())


class TranslateTester(MotionState):

    @staticmethod
    def transistions():
        return { motion.basic.MotionManager.FINISHED : TranslateTester,
                 DONE : state.State }

    @staticmethod
    def getattr():
        return { 'rate' : 0.1, 'distance' : 1 }

    def enter(self):
        self.translate(self._distance, self._distance, self._rate)

    def exit(self):
        freeze(self)
        self.motionManager.stopCurrentMotion()

class DiveTester(MotionState):

    @staticmethod
    def transistions():
        return { motion.basic.MotionManager.FINISHED : DiveTester,
                 DONE : state.State }

    @staticmethod
    def getattr():
        return { 'rate' : 0.1, 'depth' : 1 }

    def enter(self):
        self.dive(self._depth, self._rate)

    def exit(self):
        freeze(self)
        self.motionManager.stopCurrentMotion()

class LocalYaw(MotionState):

    @staticmethod
    def transistions():
        return { motion.basic.MotionManager.FINISHED : LocalYaw,
                 YAWED : LocalYaw,
                 DONE : state.State }

    @staticmethod
    def getattr():
        return { 'time' : 15, 'degree' : 90 }

    def enter(self):
        self.yaw( self._degree, self._time )

    def exit(self):
        freeze(self)
        self.motionManager.stopCurrentMotion()


class GlobalYaw(MotionState):
    
    @staticmethod
    def transistions():
        return { motion.basic.MotionManager.FINISHED : TranslateTester,
                 YAWED : GlobalYaw,
                 DONE : state.State }

    @staticmethod
    def getattr():
        return { 'time' : 15, 'degree' : 90 }

    def enter(self):
        self.yawGlobal( self._degree, self._time )

    def exit(self):
        freeze(self)
        self.motionManager.stopCurrentMotion()
# #to use, call run from the function for the event you want to use passing in the
# #to configure if you want to use you must define the following
# #distance, will affect how fast you go in general
# #vfunc affects how quickly you approach
# #dispfunc affects how long your corrections take
# #correctD is the displacement you use to correct displacements by
# #distance is the forwards displacement for each iteration
# #you must be pointed at the object you wish to reach to use this state
# class Approach(state.State):

#     #for scaling the rate at which the object is approached
#     #be sure to include a hard minimum that is not zero
#     #the function effectively outputs a time, but the time is
#     #going to get cutoff
#     #takes event in as input
#     def vfunc(self,event):
#         return 0
#     #based off the x-y error this function should output a desired correction
#     #see above comments for vfunc
#     #note that these are actual displacements, the robot will complete the entire
#     #displacement
#     #take event in as input
#     def dispfunc(self,event):
#         return 0
#     #this decides when to stop, you must overload this
#     def decide(self,event):
#         return True

#     def directionfunc(self,event):
#         if(event.x<self._xmin):
#             return -1#go the other way
#         else:
#             if(event.x>self._xmax):
#                 return 1#go the other way
#             else:
#             #inside the bounds, don't move
#                 return 0

#     def run(self,event):
#         if(self.decide(event) == True):
#             #set self to not move and then finish
#             freeze(self)
#             self.publish(DONE,core.Event())
#         else:
#             self.move(event)

#     def move(self,event):
#         #begin main trajectory
#         t1 = self.vfunc(event)
#         t2 = self.dispfunc(event)
#         translateTrajectory = motion.trajectories.Vector2CubicDecoupledTrajectory(
#             initialValue = math.Vector2.ZERO,
#             finalValue = math.Vector2(self._distance, self._correctD*self.directionfunc(event)),#, self._distance),
#             initialRate = self.stateEstimator.getEstimatedVelocity(),
#             time1 = t1, time2 = t2)
#         translateMotion = motion.basic.Translate(
#             trajectory = translateTrajectory,
#             frame = Frame.LOCAL)
#         self.motionManager.setMotion(translateMotion)


#     def exit(self):
#         self.motionManager.stopCurrentMotion()
#         pass







