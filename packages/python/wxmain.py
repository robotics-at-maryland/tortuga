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
import gui.input


import event
import sim.input
event.add_event_types(['FORE_THRUST_UP', 'FORE_THRUST_DOWN',
                       'LEFT_THRUST_UP', 'LEFT_THRUST_DOWN',
                       'RIGHT_THRUST_UP', 'RIGHT_THRUST_DOWN',
                       'BACK_THRUST_UP', 'BACK_THRUST_DOWN'])

class TestController(object):
    def __init__(self, robot):
        self.robot = robot
    
        # This sets up automatic setting of the key down properties
        watched_buttons = {'_fore_up' : ['FORE_THRUST_UP'],
                           '_fore_down' : ['FORE_THRUST_DOWN'],
                           '_left_up' : ['LEFT_THRUST_UP'],
                           '_left_down' : ['LEFT_THRUST_DOWN'],
                           '_right_up' : ['RIGHT_THRUST_UP'],
                           '_right_down' : ['RIGHT_THRUST_DOWN'],
                           '_back_up' : ['BACK_THRUST_UP'],
                           '_back_down' : ['BACK_THRUST_DOWN']}

        self.key_observer = sim.input.ButtonStateObserver(self, watched_buttons)
    
    def update(self, time_since_last_frame):
        if self._fore_up:
            self.robot.parts.front_thruster.force += 10.0 * time_since_last_frame
        if self._fore_down:
            self.robot.parts.front_thruster.force -= 10.0 * time_since_last_frame
            
        if self._back_up:
            self.robot.parts.aft_thruster.force += 10.0 * time_since_last_frame
        if self._back_down:
            self.robot.parts.aft_thruster.force -= 10.0 * time_since_last_frame
        
        self.robot.parts.right_thruster.force = 30
        self.robot.parts.left_thruster.force = 30
#        if self._right_up:
#            self.robot.parts.right_thruster.force += 10.0 * time_since_last_frame
#        if self._right_down:
#            self.robot.parts.right_thruster.force -= 10.0 * time_since_last_frame
#            
#        if self._left_up:
#            self.robot.parts.left_thruster.force += 10.0 * time_since_last_frame
#        if self._left_down:
#            self.robot.parts.left_thruster.force -= 10.0 * time_since_last_frame

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
        self.ogre.camera = scene.get_camera('Main').camera
        
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
        
        self.setup_input()
        
        # Show the main frame
        frame.Show(True)
        self.SetTopWindow(frame)
        
        self.last_time = 0
        
        return True
    
    def setup_input(self):
        self._input_forwarder = gui.input.InputForwarder(self.sim.input_system)
        self._input_forwarder.forward_window(self.ogre)
        
        from sim.input import KC
        self.sim.input_system.map_actions({(KC.W,0) : 'CAM_FORWARD',
                                           (KC.A,0) : 'CAM_LEFT', 
                                           (KC.S,0) : 'CAM_BACK', 
                                           (KC.D,0) :'CAM_RIGHT',
                                           (KC.Q,0) : 'CAM_UP', 
                                           (KC.E,0) :'CAM_DOWN',
                                           (KC.F,0) : 'CAM_TOGGLE_FOLLOW',
                                           (KC.U,0) : 'FORE_THRUST_UP',
                                           (KC.J,0) : 'FORE_THRUST_DOWN',
                                           (KC.Y,0) : 'LEFT_THRUST_UP',
                                           (KC.H,0) : 'LEFT_THRUST_DOWN',
                                           (KC.O,0) : 'RIGHT_THRUST_UP',
                                           (KC.L,0) : 'RIGHT_THRUST_DOWN',
                                           (KC.I,0) : 'BACK_THRUST_UP',
                                           (KC.K,0) : 'BACK_THRUST_DOWN'})
        
        self._test_controller = TestController(self.sim._scenes['Main']._robots['AUT'])
    
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

        # Loop over all components updating them, if one returns false exit
        for component in self.components:
            component.update(time_since_last_iteration)
        
        self._test_controller.update(time_since_last_iteration)
        event.process_events()
        
        self.last_time = current_time
        
    def on_close(self, close_event):
        OgreNewt.Debugger.getSingleton().deInit()
        self.sim.delete()
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