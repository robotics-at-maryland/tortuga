real_property = property

class property_meta(type):
    """
    This allow easy creation of properties that can be get, set, or deleted.
    See the python reciepe (in the dicussion) here: 
        http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/410698
    """
    def __new__(meta, class_name, bases, new_attrs):
        if bases == (object,):
            # The property class itself
            return type.__new__(meta, class_name, bases, new_attrs)
        fget = new_attrs.get('fget')
        fset = new_attrs.get('fset')
        fdel = new_attrs.get('fdel')
        fdoc = new_attrs.get('__doc__')
        return real_property(fget, fset, fdel, fdoc)

class property(object):

    __metaclass__ = property_meta

    def __new__(cls, fget=None, fset=None, fdel=None, fdoc=None):
        if fdoc is None and fget is not None:
            fdoc = fget.__doc__
        return real_property(fget, fset, fdel, fdoc)

class IVehicle(object):
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
    def __init__(self):
        print "initialized a vehicle interface"
    def process_sensor_packet(self,packet):
        print "must be implemented, but there are no interfaces in python"
    

class SensorPacket(object):
    def __init(self,type,value,time):
        self.type = type;
        self.value = value;
        self.time = time;
        
class Sensor(object):
    """
    A superclass for a Sensor object of all types. All that a sensor shares with other sensors right now is
    a type. This should change as we find other similarities (maybe a primary value for instance)  
    """
    def __init__(self,type):
        self._type = type
        self.history = [];
    class type(property):
        def fget(self):
            return self._type

class LightSensor(Sensor):
    """
    A superclass for all sensors that are light sensors. Definintly includes
    intensity value, right now between 0 and 255.
    """
    min_value = 0;
    max_value = 255;

    def __init__(self):
        print "Initializing a light sensor"
        self._type = "light"
        self._value = 0

    class value(property):
        def fset(self, value):
            if value >= LightSensor.min_value and value <= LightSensor.max_value:
                self._value = value
            else:
                print "Cannot set value. Value out of range"
        def fget(self):
            return self._value

    def __sub__(self,other):
        return self.value - other.value
    def __str__(self):
        print "Light Sensor Value:",self.value

#a light sensor that takes color into account. color is represented right now with
#a 3 element tuple, R,G,B with values from 0 to 255
class ColorLightSensor(LightSensor):
    class color(property):
        def fset(self, r, g, b):
            self._color = (r,g,b)
        def fget(self):
            return self._color;
    def __str__(self):
        print "Light Sensor Value:",self.value,"\nColor:",self.color

# a slightly dumb class for an accelerometer
class Accelerometer(Sensor):
    min_value = -255;
    max_value = 255;

    class value(property):
        def fset(self, x, y, z):
            self._acceleration_vector = (x,y,z)
        def fget(self):
            return self._acceleration_vector

    class x_acceleration(property):
        def fget(self):
            return self._acceleration_vector(0);

    class y_acceleration(property):
        def fget(self):
            return self._acceleration_vector(1);

    class z_acceleration(property):
        def fget(self):
            return self._acceleration_vector(2);

    def display(self):
        print self._acceleration_vector

class DepthSensor(Sensor):
    """
    Simple sensor depth sensor, has a value that is scaled to it's depth
    """
    min_value = 0
    max_value = 255

    class value(property):
        def fset(self, depth):
            self._depth = depth

    class depth(property):
        def fget(self):
            return self._depth

    def __str__(self):
        print self.depth

#the most complicated sensor interface, the sonar array
class Sonararray(Sensor):
    """
    Proximity is the variable i use to represent distance to the sound
    source. this can, and should, be changed with better understanding
    """
    class sonar_values(property):
        def fset(self,a,b,c):
            self._a_proximity = a
            self._b_proximity = b
            self._c_proximity = c
            self._proximity = (a,b,c)
        def fget(self):
            return self._proximity
    
    def set_sonar_by_letter(self,letter,value):
        if letter == "a":
            self.a_proximity = value
        elif letter == "b":
            self.b_proximity = value
        elif letter == "c":
            self.c_proximity = value
        else:
            print "Invalid sonar letter"
    def __str__(self):
        print self.proximity

class Gyroscope(Sensor):
    """
    Stores the angle (with 0 representing the verticle angle?)
    """
    min_angle = 0
    max_angle = 360

    class angle(property):
        def fset(self, angle):
            self._angle = angle
        def fget(self):
            return self._angle
    
class Actuator(object):
    def __init__(self,type):
        self.type = type
        
class Thruster(Actuator):
    """
    A default thruster class
    """
    class power(property):
        def fset(self, power):
            self._type = "thruster"
            self._power = power
        def fget(self):
            return self._power

    def stop(self):
        self.power = 0;
    
class PropulsionMessage:
    def __init__(self,x,y,z,clock,counter):
        self.x_power = x
        self.y_power = y
        self.z_power = z
        self.clock_power = clock
        self.counterclock_power = counter
    def compare(self,other):
        if self.x_power == other.x_power and self.y_power == other.y_power and self.z_power == other.z_power and self.clock_power == other.clock_power and self.counterclock_power == other.counterclock_power:
            return True
        else:
            return False
    def __str__(self):
        return str(self.x_power) + "," + str(self.y_power) + "," + str(self.z_power) + "," + str(self.clock_power) + "," + str(self.counterclock_power)
def main():
    th = Thruster("thruster")
    th.power = 120
    print th.power
    
    