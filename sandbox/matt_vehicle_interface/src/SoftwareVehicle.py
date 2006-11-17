from VehicleInterface import *
import time,os,sys

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
        self.acclerometer = Accelerometer()
        self.x_thruster = Thruster()
        self.y_thruster = Thruster()
        self.z_thruster = Thruster()
    def operate(self):
        def hello():
            print "hello, world"
            t = Timer(30.0, hello)
            t.start() # after 30 seconds, "hello, world" will be printed
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