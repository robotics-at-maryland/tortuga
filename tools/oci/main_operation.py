'''
The entry point for the robot operation. This module creates all the controllers and models that
are necessary for robot operation, and then starts the state machine.
'''

#import os
#import ai.state_machine as state_machine
#import vision.vision_communicator as vision_communicator
#import vehicle.motor_communicator as motor_communicator
#import vehicle.sensor_comm as sensor_comm

#vision_library = os.environ["RAM_SVN_DIR"] + "/build/lib/libram_vision"
#motor_controller = "/dev/USB0"
#sensor_communicator = "/dev/sensor"

#starts the module that communicates with the C vision libraries
#vision_communicator = vision_communicator.vision_comm(vision_library)
#start the motor controller interface, the window to the thrusters
#motor_controller = motor_communicator.communicator(motor_controller)
#start the sensor_communicator interface for communicating with steves board
#sensor_communicator = sensor_comm.communicator(sensor_communicator)
#create the vehicle abstraction, passing in all the relevant controllers
#vehicle = vehicle.main_vehicle(vision_communicator,motor_controller,sensor_communicator)
#initialize the state machine, passing the environment model to it
#machine = state_machine.state_machine(vehicle)

#set the initial state so the loop gets started without any problem
#state = "preinitialized"

#this is the state machine loop. The state machine runs a function, then returns the next state
#(the next state could of course be the same as the current state). When the state gets changed
#to "finished" vehicle operation is complete, and the program terminates
#while not state == "finished":
#    state = machine.operate()
    
# Python Imports
import os
import sys
import time
    
# Library Imports
import yaml

# Project Imports
import ociapp

def main():            
    app = ociapp.OCIApp('config.yml')
    app._modules[1].background(20)
    while(1):
        pass

if __name__ == '__main__':
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)