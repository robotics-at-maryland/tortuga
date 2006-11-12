from VehicleInterface import *

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