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

event.add_event_types(['THRUST_FORE', 'THRUST_BACK',
                       'THRUST_LEFT', 'THRUST_RIGHT','THRUST_PITCH_DOWN',
                       'THRUST_UP', 'THRUST_DOWN','THRUST_PITCH_UP',
                       'THRUST_KILL'])

class TestDepthController(object):
    def __init__(self, robot):
        self.robot = robot
        self._desired_depth = 0;
        self._desired_pitch = 0;
        
        watched_buttons = {'_forward' : ['THRUST_FORE'],
                           '_backward' : ['THRUST_BACK'],
                           '_left' : ['THRUST_LEFT'],
                           '_right' : ['THRUST_RIGHT'],
                           '_up' : ['THRUST_UP'],
                           '_down' : ['THRUST_DOWN'],
                           '_pitch_up' : ['THRUST_PITCH_UP'],
                           '_pitch_down' : ['THRUST_PITCH_DOWN']}
        self.key_observer = sim.input.ButtonStateObserver(self, watched_buttons)
        
    def update(self, time_since_last_frame):
        if self._up:
            self._desired_depth += 3 * time_since_last_frame
            print self._desired_depth
        elif self._down:
            self._desired_depth -= 3 * time_since_last_frame
            print self._desired_depth
        
        
        # Simple P type controller for the depth
        current_depth = self.robot._main_part._node.position.y
        error = self._desired_depth - current_depth
        force = error * 100
        
        self.robot.parts.aft_thruster.force = force 
        self.robot.parts.front_thruster.force = force
        
#        actual_force = self.robot.parts.front_thruster.force
#        print 'E: %5.4f D: %5.4f A: %5.4f FC: %06.4f FA: %06.4f' % \
#            (error, self._desired_depth, current_depth, force, actual_force)
            
        # Simple P type controller for the pitch
        current_pitch = self.robot._main_part.orientation.getPitch().valueDegrees()
        error = self._desired_pitch - current_pitch
        
        fbefore = self.robot.parts.front_thruster.force
        abefore = self.robot.parts.aft_thruster.force
        
        correction = (error * 0.005)
        self.robot.parts.front_thruster.force *= 1 + (error * 0.01)
        self.robot.parts.aft_thruster.force *= 1 - (error * 0.01)
        
        fafter = self.robot.parts.front_thruster.force
        aafter = self.robot.parts.aft_thruster.force
        
        print 'E: %5.4f D: %5.4f A: %5.4f C: %6.5f | FB: %06.4f FA: %06.4f AB: %06.4f AA: %06.4f' % \
            (error, self._desired_pitch, current_pitch, correction, fbefore, fafter, abefore, aafter)
            
        if self._forward:
            self.robot.parts.right_thruster.force += (7.0 * time_since_last_frame)
            self.robot.parts.left_thruster.force += (7.0 * time_since_last_frame)
        if self._backward:
            self.robot.parts.right_thruster.force -= (7.0 * time_since_last_frame)
            self.robot.parts.left_thruster.force -= (7.0 * time_since_last_frame)
        
        if self._left:
            self.robot.parts.left_thruster.force -= (5 * time_since_last_frame)
            self.robot.parts.right_thruster.force += (5 * time_since_last_frame)
        if self._right:
            self.robot.parts.left_thruster.force += (5 * time_since_last_frame)
            self.robot.parts.right_thruster.force -= (5 * time_since_last_frame)
            
        if self._pitch_up:
            self.robot.parts.aft_thruster.force -= (5 * time_since_last_frame)
            self.robot.parts.front_thruster.force += (5 * time_since_last_frame)
        if self._pitch_down:
            self.robot.parts.aft_thruster.force += (5 * time_since_last_frame)
            self.robot.parts.front_thruster.force -= (5 * time_since_last_frame)

class TestController(object):
    def __init__(self, robot):
        self.robot = robot
    
        event.register_handlers('THRUST_KILL',self._thrust_kill)
    
        watched_buttons = {'_forward' : ['THRUST_FORE'],
                           '_backward' : ['THRUST_BACK'],
                           '_left' : ['THRUST_LEFT'],
                           '_right' : ['THRUST_RIGHT'],
                           '_up' : ['THRUST_UP'],
                           '_down' : ['THRUST_DOWN'],
                           '_pitch_up' : ['THRUST_PITCH_UP'],
                           '_pitch_down' : ['THRUST_PITCH_DOWN']}

        self.key_observer = sim.input.ButtonStateObserver(self, watched_buttons)
    
    def _thrust_kill(self, key, down, mod_keys):
        if down:
            self.robot.parts.front_thruster.force = 0
            self.robot.parts.aft_thruster.force = 0
            self.robot.parts.right_thruster.force = 0
            self.robot.parts.left_thruster.force = 0
    
    def update(self, time_since_last_frame):
        if self._forward:
            self.robot.parts.right_thruster.force += (7.0 * time_since_last_frame)
            self.robot.parts.left_thruster.force += (7.0 * time_since_last_frame)
        if self._backward:
            self.robot.parts.right_thruster.force -= (7.0 * time_since_last_frame)
            self.robot.parts.left_thruster.force -= (7.0 * time_since_last_frame)
        
        if self._left:
            self.robot.parts.left_thruster.force -= (5 * time_since_last_frame)
            self.robot.parts.right_thruster.force += (5 * time_since_last_frame)
        if self._right:
            self.robot.parts.left_thruster.force += (5 * time_since_last_frame)
            self.robot.parts.right_thruster.force -= (5 * time_since_last_frame)
            
        if self._up:
            self.robot.parts.aft_thruster.force += (7.0 * time_since_last_frame)
            self.robot.parts.front_thruster.force += (7.0 * time_since_last_frame)
        if self._down:
            self.robot.parts.aft_thruster.force -= (7.0 * time_since_last_frame)
            self.robot.parts.front_thruster.force -= (7.0 * time_since_last_frame)
            
        if self._pitch_up:
            self.robot.parts.aft_thruster.force -= (5 * time_since_last_frame)
            self.robot.parts.front_thruster.force += (5 * time_since_last_frame)
        if self._pitch_down:
            self.robot.parts.aft_thruster.force += (5 * time_since_last_frame)
            self.robot.parts.front_thruster.force -= (5 * time_since_last_frame)

class SimApp(wx.App):
    def OnInit(self):
        frame = wx.Frame(None, -1, "AUV Sim", wx.DefaultPosition, 
                         wx.Size(600,600))
        
        self.config = yaml.load(file(os.path.join('..', 'sim.yml')))
        
        self.first = False
        
        # Start up simulation
        self.ogre = wx.Panel(frame)
        self._init_simulation()
        # Create our windows
        self.ogre = gui.wxogre.wxOgre(None, frame, -1, wx.DefaultPosition, 
                                      wx.Size(600,600))
        
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
        self.timer.Start(1000 / 200)
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
                                           (KC.I,0) : 'THRUST_FORE',
                                           (KC.K,0) : 'THRUST_BACK',
                                           (KC.G,0) : 'THRUST_KILL',
                                           (KC.J,0) : 'THRUST_LEFT',
                                           (KC.L,0) : 'THRUST_RIGHT',
                                           (KC.U,0) : 'THRUST_DOWN',
                                           (KC.O,0) : 'THRUST_UP',
                                           (KC.Y,0) : 'THRUST_PITCH_UP',
                                           (KC.H,0) : 'THRUST_PITCH_DOWN'})
        
        self._test_controller = TestDepthController(self.sim._scenes['Main']._robots['AUT'])
    
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