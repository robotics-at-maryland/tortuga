from VehicleInterface import *
import threading
from time import localtime
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
    #interval in seconds that we send propulsion a message
    prop_time = 0.5     
    
    def __init__(self):
        self.light_sensor = LightSensor()
        self.acclerometer = Accelerometer("accelerometer")
        self.x_thruster = Thruster("thruster")
        self.y_thruster = Thruster("thruster")
        self.z_thruster = Thruster("thruster")
    """
    Function starts the propulsion message thread, activated every
    self.prop_time seconds that sends a message with the requested
    power of each direction axis
    """
    def send_propulsion_messaging(self):
        def reset_propulsion_ticker():
            self.propulsion_message_thread = threading.Timer(self.prop_time,self.send_propulsion_messaging)
            self.propulsion_message_thread.start()
        print "sending propulsion a message"
        reset_propulsion_ticker()    
    """
    Function stops the sending of messages to the propulsion system every
    prop_time seconds
    """
    def stop_propulsion_messaging(self):
        robot.propulsion_message_thread.cancel()
    """
    Helper function that allows the propulsion messager to repeat
    """
    
        
    def operate(self):
        self.send_propulsion_messaging()
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



robot = Vehicle()
robot.operate()

