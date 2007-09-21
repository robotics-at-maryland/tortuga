from numpy import *
from numpy.linalg import *

class SonarLocator:
    def __init__(self, sensor_positions):
        self.sensor_positions = array(sensor_positions)
        self.num_sensors = len(sensor_positions)
        self.sensorDistancesSquared = sum((sensor_positions - sensor_positions[0])**2,1)
        self.lhsmat = zeros((self.num_sensors,4))
        self.lhsmat[:,0:3] = self.sensor_positions
        self.speed_of_sound = 1497.0 # meters per second
        self.rhsmat = zeros((self.num_sensors,1))
    
    def getSourcePoint(self,arrival_times):
        distance_differences = (array(arrival_times) - arrival_times[0]) * self.speed_of_sound
        self.lhsmat[:,3] = distance_differences
        self.rhsmat = 0.5 * (self.sensorDistancesSquared - distance_differences**2)
        return lstsq(self.lhsmat,self.rhsmat)

    def getArrivalTimes(self,source_point):
        return sqrt(sum((self.sensor_positions-source_point)**2,1)) / self.speed_of_sound

unitTetrahedron = zeros((4,3))
unitTetrahedron[0] = [0,0,0]
unitTetrahedron[1] = [0,sqrt(3)/3,sqrt(2.0/3)]
unitTetrahedron[2] = [0.5,-sqrt(3)/6,sqrt(2.0/3)]
unitTetrahedron[3] = [-0.5,-sqrt(3)/6,sqrt(2.0/3)]

unitDoubleTetrahedron = resize(unitTetrahedron,(5,3))
unitDoubleTetrahedron[4] = [0,0,2*sqrt(2.0/3)]

hydrophones = unitTetrahedron
s = SonarLocator(hydrophones)
source_point = random.random(3)
signal_frequency = 30000 # hertz
signal_wavelength = s.speed_of_sound / signal_frequency

print "Hydrophone locations:"
print hydrophones

print "Speed of sound:"
print s.speed_of_sound
print "Wavelength for 30 kHz signal:"
print s.speed_of_sound / 30000

print ""

print "Source Point:"
print source_point
arrival_times = s.getArrivalTimes(source_point)
print "Arrival Times:"
print arrival_times
print "Localization Results:"
x, resid, rank, sing = s.getSourcePoint(arrival_times)
print x


print ""

print "Adding noise to arrival times"
arrival_times = arrival_times + (random.random(shape(arrival_times)) - 0.5) * 2 / 2 * (1.0/30000/2)
print arrival_times
print "Localization Results:"
x, resid, rank, sing = s.getSourcePoint(arrival_times)
print x

