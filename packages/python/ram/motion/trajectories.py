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
    An abstract class for motion trajctories
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

    # The following slopes were determined by experimentation to give us some
    # way to control the speed of the dive
    MAGIC_RATE_SLOPES = [4, 3, 2.5, 2, 1.5]

    def __init__(self, initialValue, finalValue, initialTime, initialRate = 0,
                 finalRate = 0, maxRate = 4):

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
        iRate = int(maxRate)

        if(iRate < 1):
            iRate = 1
        elif(iRate > 5):
            iRate = 5

        slope = self.MAGIC_RATE_SLOPES[iRate-1]
        return slope * changeInValue


class StepTrajectory(Trajectory):
    """
    This trajectory is a step input that returns a scalar
    """

    def __init__(self, finalValue, finalRate):
        self._finalValue = finalValue
        self._finalRate = finalRate
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
