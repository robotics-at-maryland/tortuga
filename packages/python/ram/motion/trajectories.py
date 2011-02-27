# Copyright (C) 2010 Robotics @ Maryland
# Copyright (C) 2010 Jonathan Wonders <jwonders@umd.edu>
# All rights reserved.
#
# Author: Jonathan Wonders <jwonders@umd.edu>
# File:  packages/python/ram/motion.py

# Python Imports
import math as pmath
import numpy

# Project Imports
import ext.math as math
import ram.timer as timer

class Trajectory:
    """
    An abstract class for motion trajectories
    """

    def __init__(self):
        """
        Nothing to do here since this is abstract
        """
        
    def computeValue(self, time):
        """
        Returns the value of the trajectory at given time
        """
        raise Exception("You must implement this method")

    def computeDerivative(self, time, order):
        """
        Returns the value of the derivative of given order at given time
        """
        raise Exception("You must implement this method")

    def getInitialTime(self):
        """
        Returns the time at which the trajectory starts to deviate
        from the initial state.
        """
        raise Exception("You must implement this method")

    def getFinalTime(self):
        """
        Returns the time at which the trajectory will evaluate
        to its final state
        """
        raise Exception("You must implement this method")

    def getMaxOfDerivative(self, order):
        """
        Returns the maximum of the derivative of given order
        If order is negative or the max is unknown, returns None
        """
        raise Exception("You must implement this method")

    def isRelative(self):
        """
        Returns True if the trajectory evaluates given a relative time
        or False if the trajectory evaluates given an absolute time
        """
        raise Exception("You must implement this method")


class ScalarCubicTrajectory(Trajectory):
    """
    Defines the creation and evaluation of a trajectory based
    on a cubic polynomial.  The trajectory is a scalar valued
    function and therefore the values returned will be scalars.
    This trajectory will be useful for changing depths.
    """

    # this is a parameter that relates the max rate to the time
    # interval in which the trajectory will be defined
    MAGIC_RATE_SLOPE = 10.0


    def __init__(self, initialValue, finalValue, initialTime = 0,
                 initialRate = 0, finalRate = 0, maxRate = 3):

        # keep track of the arguments
        self._initialValue = initialValue
        self._finalValue = finalValue
        self._initialRate = initialRate
        self._finalRate = finalRate
        self._initialTime = initialTime

        if initialTime == 0:
            self._relative = True

        # compute the final time
        self._timePeriod = self.approximateTimePeriod(
            pmath.fabs(finalValue - initialValue), maxRate)

        # the matrix will be singular if initial and final values are equal
        # this shouldn't happen but 
        if initialValue == finalValue:
            tf = 0
            self._coefficients = None
            self._maxRate = 0

        else:
            # we solve for the polynomial starting at time 0 to avoid
            # numerical problems that could occur if we use the given
            # initial time.
            ti = 0
            tf = self._timePeriod

            # compute matrix A and vector b for equation Ax = b
            ti_p2 = ti * ti
            ti_p3 = ti_p2 * ti

            tf_p2 = tf * tf
            tf_p3 = tf_p2 * tf

            A = numpy.array([[1, ti, ti_p2, ti_p3],
                             [1, tf, tf_p2, tf_p3],
                             [0, 1, 2 * ti, 3 * ti_p2],
                             [0, 1, 2 * tf, 3 * tf_p2]])
        
            b = numpy.array([initialValue, finalValue, initialRate, finalRate])

            # solve for coefficient vector x
            x = numpy.linalg.solve(A,b)

            pow = pmath.pow
            self._coefficients = x
            self._maxRate = x[1] - pow(x[2],2) / (3 * x[3])


    def computeValue(self, time):
        # compute the time from the beginning of the trajectory
        # because we have constructed the trajectory starting at time 0
        time = time - self._initialTime

        if time < 0:
            return self._initialValue
        elif time >= self._timePeriod:
            return self._finalValue
        else:
            c = self._coefficients
            return c[0] + time * (c[1] + time * (c[2]  + time * c[3]))

    def computeDerivative(self, time, order):
        # handle t < ti
        if time < 0:
            if order == 1:
                return self._initialRate
            elif order > 1:
                return 0
            else:
                return None

        # handle t > tf
        if time >= self._timePeriod:
            if order == 1:
                return self._finalRate
            elif order > 1:
                return 0
            else:
                return None

        # handle times during the computed trajectory
        c = self._coefficients
        if order < 1 :
            return None
        elif order == 1:
            return c[1] + time * (2 * c[2]  + time * 3 * c[3])
        elif order == 2:
            return 2 * c[2] + 6 * c[3] * time
        elif order == 3:
            return 6 * c[3]
        else:
            return 0

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._initialTime + self._timePeriod

    def getMaxOfDerivative(self, order):
        if order == 1:
            return self._maxRate
        elif order == 2:
            return 2 * coefficients[2]
        elif order > 2:
            return 0
        else:
            return None

    def isRelative(self):
        return self._relative

    def approximateTimePeriod(self, changeInValue, maxRate):
        slope = self.MAGIC_RATE_SLOPE / maxRate
        return slope * changeInValue

class Vector2CubicTrajectory(Trajectory):
    """
    This trajectory goes in a straight line between 2D points.
    It has two continous derivatives. This is very similar to
    the scalar cubic trajectory.
    """
    def __init__(self, initialValue, finalValue, initialTime = 0,
                 initialRate = math.Vector2.ZERO,
                 finalRate = math.Vector2.ZERO, avgRate = 0.5):

        # keep track of the arguments V - vector quantity, S - scalar quantity
        self._initialValue = initialValue
        self._finalValue = finalValue
        self._initialRateV = initialRate
        self._finalRateV = finalRate
        self._initialTime = initialTime
        self._changeInValueV = finalValue - initialValue
        self._changeInRateV = finalRate - initialRate

        if initialTime == 0:
            self._relative = True

        # compute the final time
        self._timePeriod = float(self._changeInValueV.length() + \
                                     1 * self._changeInRateV.length()) \
                                     / float(avgRate)

        self._ignore0 = False
        self._ignore1 = False

        # the matrix will be singular if initial and final values are equal
        # this shouldn't happen but 
        if initialValue[0] == finalValue[0]:
            tf = 0
            self._ignore0 = True
            self._coefficients0 = numpy.array([0, 0, 0, 0, 0])

        if initialValue[1] == finalValue[1]:
            tf = 0
            self._ignore1 = True
            self._coefficients1 = numpy.array([0, 0, 0, 0, 0])


        # we solve for the polynomial starting at time 0 to avoid
        # numerical problems that could occur if we use the given
        # initial time.
        ti = 0
        tf = self._timePeriod
        
        # compute matrix A and vector b for equation Ax = b
        ti_p2 = ti * ti
        ti_p3 = ti_p2 * ti

        tf_p2 = tf * tf
        tf_p3 = tf_p2 * tf

        A = numpy.array([[1, ti, ti_p2, ti_p3],
                         [1, tf, tf_p2, tf_p3],
                         [0, 1, 2 * ti, 3 * ti_p2],
                         [0, 1, 2 * tf, 3 * tf_p2]])
            
        b0 = numpy.array([self._initialValue[0], self._finalValue[0],
                          self._initialRateV[0], self._finalRateV[0]])
        
        b1 = numpy.array([self._initialValue[1], self._finalValue[1],
                          self._initialRateV[0], self._finalRateV[1]])
        
        # solve for coefficient vector x
        x0 = numpy.linalg.solve(A,b0)
        x1 = numpy.linalg.solve(A,b1)
        
        pow = pmath.pow
        if not self._ignore0:
            self._coefficients0 = x0
        if not self._ignore1:
            self._coefficients1 = x1

        self._maxRate = 0 # TODO


    def computeValue(self, time):
        # compute the time from the beginning of the trajectory
        # because we have constructed the trajectory starting at time 0
        ts = time - self._initialTime

        if ts < 0:
            return self._initialValue
        elif ts >= self._timePeriod:
            return self._finalValue
        else:
            c0 = self._coefficients0
            c1 = self._coefficients1
            v = math.Vector2(c0[0] + ts * (c0[1] + ts * (c0[2]  + ts * c0[3])),
                             c1[0] + ts * (c1[1] + ts * (c1[2]  + ts * c1[3])))
            if self._ignore0:
                v = math.Vector2(
                    self._initialValue[0],
                    c1[0] + ts * (c1[1] + ts * (c1[2]  + ts * c1[3])))
            if self._ignore1:
                v = math.Vector2(
                    c0[0] + ts * (c0[1] + ts * (c0[2]  + ts * c0[3])),
                    self._initialValue[1])
            
            return v

    def computeDerivative(self, time, order):
        # compute the time from the beginning of the trajectory
        # because we have constructed the trajectory starting at time 0

        # handle t < ti
        if time < 0:
            if order == 1:
                return self._initialRateV
            elif order > 1:
                return math.Vector2.ZERO
            else:
                return None

        # handle t > tf
        if time >= self._timePeriod:
            if order == 1:
                return self._finalRateV
            elif order > 1:
                return math.Vector2.ZERO
            else:
                return None


        # hand times during the computed trajector
        c0 = self._coefficients0
        c1 = self._coefficients1
        if order < 1 :
            return None
        elif order == 1:
            r = math.Vector2(c0[1] + time * (2 * c0[2] + time * 3 * c0[3]),
                             c1[1] + time * (2 * c1[2] + time * 3 * c0[3]))
            if self._ignore0:
                r = math.Vector2(self._initialRateV[0],
                                 c1[1] + time * (2 * c1[2] + time * 3 * c0[3]))
            if self._ignore1:
                r = math.Vector2(c0[1] + time * (2 * c0[2] + time * 3 * c0[3]),
                                 self._initialRateV[1])

            return r
        elif order == 2:
            r = math.Vector2(2 * c0[2] + 6 * c0[3] * time,
                             2 * c1[2] + 6 * c1[3] * time)

            if self._ignore0:
                r = math.Vector2(0, 2 * c1[2] + 6 * c1[3] * time)
            if self._ignore1:
                r = math.Vector2(2 * c0[2] + 6 * c0[3] * time, 0)

            return r
        elif order == 3:
            r = math.Vector2(6 * c0[3], 6 * c1[3])

            if self._ignore0 or self._ignore1:
                r = math.Vector2.ZERO

            return r
        else:
            return math.Vector2.ZERO

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._initialTime + self._timePeriod

    def getMaxOfDerivative(self, order):
        return None

    def isRelative(self):
        return self._relative


class StepTrajectory(Trajectory):
    """
    This trajectory represents a step change.  That is, before the
    initial time, the output will be the initial value / initial rate
    and after the initial time, the output will be the final value / final rate.
    This type of trajectory is type agnostic so you can use anything from a
    scalar to a quaternion.  This is only really meant to be used for
    testing to compare other trajectories to this as a performance baseline.
    """

    def __init__(self, initialValue, finalValue,
                 initialRate = 0, finalRate = 0,
                 initialTime = 0):
        self._initialValue = initialValue
        self._finalValue = finalValue
        self._initialRate = initialRate
        self._finalRate = finalRate
        self._initialTime = initialTime

        if initialTime == 0:
            self._relative = True

    def computeValue(self, time):
        if time >= self._initialTime:
            return self._finalValue
        else:
            return self._initialValue

    def computeDerivative(self, time, order):
        if order == 1:
            if time >= self._initialTime:
                return self._finalRate
            else:
                return self._initialRate
        else:
            return None

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._initialTime + 0.0001

    def getMaxOfDerivative(self, order):
        return float('inf')

    def isRelative(self):
        return self._relative

class SlerpTrajectory(Trajectory):
    """
    This trajectory interpolates between two quaternions assuming a constant
    angular rate.  The interpolation will occur over a given time period
    starting at the given initial time.
    """

    def __init__(self, initialValue, finalValue,
                 timePeriod, initialTime = 0):

        assert isInstance(initialValue, math.Quaternion), \
            'SlerpTrajectory: initialValue must be a Quaternion'
        assert isInstance(finalValue, math.Quaternion), \
            'SlerpTrajectory: finalValue must be a Quaternion'

        self._initialValue = initialValue
        self._finalValue = finalValue
        self._initialTime = initialTime
        self._timePeriod = timePeriod
        self._finalTime = initialTime + timePeriod

        if initialTime == 0:
            self._relative = True
        
        # slerp is constant angular velocity so lets
        # calculate it once here
        rotQuat = initialValue.errorQuaternion(finalValue)
        angle = math.Radian(0)
        axis = math.Vector3.ZERO
        rotQuat.ToAxisAngle(angle, axis)
        self._angularVelocity = angle / self._timePeriod

    def computeValue(self, time):
        # before the initial time, we need to return the initial value
        if time < self._initialTime:
            return self._initialValue     
   
        # after the final time, we need to return the final value
        elif time > self._finalTime:
            return self._finalValue

        # during the defined time, we calculate the trajectory value
        else:
            pctComplete = (time - self._initialTime) / self._timePeriod
            return math.Quaternion.Slerp(pctComplete, self._initialValue,
                                         self._finalValue, true)
        


    def computeDerivative(self, time, order = 1):
        # during the defined time, compute the derivative
        if time > self._initialTime and time < self._finalTime:
            if order == 1:
                return self._angularVelocity
            else:
                return 0
        # we dont know the pre, post conditions so return None
        else:
            return None

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._finalTime

    def getMaxOfDerivative(self, order):
        return self._angularVelocity

    def isRelative(self):
        return self._relative

class AngularRateTrajectory(Trajectory):
    """
    This trajectory just rotates at a constant
    angular rate between the time interval specified
    """

    def __init__(self, initialValue, rate,
                 timePeriod, initialTime = 0):
        self._initialValue = initialValue
        self._rate = rate
        self._initialTime = initialTime
        self._timePeriod = timePeriod
        self._finalTime = initialTime + timePeriod

        if initialTime == 0:
            self._relative = True

    def computeValue(self, time):
        # before the initial time, we need to return the initial value
        if time < self._initialTime:
            return self._initialValue
        
        # otherwise compute the difference in time between start and now
        elif time > self._finalTime:
            timestep = self._timePeriod
        elif time > self._initialTime and time < self._finalTime:
            timestep = time - self._initialTime

        # calculate the current value based on the timestep and rate
        derivativeQuat = self._initialValue.derivative(self._rate)
        currentValue += timestep * derivativeQuat
        currentValue.normalise()
        return currentValue
        

    def computeDerivative(self, time, order = 1):
        # during the defined time, compute the derivative
        if time > self._initialTime and time < self._finalTime:
            if order == 1:
                return self._rate
            else:
                return 0
        # we dont know the pre, post conditions so return None
        else:
            return None

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._finalTime

    def getMaxOfDerivative(self, order):
        if order == 1:
            return self._rate
        else:
            return 0

    def isRelative(self):
        return self._relative

class Vector2ConstAccelTrajectory(Trajectory):
    """
    This trajectory is to be used for attaining a desired
    velocity by means of a constant acceleration.  It is to
    be used for velocity control and hence there is no
    parameter for a final value.
    """

    def __init__(self, initialValue, initialRate, finalRate,
                 accel,  initialTime = 0):

        self._initialValue = initialValue
        self._initialRate = initialRate
        self._finalRate = finalRate
        self._initialTime = initialTime
        self._accel = accel

        self._changeInRateV = finalRate - initialRate
        self._changeInRateS = changeInRateS.length()

        self._timePeriod = self._changeInRateS / accel
        self._finalTime = initialTime + self._timePeriod

        if initialTime == 0:
            self._relative = True


    def computeValue(self, time):
        # before the initial time, we need to return the initial value
        if time < self._initialTime:
            return self._initialValue
        
        # otherwise compute the difference in time between start and now
        else:
            timestep = time - self._initialTime

        # calculate the current value based on the timestep and rate
        return .5 * self._accel * timestep * timestep
        

    def computeDerivative(self, time, order = 1):
        if time < self._initialTime:
            if order == 1:
                return self._initialRate
            else:
                return None
        elif time < self._finalTime:
            timestep = time - self._initialTime
            if order == 1:
                return self._accel * timestep
            elif order == 2:
                return self._accel
            else:
                return 0
            
        else:
            if order == 1:
                return self._finalRate
            else:
                return 0
            

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._finalTime

    def getMaxOfDerivative(self, order):
        if order == 1:
            return self._finalRate
        else:
            return 0

    def isRelative(self):
        return self._relative

#class Vector2SigmoidAccelTrajectory(Trajectory):
