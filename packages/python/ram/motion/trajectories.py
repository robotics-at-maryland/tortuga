# Copyright (C) 2010 Maryland Robotics Club
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


class ScalarCubicTrajectory(Trajectory):
    """
    Defines the creation and evaluation of a trajectory based
    on a cubic polynomial.  The trajectory is a scalar valued
    function and therefore the values returned will be scalars.
    This trajectory will be useful for changing depths.
    """

    # this is a parameter that relates the max rate to the time
    # interval in which the trajectory will be defined
    MAGIC_RATE_SLOPE = 10

    def __init__(self, initialValue, finalValue, initialTime,
                 initialRate = 0, finalRate = 0, maxRate = 3):

        # keep track of the arguments
        self._initialValue = initialValue
        self._finalValue = finalValue
        self._initialRate = initialRate
        self._finalRate = finalRate
        self._initialTime = initialTime

        # compute the final time
        self._timeInterval = self.approximateTimeInterval(
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
            tf = self._timeInterval

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
        elif time >= self._timeInterval:
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
        if time >= self._timeInterval:
            if order == 1:
                return self._finalRate
            elif order > 1:
                return 0
            else:
                return None

        # hand times during the computed trajector
        c = self._coefficients
        if order < 1 :
            return None
        elif order == 1:
            return c[1] + time * (2 * c[2] * time + (3 * time * c[3]))
        elif order == 2:
            return 2 * c[2] + 6 * c[3] * time
        elif order == 3:
            return 6 * c[3]
        else:
            return 0

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._initialTime + self._timeInterval

    def getMaxOfDerivative(self, order):
        if order == 1:
            return self._maxRate
        elif order == 2:
            return 2 * coefficients[2]
        elif order > 2:
            return 0
        else:
            return None

    def approximateTimeInterval(self, changeInValue, maxRate):

        if(maxRate < 0):
            maxRate = 0.1
        elif(maxRate > 5):
            maxRate = 5

        slope = self.MAGIC_RATE_SLOPE / maxRate
        return slope * changeInValue

class Vector2CubicTrajectory(Trajectory):
    """
    This trajectory goes in a straight line between 2D points.
    It has two continous derivatives. This is very similar to
    the scalar cubic trajectory.
    """
    
    # this is a parameter that relates the max rate to the time
    # interval in which the trajectory will be defined
    MAGIC_RATE_SLOPE = 10

    def __init__(self, initialValue, finalValue, initialTime,
                 initialRate = 0, finalRate = 0, maxRate = 3):

        # keep track of the arguments V - vector quantity, S - scalar quantity
        self._initialValue = initialValue
        self._finalValue = finalValue
        self._initialRateV = initialRate
        self._finalRateV = finalRate
        self._initialTime = initialTime
        self._changeInValueV = finalValue - initialValue 

        # turn this into a 1D problem
        self._changeInValueS = self._changeInValueV.length()
        self._initialRateS = initialRate.length()
        self._finalRateS = finalRate.length()
   
        # compute the final time
        self._timeInterval = self.approximateTimeInterval(
            self._changeInValueS, maxRate)

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
            tf = self._timeInterval

            # compute matrix A and vector b for equation Ax = b
            ti_p2 = ti * ti
            ti_p3 = ti_p2 * ti

            tf_p2 = tf * tf
            tf_p3 = tf_p2 * tf

            A = numpy.array([[1, ti, ti_p2, ti_p3],
                             [1, tf, tf_p2, tf_p3],
                             [0, 1, 2 * ti, 3 * ti_p2],
                             [0, 1, 2 * tf, 3 * tf_p2]])
        
            b = numpy.array([0, self._changeInValueS, self._initialRateS, self._finalRateS])

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
        elif time >= self._timeInterval:
            return self._finalValue
        else:
            c = self._coefficients
            scalarValue = c[0] + time * (c[1] + time * (c[2]  + time * c[3]))
            return self._initialValue + self._projectOntoAxes(scalarValue)

    def computeDerivative(self, time, order):
        # compute the time from the beginning of the trajectory
        # because we have constructed the trajectory starting at time 0
        time = time - self._initialTime

        # handle t < ti
        if time < 0:
            if order == 1:
                return self._initialRateV
            elif order > 1:
                return 0
            else:
                return None

        # handle t > tf
        if time >= self._timeInterval:
            if order == 1:
                return self._finalRateV
            elif order > 1:
                return 0
            else:
                return None

        # hand times during the computed trajector
        c = self._coefficients
        if order < 1 :
            return None
        elif order == 1:
            scalarRate = c[1] + time * (2 * c[2] * time + (3 * time * c[3]))
            return self._projectOntoAxes(scalarRate)
        elif order == 2:
            scalarRate = 2 * c[2] + 6 * c[3] * time
            return self._projectOntoAxes(scalarRate)
        elif order == 3:
            scalarRate = 6 * c[3]
            return self._projectOntoAxes(scalarRate)
        else:
            return math.Vector2.ZERO

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._initialTime + self._timeInterval

    def getMaxOfDerivative(self, order):
        if order == 1:
            return self._projectOntoAxes(self._maxRate)
        elif order == 2:
            scalarRate = 2 * coefficients[2]
            return self._projectOntoAxes(scalarRate)
        elif order > 2:
            return 0
        else:
            return None

    def approximateTimeInterval(self, changeInValue, maxRate):
        iRate = int(maxRate)

        if(maxRate < 0):
            maxRate = 0.1
        elif(maxRate > 5):
            maxRate = 5

        slope = self.MAGIC_RATE_SLOPE / maxRate
        return slope * changeInValue


    def _projectOntoAxes(self, scalar):
        xCoord = scalar * self._changeInValueV[0] / self._changeInValueS
        yCoord = scalar * self._changeInValueV[1] / self._changeInValueS
        return math.Vector2(xCoord,yCoord)


class StepTrajectory(Trajectory):
    """
    This trajectory is a step input that returns a scalar
    """

    def __init__(self, finalValue, finalRate):
        self._finalValue = finalValue
        self._finalRate = rate
        self._initialTime = timer.time()

    def computeValue(self, time):
        return self._finalValue

    def computeDerivative(self, time, order):
        if order == 1:
            return self._finalRate
        else:
            return None

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._initialTime

    def getMaxOfDerivative(self, order):
        return float('inf')


class SlerpTrajectory(Trajectory):
    """
    This trajectory is meant to interpolate between two
    quaternions
    """

    def __init__(self, initialQuat, finalQuat,
                 initialTime, finalTime):
        self._initialQuat = initialQuat
        self._finalQuat = finalQuat
        self._initialTime = initialTime
        self._finalTime = finalTime
        self._timeSpan = finalTime - initialTime
        
        # slerp is constant angular velocity so lets
        # calculate it once here
        rotQuat = initialQuat.errorQuaternion(finalQuat)
        angle = math.Radian(0)
        axis = math.Vector3.ZERO
        rotQuat.ToAxisAngle(angle, axis)
        self._angularVelocity = angle / self._timeSpan

    def computeValue(self, time):
        if time > self._initialTime and time < self._finalTime:
            pctComplete = (time - self._initialTime) / self._timeSpan
            return math.Quaternion.Slerp(pctComplete, self._initialQuat,
                                         self._finalQuat, true)
        else:
            return self._finalQuat
        

    def computeDerivative(self, time, order = 1):
        if time > self._initialTime and time < self._finalTime:
            if order == 1:
                return self._angularVelocity
            else:
                return 0
        else:
            return 0

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._finalTime

    def getMaxOfDerivative(self, order):
        return self._angularVelocity

class AngularRateTrajectory(Trajectory):
    """
    This trajectory just rotates at a constant
    angular rate between the time interval specified
    """

    def __init__(self, initialQuat, angularVelocity,
                 initialTime, finalTime):
        self._currentQuat = initialQuat
        self._angularVelocity = angularVelocity
        self._initialTime = initialTime
        self._finalTime = finalTime

    def computeValue(self, time):
        if time > self._initialTime and time < self._finalTime:
            timestep = time - self._initialTime
            derivativeQuat = self._currentQuat.derivative(m_angularVelocity)
            self._currentQuat += timestep * derivativeQuat
            self._currentQuat.normalise()
            return self._currentQuat
        else:
            return self._currentQuat
        

    def computeDerivative(self, time, order = 1):
        if time > self._initialTime and time < self._finalTime:
            if order == 1:
                return self._angularVelocity
            else:
                return 0
        else:
            return 0

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._finalTime

    def getMaxOfDerivative(self, order):
        if order == 1:
            return self._angularVelocity
        else:
            return 0
