'''
This module stores device objects. Devices include things like sensors, thrusters etc.
A vehicle has devices.
'''

'''
This is a thruster object. A thruster has a power setting between -1.0 and 1.0, and an address.
The object, remember, is a solely software thing. Only a vehicle can communicate with the motor controller
to actually set a thruster power.
'''
class Thruster():
    def __init__(self,power,address):
        self.power = power
        self.address = address
    def set_power(self,power):
        self.power = power
	if self.power > 1.0:
	    self.power = 1.0
	elif self.power < -1.0:
	    self.power = -1.0
	elif self.power * 1000 < 0.1 and self.power > 0.0:   #bad check for the float zero problem
	    self.power = 0.0
	elif self.power * 1000 > -0.1 and self.power < 0.0:
	    self.power = 0.0
    def power_up(self,increment):
        new_pow = self.power + increment
	self.set_power(new_pow)

'''
This is a depth sensor object. The depth sensor is actually passed a sensor communicator, so
it is capable of actually getting a value from the sensor board. It returns this value when 
anyone asks for it.
'''        
class DepthSensor():
    def __init__(self,sc):
        self.sensor_comm = sc
    def depth(self):
        return self.sensor_comm.depth()
    
