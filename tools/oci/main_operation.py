'''
The entry point for the robot operation. This module creates all the controllers and models that
are necessary for robot operation, and then starts the state machine.
'''

import vehicle,model,state_machine,timer,vision_communicator,paths,motor_communicator
import sensor_comm
import time

#starts a clock for the robot. clock.time() returns seconds since this timer started
clock = timer.timer()
init_time = clock.init_time
#starts the module that communicates with the C vision libraries
vision_communicator = vision_communicator.vision_comm(paths.vision_lib)
#start the motor controller interface, the window to the thrusters
motor_controller = motor_communicator.communicator()
#start the sensor_communicator interface for communicating with steves board
sensor_communicator = sensor_comm.communicator()
#create the vehicle abstraction, passing in all the relevant controllers
vehicle = vehicle.main_vehicle(vision_communicator,motor_controller,sensor_communicator)
#the environment object is a way to pass a vehicle and the clock to the state machine, very simple warpping
environment = model.model(clock,vehicle)
#initialize the state machine, passing the environment model to it
machine = state_machine.state_machine(environment)

#set the initial state so the loop gets started without any problem
state = "preinitialized"

#this is the state machine loop. The state machine runs a function, then returns the next state
#(the next state could of course be the same as the current state). When the state gets changed
#to "finished" vehicle operation is complete, and the program terminates
while not state == "finished":
    state = machine.operate()
    

