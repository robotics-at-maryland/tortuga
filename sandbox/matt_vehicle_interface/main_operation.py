import vehicle,model,state_machine,timer

import time

clock = timer.timer()
init_time = clock.init_time
vehicle = vehicle.main_vehicle()
environment = model.model(clock,vehicle)
machine = state_machine.state_machine(environment)
state = "preinitialized"

while not state == "finished":
    state = machine.operate()
    

