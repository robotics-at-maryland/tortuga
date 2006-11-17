from VehicleInterface import *
import threading
from time import localtime
import PropulsionController
"""
    Sensors:
        Light Sensor object
        Sonar array object
        Accelerometer object
        Gyroscope object
        Camera Object (might be through different end)
        Depth transducer object
    Actuators:
        X number of thruster objects
        Marker Dropper object
    """
    
class Vehicle(IVehicle):
    
    def __init__(self):        
        self.light_sensor = LightSensor()
        self.acclerometer = Accelerometer("accelerometer")
        self.x_thruster = Thruster("thruster")
        self.y_thruster = Thruster("thruster")
        self.z_thruster = Thruster("thruster")
        
        self.propulsion_controller = PropulsionController.Controller()
        self.propulsion_controller.set_vehicle(self)
        
        self.propulsion_instruction = PropulsionMessage(10,0,0,0,0)
    """
    Function that begins the operatiorn of the vehicle
    """
        
    def operate(self):
        self.propulsion_controller.start()
    def terminate(self):
        self.propulsion_controller.close()
        
    def process_sensor_packet(self,packet):
        if packet.type == "light":
            print "Light packet received at: " + packet.time
            print "Value: " + packet.value
    def set_thruster_power(self,axis,value):
        if value > 100 or value < -100:
            print "Invalid value"
            return
        else:
            if axis == "x":
                self.x_thruster.power = value
            elif axis == "y":
                self.y_thruster.power = value
            elif axis == "z":
                self.z_thurster.power = value
            else:
                print "Invalid axis"
                return


###################

robot = Vehicle()
robot.operate()

