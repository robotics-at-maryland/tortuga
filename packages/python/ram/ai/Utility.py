import ram.ai.state as state
import ram.motion as motion
import ext.core as core
import ext.vision as vision
import ext.math as math
from ram.motion.basic import Frame


DONE = core.declareEventType('DONE')


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
            avgRate = diveRate)
        # Dive
        diveMotion = motion.basic.ChangeDepth(trajectory = diveTrajectory)
        you.motionManager.setMotion(diveMotion)

#hold the current position in xy
def freeze(you):
    traj = motion.trajectories.Vector2CubicTrajectory(math.Vector2.ZERO,math.Vector2.ZERO)
    mot = motion.basic.Translate(traj,Frame.LOCAL)
    you.motionManager.setMotion(mot)

#specialized version of state for doing intitializations
#and other things that are just busywork
#this sends a DONE event when finished
#code for this class should be put in run, NOT enter
#you must still define transitions for this state
class FiniteState(state.State):
    #DONE = core.declareEventType('DONE')

    def enter(self):
        self.run()
        self.publish(DONE,core.Event())
#to use, call run from the function for the event you want to use passing in the
#to configure if you want to use you must define the following
#distance, will affect how fast you go in general
#vfunc affects how quickly you approach
#dispfunc affects how long your corrections take
#correctD is the displacement you use to correct displacements by
#distance is the forwards displacement for each iteration
#you must be pointed at the object you wish to reach to use this state
class Approach(state.State):

    #for scaling the rate at which the object is approached
    #be sure to include a hard minimum that is not zero
    #the function effectively outputs a time, but the time is
    #going to get cutoff
    #takes event in as input
    def vfunc(self,event):
        return 0
    #based off the x-y error this function should output a desired correction
    #see above comments for vfunc
    #note that these are actual displacements, the robot will complete the entire
    #displacement
    #take event in as input
    def dispfunc(self,event):
        return 0
    #this decides when to stop, you must overload this
    def decide(self,event):
        return True

    def directionfunc(self,event):
        if(event.x<self._xmin):
            return -1#go the other way
        else:
            if(event.x>self._xmax):
                return 1#go the other way
            else:
            #inside the bounds, don't move
                return 0

    def run(self,event):
        if(self.decide(event) == True):
            #set self to not move and then finish
            freeze(self)
            self.publish(DONE,core.Event())
        else:
            self.move(event)

    def move(self,event):
        #begin main trajectory
        t1 = self.vfunc(event)
        t2 = self.dispfunc(event)
        translateTrajectory = motion.trajectories.Vector2CubicDecoupledTrajectory(
            initialValue = math.Vector2.ZERO,
            finalValue = math.Vector2(self._distance, self._correctD*self.directionfunc(event)),#, self._distance),
            initialRate = self.stateEstimator.getEstimatedVelocity(),
            time1 = t1, time2 = t2)
        translateMotion = motion.basic.Translate(
            trajectory = translateTrajectory,
            frame = Frame.LOCAL)
        self.motionManager.setMotion(translateMotion)


    def exit(self):
        self.motionManager.stopCurrentMotion()
        pass




#variant on approach that just moves at differings SPEEDS
#and makes stop decision based off critical range
#a variant using size should be done at some point(needs to be added to vision system)
#you must define distance, correctD, speed and critRange
#really, I need to make a Vector2Step trajectory
class ConstApproach(Approach):


    def vfunc(self,event):
        #return self._distance/self._speed
        return .1

    def dispfunc(self,event):
        #return self._correctD/self._speed
        return .1

    def decide(self,event):
        if(event.range < self._critRange):
            return True
        else:
            return False

#gen approach
#this variant of approach designed to target a buoy
#also designed to be a "test" version of approach
class genApproach(ConstApproach):
    @staticmethod
    def getattr():
        return { 'speed' : .15 ,  'distance' : .2 ,
                 'xmin' : -0.05 , 'xmax' : 0.05, 'correctD' : .4, 'critRange': 4 }

    def enter(self):
        pass
    def BUOY_FOUND(self,event):
        self.run(event)
    @staticmethod
    def transitions():
        return {DONE : state.State, vision.EventType.BUOY_FOUND : genApproach}#, motion.basic.MotionManager.FINISHED : genApproach}
        
        

        
