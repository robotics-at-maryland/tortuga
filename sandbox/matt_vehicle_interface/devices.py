class DepthSensor():
    def __init__(self):
        print "Creating a new depth sensor"
    def set_depth(self,value):
         self.depth = value
    def depth(self):
        return self.depth
    
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

class AbstractController():
    def __init__(self):
        print "Creating a new abstract controller"
        
class DepthSensor():
    def __init__(self,sc):
        self.sensor_comm = sc
    def depth(self):
        return self.sensor_comm.depth()
    
