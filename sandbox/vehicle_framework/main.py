# Python Imports
import sys
import yaml



# Project Imports
from vehicle import *

def main():
    # Parse Command line options

    # Read in value from config file and create the right vehicle    
    config = yaml.load(file('sim.cfg'))

    vehicle_type = config['vehicle']
    # Pass along the subsection of the config corresponding to the vehicle
    vehicle = VehicleFactory.create(vehicle_type,
                                    config['Vehicles'][vehicle_type])

    # Start up our other systems(possibly use component interface to find them?)

    # Start up our vehicle
    vehicle.start_update();
    print "Hello World"

    return 0


if __name__ == '__main__':
    sys.exit(main())