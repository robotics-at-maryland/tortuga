import SoftwareVehicle
import PropulsionController
import VehicleInterface
from random import Random

random = Random()

vehicle = SoftwareVehicle.Vehicle()
controller = PropulsionController.Controller(vehicle)
controller.start()
vehicle.operate()

while True:
    vehicle.propulsion_instruction.x_power = random.randint(-100,100)
    vehicle.propulsion_instruction.y_power = random.randint(-100,100)
    vehicle.propulsion_instruction.z_power = random.randint(-100,100)
    vehicle.propulsion_instruction.yaw_power = random.randint(-100,100)