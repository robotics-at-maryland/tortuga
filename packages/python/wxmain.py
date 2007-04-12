# Stdlib Imports
import os
import sys
import time

# Library Imports
import wx
import yaml
import ogre.renderer.OGRE as Ogre
import ogre.physics.OgreNewt as OgreNewt

# Project Imports
import logloader
import event

import sim.simulation
import sim.robot
import gui.wxogre

class SimApp(wx.App):
    def OnInit(self):
        frame = wx.Frame(None, -1, "AUV Sim", wx.DefaultPosition, 
                         wx.Size(400,400))
        
        self.config = yaml.load(file(os.path.join('..', 'sim.yml')))
        
        self.first = False
        
        # Start up simulation
        self.ogre = wx.Panel(frame)
        self._init_simulation()
        # Create our windows
        self.ogre = gui.wxogre.wxOgre(None, frame, -1, wx.DefaultPosition, 
                                      wx.Size(400,400))
        
        # Create out scene
        self.sim.create_scene('Main', self.config['Scenes']['current'],
                              self.config['Scenes']['path'])
        
        # Setup the camera
        scene = self.sim._scenes['Main']
        OgreNewt.Debugger.getSingleton().init(scene.scene_mgr)
        self.ogre.camera = scene.cameras['Main']
        
        scene._robots['Main'] = \
            sim.robot.Robot(scene, os.path.join('..','data','robots','aut.rml'))
        
        #Ogre.Root.getSingleton().renderOneFrame()
        
        # Setup Update timer
        self.timer = wx.Timer()
        self.timer.Start(30)
        self.Bind(wx.EVT_TIMER, self.on_timer, self.timer)
        frame.Bind(wx.EVT_CLOSE, self.on_close)
        
        # Setup Layout on the form
        sizer_1 = wx.BoxSizer(wx.VERTICAL) 
        sizer_1.Add(self.ogre, 2, wx.EXPAND, 0)  
        frame.SetAutoLayout(True) 
        frame.SetSizer(sizer_1) 
        #sizer_1.Fit(frame) 
        #sizer_1.SetSizeHints(frame) 
        frame.Layout() 
        
        # Show the main frame
        frame.Show(True)
        self.SetTopWindow(frame)
        
        self.last_time = 0
        
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
        
        self.components = [self.sim]
        #self.components = []
    def on_timer(self, timer_event):
        self.ogre._update()
        OgreNewt.Debugger.getSingleton().showLines(self.sim._scenes['Main'].world)
        
        current_time = time.clock()
        time_since_last_iteration = current_time - self.last_time;
        #print time_since_last_iteration * 1000
        # Loop over all components updating them, if one returns false exit
        for component in self.components:
            component.update(time_since_last_iteration)
        
        event.process_events()
        
        self.last_time = current_time
        
    def on_close(self, close_event):
        OgreNewt.Debugger.getSingleton().deInit()
        del self.sim
        del self.components
        
        self.timer.Stop()
        close_event.GetEventObject().Destroy()
            
def main():            
    app = SimApp(0)
    app.MainLoop()

if __name__ == '__main__':
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)