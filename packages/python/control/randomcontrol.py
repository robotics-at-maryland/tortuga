import random
import math
class RandomVehicleController(object):
    
    def __init__(self, vehicle):
        self.vehicle = vehicle
        print dir(random)
        self.random = random.Random()
        
    def update(self, time):
        #vehicle.propulsion_instruction.x_power = random.randint(-100,100)
        #vehicle.propulsion_instruction.y_power = random.randint(-100,100)
        #vehicle.propulsion_instruction.z_power = random.randint(-100,100)
        #vehicle.propulsion_instruction.yaw_power = random.randint(-100,100)
         
        
        #self.vehicle.PortThruster.power += random.random() * random.randint(-1,1)
        self.vehicle.PortThruster.power = math.sin(time)
        #self.vehicle.AftThruster.power += random.random() * random.randint(-1,1)
        #self.vehicle.StarboardThruster.power += random.random() * random.randint(-1,1)
        #self.vehicle.ForeThruster.power += random.random() * random.randint(-1,1)
        return True