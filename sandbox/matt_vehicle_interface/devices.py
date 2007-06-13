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
    def power_up(self,increment):
        self.power += increment

class AbstractController():
    def __init__(self):
        print "Creating a new abstract controller"
    
