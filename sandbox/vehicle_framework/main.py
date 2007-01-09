# Python Imports
import sys
import time
import yaml

# Project Imports
from vehicle import *

def main():
    # Parse Command line options

    # Read in value from config file and create the right vehicle    
    config = yaml.load(file('sim.yml'))

    vehicle_type = config['vehicle']
    # Pass along the subsection of the config corresponding to the vehicle
    vehicle = VehicleFactory.create(vehicle_type,
                                    config['Vehicles'][vehicle_type])

    # Start up our other systems(possibly use component interface to find them?)
    components = [vehicle]

    # Main Loop
    last_time = time.clock()
    time_since_last_iteration = 0
    run = True
    
    while (run):
        # Loop over all components updating them, if one returns false exit
        print 'Vehicle Update time slice: %f' % time_since_last_iteration
        for component in components:
            if not component.update(time_since_last_iteration):
                print 'On Main Comp %s, quitting' % str(component)
                run = False
        
        current_time = time.clock()
        time_since_last_iteration = current_time - last_time;
        last_time = current_time
    
    del vehicle
    print "Hello World"
    

    return 0


if __name__ == '__main__':
    sys.exit(main())