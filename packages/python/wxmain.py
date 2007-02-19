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

import sim.simulation
import gui.wxogre

class SimApp(wx.App):
    def OnInit(self):
        frame = wx.Frame(None, -1, "AUV Sim")
        
        config = yaml.load(file(os.path.join('..', 'sim.yml')))
        
        # Start up simulation
        self._init_simulation()
        # Create our windows
        ogre = gui.wxogre.wxOgre(None, frame)
        
        frame.Show(True)
        self.SetTopWindow(frame)
        
        # Create out scene
        self.sim.create_scene('Main', config['Scenes']['current'],
                              config['Scenes']['path'])
        
        ogre.camera = self.sim.cameras['Main']
        return True
    
    def _init_simulation(self):
        # Read in value from config file and create the right vehicle
        #config = yaml.load(file(os.path.join('..', 'sim.yml')))
        #logloader.load_loggers(config["Logging"])
        
        #vehicle_type = config['vehicle']
        # Pass along the subsection of the config corresponding to the vehicle
        #vehicle = VehicleFactory.create(vehicle_type,
        #                                config['Vehicles'][vehicle_type])
    
        self.sim = sim.simulation.Simulation({})
        #self.components = [vehicle]
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
            
def main():            
    app = SimApp(0)
    app.MainLoop()

if __name__ == '__main__':
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)