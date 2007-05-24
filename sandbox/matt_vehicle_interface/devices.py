class DepthSensor():
    def __init__(self):
        print "Creating a new depth sensor"
    def set_depth(self,value):
         self.depth = value
    def depth(self):
        return self.depth
    
class AbstractController():
    def __init__(self):
        print "Creating a new abstract controller"
    
