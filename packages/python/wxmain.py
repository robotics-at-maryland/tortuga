# Stdlib Imports
import os
import sys
import time

# Library Imports
import wx
import yaml

# Project Imports
import logloader
import event

import sim
from vehicle.sim.simvehicle import Vehicle, VehicleFactory

class SimApp(wx.App):
    def OnInit(self):
        frame = wx.Frame(None, -1, "Hello wxPython world")
        
        self._init_simulation()
        
        frame.Show(True)
        self.SetTopWindow(frame)
        return True
    
    def _init_simulation(self):
        # Read in value from config file and create the right vehicle
        config = yaml.load(file(os.path.join('..', 'sim.yml')))
        logloader.load_loggers(config["Logging"])
        
        vehicle_type = config['vehicle']
        # Pass along the subsection of the config corresponding to the vehicle
        vehicle = VehicleFactory.create(vehicle_type,
                                        config['Vehicles'][vehicle_type])
    
        self.components = [vehicle]
    def on_timer(self):
        last_time = time.clock()
        time_since_last_iteration = 0
        run = True
        
        while (run):
            # Loop over all components updating them, if one returns false exit
            for component in self.components:
                if not component.update(time_since_last_iteration):
                    run = False
            
            event.process_events()
            
            current_time = time.clock()
            time_since_last_iteration = current_time - last_time;
            last_time = current_time
            
            
app = SimApp(0)
app.MainLoop()