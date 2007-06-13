import vehicle,model,state_machine,timer,vision_communicator,paths,motor_communicator
import sensor_comm
import time

clock = timer.timer()
init_time = clock.init_time
vision_communicator = vision_communicator.vision_comm(paths.vision_lib)
motor_controller = motor_communicator.communicator()
sensor_communicator = sensor_comm.communicator()
vehicle = vehicle.main_vehicle(vision_communicator,motor_controller,sensor_communicator)
environment = model.model(clock,vehicle)
machine = state_machine.state_machine(environment)
state = "preinitialized"

while not state == "finished":
    state = machine.operate()
    

