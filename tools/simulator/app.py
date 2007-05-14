# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:   tools/simulator/app.py

# Python Imports
import time
import sys
import os.path

# Library Imports
import wx
import yaml
import ogre.renderer.OGRE as Ogre

# Project Imports
import event
import frame
from module import ModuleManager
from baseapp import AppBase
from sim.simulation import Simulation
            
class Application(wx.App, AppBase):
    
    def OnInit(self):
        self.update_interval = (1.0 / 30.0 * 1000) # In milliseconds
        
        # Load main application
        AppBase.__init__(self, os.path.join('data', 'config','sim.yml'), False)
        
        # We can not create any Ogre windows (or start the sim) until the main
        # frame has been created
        self._frame = frame.MainFrame(self._config)
        self._frame.Show(True)
        self.SetTopWindow(self._frame)
        
        # Load all the modules
        ModuleManager.get().load_modules(self._config)
        
        # Setup Update timer
        self.timer = wx.Timer()
        self.Bind(wx.EVT_TIMER, self.on_timer, self.timer)
        
        self.create_scenes()
        
        return True
        
    def OnExit(self):
        self.timer.Stop()
        
        # Remove all references to the simulation
        ModuleManager.get().unregister(Simulation.get())
        
        return 0
        
    def create_scenes(self):
        sim = Simulation.get()
        
        # This should be done internall by the simulation
        sim.create_all_scenes()
        
        # Setup the camera (Still a Hack, need to fix this)
        scene = sim.get_scene('Main')
        #OgreNewt.Debugger.getSingleton().init(scene.scene_mgr)
        children = self._frame.GetChildren()
        children[2].camera = scene.get_camera('Main').camera
        
        #self._test_controller = TestDepthController(scene._robots['AUT'])
        
        self.last_time = self._get_time()
        self.timer.Start(self.update_interval, True)
        
    def _get_time(self):
        if wx.Platform == '__WXGTK__':
            return time.time()
        else:
            return time.clock()
        

    def on_timer(self, timer_event): 
        current_time = self._get_time()
        time_since_last_iteration = (current_time - self.last_time);

        Ogre.Root.getSingleton().renderOneFrame()
        #self.frame.ogre._update()
        #OgreNewt.Debugger.getSingleton().showLines(self.sim.get_scene('Main').world)
            
        # Loop over all components updating them, if one returns false exit
        for mod in ModuleManager.get().itermodules():
            mod.update(time_since_last_iteration)
        
        #self._test_controller.update(time_since_last_iteration)
        event.process_events()
        
        self.last_time = current_time
        
        update_time = self.update_interval - (self._get_time() - current_time)
        if update_time < 0:
            update_time = self.update_interval
        self.timer.Start(update_time, True)
        
def main():            
    app = Application(0)
    app.MainLoop()

if __name__ == '__main__':
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)