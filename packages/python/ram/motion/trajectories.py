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

class Trajectory(object):
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

    def __init__(self, initialValue, finalValue, initialTime = 0,
                 initialRate = 0, avgRate = 0.5):

        # keep track of the arguments
        self._initialValue = initialValue
        self._finalValue = finalValue
        self._initialRate = initialRate
        self._finalRate = 0
        self._initialTime = initialTime

        # detect relative trajectory
        if initialTime == 0:
            self._relative = True

        # compute the final time
        self._timePeriod = pmath.fabs(finalValue - initialValue) / avgRate

        # the matrix will be singular if initial and final values are equal
        # this shouldn't happen but 
        if initialValue == finalValue:
            self._coefficients = numpy.array([0, 0, 0, 0])
            self._maxRate = 0

        else:
            # we solve for the polynomial starting at time 0 to avoid
            # numerical problems that could occur if we use the given
            # initial time.
            ti = 0
            tf = self._timePeriod

            # compute matrix A and vector b for equation Ax = b
            ti_p2 = ti ** 2
            ti_p3 = ti ** 3

            tf_p2 = tf ** 2
            tf_p3 = tf ** 3

            A = numpy.array([[1, ti, ti_p2 , ti_p3    ],
                             [1, tf, tf_p2 , tf_p3    ],
                             [0, 1 , 2 * ti, 3 * ti_p2],
                             [0, 1 , 2 * tf, 3 * tf_p2]])
            
            b = numpy.array([initialValue, finalValue,
                             self._initialRate, self._finalRate])

            # solve for coefficient vector x
            x = numpy.linalg.solve(A,b)

            self._coefficients = x
            self._maxRate = x[1] - (x[2] ** 2) / (3 * x[3])

    def computeValue(self, time):
        # compute the time from the beginning of the trajectory
        # because we have constructed the trajectory starting at time 0
        time = time - self._initialTime

        # take care of boundary conditions for when the polynomial is not valid
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

class Vector2CubicTrajectory(Trajectory):
    """
    This trajectory goes in a straight line between 2D points.
    It has two continous derivatives. This is very similar to
    the scalar cubic trajectory.
    """
    def __init__(self, initialValue, finalValue, initialTime = 0,
                 initialRate = math.Vector2.ZERO, avgRate = 0.25):

        # keep track of the arguments V - vector quantity, S - scalar quantity
        self._initialValue = initialValue
        self._finalValue = finalValue
        self._changeInValueV = finalValue - initialValue
        self._changeInValueS = self._changeInValueV.length()

        self._initialRate = initialRate
        self._finalRate = math.Vector2.ZERO
        self._changeInRateV = self._finalRate - self._initialRate
        self._changeInRateS = self._changeInRateV.length()
        
        self._initialTime = initialTime
        if initialTime == 0:
            self._relative = True

        # compute the final time
        self._timePeriod = self._changeInValueV.length() / avgRate
        self._finalTime = self._initialTime + self._timePeriod

        if initialValue == finalValue:
            self._maxRate = math.Vector2.ZERO
            self._coefficients = numpy.array([0, 0, 0, 0, 0])

        else:
            # we solve for the polynomial starting at time 0 to avoid
            # numerical problems that could occur if we use the given
            # initial time.
            ti = 0
            tf = self._timePeriod
            
            # compute matrix A and vector b for equation Ax = b
            ti_p2 = ti ** 2
            ti_p3 = ti ** 3
            
            tf_p2 = tf ** 2
            tf_p3 = tf ** 3

            A = numpy.array([[1, ti, ti_p2,  ti_p3    ],
                             [1, tf, tf_p2,  tf_p3    ],
                             [0, 1,  2 * ti, 3 * ti_p2],
                             [0, 1,  2 * tf, 3 * tf_p2]])
            
            b = numpy.array([0, self._changeInValueS, 0, self._changeInRateS])
            
            # solve for coefficient vector x
            x = numpy.linalg.solve(A,b)
            self._coefficients = x
            
            self._maxRate = math.Vector2.ZERO # TODO


    def computeValue(self, time):
        # compute the time from the beginning of the trajectory
        # because we have constructed the trajectory starting at time 0
        ts = time - self._initialTime

        if ts < 0:
            return self._initialValue
        elif ts >= self._timePeriod:
            return self._finalValue
        else:
            c = self._coefficients
            valueScalar = c[0] + ts * (c[1] + ts * (c[2] + ts * c[3]))
            v = self._projectOntoAxes(valueScalar)
            v0 = v[0] + self._initialValue[0]
            v1 = v[1] + self._initialValue[1]
            return math.Vector2(v0, v1)

    def computeDerivative(self, time, order):
        # compute the time from the beginning of the trajectory
        # because we have constructed the trajectory starting at time 0

        # handle t < ti
        if time < 0:
            if order == 1:
                return self._initialRate
            elif order > 1:
                return math.Vector2.ZERO
            else:
                return None

        # handle t > tf
        if time >= self._timePeriod:
            if order == 1:
                return self._finalRate
            elif order > 1:
                return math.Vector2.ZERO
            else:
                return None


        # hand times during the computed trajector
        c = self._coefficients
        if order < 1 :
            return None
        elif order == 1:
            rateScalar = c[1] + time * (2 * c[2] + time * 3 * c[3])
            v = self._projectOntoAxes(rateScalar)
            v0 = v[0] + self._initialRate[0]
            v1 = v[1] + self._initialRate[1]
            return math.Vector2(v0, v1)
        elif order == 2:
            rateScalar = 2 * c[2] + 6 * c[3] * time
            v = self._projectOntoAxes(rateScalar)
            return v
        elif order == 3:
            rateScalar = 6 * c[3], 6 * c[3]
            v = self._projectOntoAxes(rateScalar)
            return v
        else:
            return math.Vector2.ZERO

    def getInitialTime(self):
        return self._initialTime

    def getFinalTime(self):
        return self._finalTime

    def getMaxOfDerivative(self, order):
        return None

    def isRelative(self):
        return self._relative

    def _projectOntoAxes(self, scalar):
        xCoord = scalar * self._changeInValueV[0] / self._changeInValueS
        yCoord = scalar * self._changeInValueV[1] / self._changeInValueS
        return math.Vector2(xCoord,yCoord)

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
        self._changeInRateS = self._changeInRateV.length()

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
        scalar = .5 * self._accel * timestep * timestep
        return self._projectOntoAxes(scalar)
        

    def computeDerivative(self, time, order = 1):
        if time < self._initialTime:
            if order == 1:
                return self._initialRate
            else:
                return None
        elif time < self._finalTime:
            timestep = time - self._initialTime
            if order == 1:
                return self._projectOntoAxes(self._accel * timestep)
            elif order == 2:
                return self._projectOntoAxes(self._accel)
            else:
                return math.Vector2.ZERO
            
        else:
            if order == 1:
                return self._finalRate
            else:
                return math.Vector2.ZERO
            

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

    def _projectOntoAxes(self, scalar):
        xCoord = scalar * self._changeInRateV[0] / self._changeInRateS
        yCoord = scalar * self._changeInRateV[1] / self._changeInRateS
        return math.Vector2(xCoord,yCoord)

#class Vector2SigmoidAccelTrajectory(Trajectory):
class ScalarSVAJTrajectory(Trajectory):
    """
    Defines the creation of a trajectory t
    """

    def __init__(self, initialValue, finalValue, initialTime = 0,
                 initialRate = 0, maxRate = 0.7291):

        # keep track of the arguments
        self._initialValue = initialValue
        self._finalValue = finalValue
        self._initialRate = initialRate
        self._finalRate = 0
        self._initialTime = initialTime

        # detect relative trajectory
        if initialTime == 0:
            self._relative = True

        # compute the final time
        self._timePeriod = (35.0 / 16.0) * \
            pmath.fabs(finalValue - initialValue) / maxRate
        self._maxRate = 0 # TODO

        p = 4.0
        q = 5.0
        r = 6.0
        s = 7.0

        self._cp = q * r * s / ((q - p) * (r - p) * (s - p))
        self._cq = p * r * s / ((p - q) * (r - q) * (s - q))
        self._cr = p * q * s / ((p - r) * (q - r) * (s - r))
        self._cs = p * q * r / ((p - s) * (q - s) * (r - s))

    def computeValue(self, time):
        # compute the time from the beginning of the trajectory
        # because we have constructed the trajectory starting at time 0
        time = time - self._initialTime

        # take care of boundary conditions for when the polynomial is not valid
        if time < 0:
            return self._initialValue
        elif time >= self._timePeriod:
            return self._finalValue
        else:
            [cp, cq, cr, cs] = self._getCoefficients()
            chg = self._finalValue - self._initialValue
            rt = time / self._timePeriod
            iv = self._initialValue
            val = iv + chg * (cp * rt**4 + cq * rt**5 + cr * rt**6 + cs * rt**7)
            return val
            
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
        [cp, cq, cr, cs] = self._getCoefficients()
        chg = self._finalValue - self._initialValue
        rt = time / self._timePeriod # fraction complete
        it = 1 / self._timePeriod # inverse time

        if order < 1 :
            return None
        elif order == 1:
            r = chg * it * ( \
                cp * 4 * rt**3 + \
                    cq * 5 * rt**4 + \
                    cr * 6 * rt**5 + \
                    cs * 7 * rt**6)
            return r
        elif order == 2:
            r = chg * it**2 * ( \
                cp * 12 * rt**2 + \
                    cq * 20 * rt**3 + \
                    cr * 30 * rt**4 + \
                    cs * 42 * rt**5)
            return r
        elif order == 3:
            r = chg * it**3 * ( \
                cp * 24 * rt + \
                    cq * 60 * rt**2 + \
                    cr * 120 * rt**3 + \
                    cs * 210 * rt**4)
            return r
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
            return 0
        elif order > 2:
            return 0
        else:
            return None

    def isRelative(self):
        return self._relative

    def _getCoefficients(self):
        return [self._cp, self._cq, self._cr, self._cs]
