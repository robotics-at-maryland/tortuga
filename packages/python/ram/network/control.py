
import ext.core as core
import ext.math as math
import ext.control as control
import ext.estimation as estimation
import ext.network as network
import ext.vehicle as vehicle

class RemoteController(core.Subsystem):
    def __init__(self, config, deps):
        core.Subsystem.__init__(self, config.get('name', 'RemoteController'))

        # Grab the subsystems we need
        self._eventHub = core.Subsystem.getSubsystemOfType(
            core.QueuedEventHub, deps, nonNone = True)

        self._controller = core.Subsystem.getSubsystemOfType(
            control.IController, deps, nonNone = True)

        self._estimator = core.Subsystem.getSubsystemOfType(
            estimation.IStateEstimator, deps, nonNone = True)

        self._vehicle = core.Subsystem.getSubsystemOfType(
            vehicle.IVehicle, deps)


        self._yawChange = config.get('yawChange', 10)
        
        self._speedGain = config.get('speedGain', 3)
        self._tSpeedGain = config.get('tSpeedGain', 3)
        self._angleYawGain = config.get('yawGain', 10)

        self._speed = 0
        self._tspeed = 0

        self._lastSpeedCommand = 0
        self._lastTSpeedCommand = 0
        self._lastRollCommand = 0
        self._lastPitchCommand = 0
        self._lastYawCommand = 0

        self._connections = []

        registrationInfo = ['EMERGENCY_STOP', 'YAW_LEFT', 'YAW_RIGHT',
                            'PITCH_UP', 'PITCH_DOWN', 'ROLL_LEFT',
                            'ROLL_RIGHT', 'FORWARD_MOVEMENT',
                            'DOWNWARD_MOVEMENT', 'LEFT_MOVEMENT',
                            'RIGHT_MOVEMENT', 'DESCEND', 'ASCEND',
                            'SETSPEED', 'TSETSPEED', 'ANGLEYAW',
                            'ANGLEPITCH', 'ANGLEROLL', 'MAINTAIN_DEPTH',
                            'FIRE_MARKER_DROPPER', 'FIRE_TORPEDO_LAUNCHER']
                            
        for type_ in registrationInfo:
            realType = getattr(network.EventType, type_)
            handler = getattr(self, '_%s' % type_.lower())
            self._registerForEvent(realType, handler)

    def __del__(self):
        for conn in self._connections:
            conn.disconnect()

    def backgrounded(self):
        return True

    def _registerForEvent(self, type_, callback):
        conn = self._eventHub.subscribeToType(type_, callback)
        self._connections.append(conn)

    def _emergency_stop(self, event):
        # Safe the thrusters (if vehicle is available)
        if self._vehicle is not None:
            self._vehicle.safeThrusters()

    def _yaw_left(self, event):
        ori = self._estimator.getEstimatedOrientation()
        rate = event.number
        self._controller.rotate(ori, math.Vector3(0, 0, rate))

    def _yaw_right(self, event):
        ori = self._estimator.getEstimatedOrientation()
        rate = event.number
        self._controller.rotate(ori, math.Vector3(0, 0, rate))

    def _pitch_up(self, event):
        ori = self._estimator.getEstimatedOrientation()
        rate = event.number
        self._controller.rotate(ori, math.Vector3(0, rate, 0))

    def _pitch_down(self, event):
        ori = self._estimator.getEstimatedOrientation()
        rate = event.number
        self._controller.rotate(ori, math.Vector3(0, rate, 0))

    def _roll_left(self, event):
        ori = self._estimator.getEstimatedOrientation()
        rate = event.number
        self._controller.rotate(ori, math.Vector3(rate, 0, 0))

    def _roll_right(self, event):
        ori = self._estimator.getEstimatedOrientation()
        rate = event.number
        self._controller.rotate(ori, math.Vector3(rate, 0, 0))

    def _forward_movement(self, event):
        self._speed = event.number
        self._setvelocity()

    def _downward_movement(self, event):
        self._controller.translate(pos, vel)

    def _left_movement(self, event):
        self._tspeed = event.number
        self._setvelocity()

    def _right_movement(self, event):
        self._tspeed = event.number
        self._setvelocity()

# Descent and Ascent currently do not work because the putton press
# event is only being sent once however the button release event is
# properly being sent.

    def _descend(self, event):
        """
        When we want to descend, we send a positive downward 
        speed to the controller.
        """
        depth = self._estimator.getEstimatedDepth()
        self._controller.changeDepth(depth + 0.8)

    def _ascend(self, event):
        """
        When we want to ascend, we send a negative downward
        speed to the controller.
        """
        depth = self._estimator.getEstimatedDepth()
        self._controller.changeDepth(depth - 0.8)

    def _setspeed(self, event):
        """
        When a speed command is received, we need to pass the
        desired speed onto the controller.  If the speed command is zero,
        we only send it ONCE because sending it repeatedly would prevent
        the AI from controlling the robot
        """
        self._speed = (event.number / 100.0) * self._speedGain
        if self._speed == 0 and self._lastSpeedCommand == 0:
            # avoid sending repeated speed = 0 commands
            # note that we do want to send commands repeatedly if
            # the speed is not zero
            pass
        else:
            self._setvelocity()
            self._lastSpeedCommand = self._speed

    def _tsetspeed(self, event):
        """
        When a sideways speed command is received, we need to pass the
        desired speed onto the controller.  If the speed command is zero,
        we only send it ONCE because sending it repeatedly would prevent
        the AI from controlling the robot
        """
        self._tspeed = (event.number / 100.0) * self._tSpeedGain
        if self._tspeed == 0 and self._lastTSpeedCommand == 0:
            # avoid sending repeated sideways speed = 0 commands
            # note that we do want to send commands repeatedly if
            # the speed is not zero
            pass
        else:
            self._setvelocity()
            self._lastTSpeedCommand = self._tspeed

    def _setvelocity(self):
        """
        Given the current desired speeds, calcualte what our inertial
        frame velocity should be and send the appropriate command to
        the controller.
        """
        pos = self._estimator.getEstimatedPosition()
        yaw = self._estimator.getEstimatedOrientation().getYaw()
        nRb = math.Matrix2.nRb(yaw)
        self._controller.translate(
            pos, nRb * math.Vector2(self._speed, self._tspeed))

    def _angleyaw(self, event):
        """
        When we want to yaw the robot, send a command to the controller
        to rotate the robot around the body z axis proportional to the
        event command.  When we receive a zero rate command, we should
        only pass along the command to the controller ONCE.
        """
        change = (event.number / 100.0) * self._angleYawGain
        if change == 0 and self._lastYawCommand == 0:
            # dont sent ZERO command twice
            # note that we do want to send commands repeatedly if
            # the speed is not zero
            pass
        else:
            ori = self._estimator.getEstimatedOrientation()
            qHeading = self._controller.holdCurrentHeadingHelper(ori)
            self._controller.rotate(qHeading * math.Quaternion(
                    math.Degree(change), math.Vector3.UNIT_Z))
            self._lastYawCommand = change

    def _anglepitch(self, event):
        """
        When we want to pitch the robot, send a command to the controller
        to rotate the robot around the body y axis proportional to the
        event command.  When we receive a zero rate command, we should
        only pass along the command to the controller ONCE.
        """
        rate = event.number
        if rate == 0 and self._lastPitchCommand == 0:
            # dont sent ZERO command twice
            # note that we do want to send commands repeatedly if
            # the speed is not zero
            pass
        else:
            ori = self._estimator.getEstimatedOrientation()
            self._controller.rotate(ori, math.Vector3(0, rate, 0))
            self._lastPitchCommand = rate

    def _angleroll(self, event):
        """
        When we want to roll the robot, send a command to the controller
        to rotate the robot around the body x axis proportional to the
        event command.  When we receive a zero rate command, we should
        only pass along the command to the controller ONCE.
        """
        rate = event.number
        if rate == 0 and self._lastRollCommand == 0:
            # dont sent ZERO command twice
            # note that we do want to send commands repeatedly if
            # the speed is not zero
            pass
        else:
            ori = self._estimator.getEstimatedOrientation()
            self._controller.rotate(ori, math.Vector3(rate, 0, 0))
            self._lastRollCommand = rate

    def _fire_marker_dropper(self, event):
        self._vehicle.dropMarker()

    def _fire_torpedo_launcher(self, event):
        self._vehicle.fireTorpedo()

    def _maintain_depth(self, event):
        self._controller.holdCurrentDepth()

core.SubsystemMaker.registerSubsystem('RemoteController', RemoteController)
