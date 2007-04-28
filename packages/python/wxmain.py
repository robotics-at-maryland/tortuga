# Stdlib Imports
import os
import sys
import time
import warnings

# Library Imports
import wx
import yaml

warnings.simplefilter('ignore', RuntimeWarning)
import ogre.renderer.OGRE as Ogre
import ogre.physics.OgreNewt as OgreNewt
import ogre.io.OIS
warnings.simplefilter('default', RuntimeWarning)

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
        force = error * 10 - self.robot._main_part.acceleration.y * 10
        
        self.robot.parts.aft_thruster.force = force 
        self.robot.parts.front_thruster.force = force
        
        actual_force = self.robot.parts.front_thruster.force
#        print 'E: %5.4f A: %5.4f D: %5.4f AD: %5.4f FC: %06.4f FA: %06.4f' % \
#            (error, self.robot._main_part.acceleration.y, self._desired_depth, 
#             current_depth, force, actual_force)
        
#        print 'E: %5.4f A: %5.4f' % (error, self.robot._main_part.acceleration.y)
        # Simple P type controller for the pitch
        current_pitch = self.robot._main_part.orientation.getPitch().valueDegrees()
        error = self._desired_pitch - current_pitch
        
        fbefore = self.robot.parts.front_thruster.force
        abefore = self.robot.parts.aft_thruster.force
        
        force = (error * 5)
        self.robot.parts.front_thruster.force -= force
        self.robot.parts.aft_thruster.force += force
        
        fafter = self.robot.parts.front_thruster.force
        aafter = self.robot.parts.aft_thruster.force
        
#        print 'E: %5.4f D: %5.4f A: %5.4f C: %6.5f | FB: %06.4f FA: %06.4f AB: %06.4f AA: %06.4f' % \
#            (error, self._desired_pitch, current_pitch, 0, fbefore, fafter, abefore, aafter)
            
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

class MainFrame(gui.wxogre.wxOgreFrame):
    def __init__(self, activate_callback):
        self._activate_callback = activate_callback
        gui.wxogre.wxOgreFrame.__init__(self, None, -1, "AUV Sim", 
                                        wx.DefaultPosition, wx.Size(600,600))
        
        self.ogre = gui.wxogre.wxOgre(None, self, -1, wx.DefaultPosition, 
                                      wx.Size(600,600))
        
        if wx.Platform == '__WXMSW__':
            self._input_forwarder = gui.input.InputForwarder(\
                 sim.simulation.Simulation.get().input_system)
            self._input_forwarder.forward_window(self.ogre)
        elif wx.Platform == '__WXGTK__':
            # Handled in on_activate
            pass
        else:
            raise "Error platform not supported"
        
        # Setup Layout
        sizer_1 = wx.BoxSizer(wx.VERTICAL) 
        sizer_1.Add(self.ogre, 2, wx.EXPAND, 0)  
        self.SetAutoLayout(True) 
        self.SetSizer(sizer_1) 
        self.Layout() 
        
    def on_activate(self, event):
        gui.wxogre.wxOgreFrame.on_activate(self, event)
        self.ogre._init_ogre()
        if wx.Platform == '__WXGTK__':
            self._input_forwarder = sim.input.OISInputForwarder({}, \
                 sim.simulation.Simulation.get().input_system,
                 self.ogre._render_window)
        
        self._activate_callback()
        

class SimApp(wx.App):
    def OnInit(self):
        self.last_time = 0
        self.frame = None
        self.sim = None
        self.components = []
        self.timer = None

        self.config = yaml.load(file(os.path.join('..', 'sim.yml')))
        # Create simulation
        self.sim = sim.simulation.Simulation(self.config['Simulation'])
        self.components = [self.sim]
        
        # Create our main frame
        self.frame = MainFrame(self.create_scenes)

        # Setup Update timer
        self.timer = wx.Timer()
        self.Bind(wx.EVT_TIMER, self.on_timer, self.timer)

        # Show the main frame
        self.frame.Show(True)
        self.SetTopWindow(self.frame)
        self.frame.Bind(wx.EVT_CLOSE, self.on_close)

        return True
    
    def _get_time(self):
        if wx.Platform == '__WXGTK__':
            return time.time()
        else:
            return time.clock()
    
    def create_scenes(self):
        self.sim.create_all_scenes()
        
        # Setup the camera (Still a Hack, need to fix this)
        scene = self.sim.get_scene('Main')
        OgreNewt.Debugger.getSingleton().init(scene.scene_mgr)
        self.frame.ogre.camera = scene.get_camera('Main').camera
        
        self._test_controller = TestDepthController(scene._robots['AUT'])
        
        self.last_time = self._get_time()
        self.timer.Start(1000.0 / 200.0)
    
    def on_timer(self, timer_event):
        self.frame.ogre._update()
        #OgreNewt.Debugger.getSingleton().showLines(self.sim.get_scene('Main').world)
        
        current_time = self._get_time()
        time_since_last_iteration = (current_time - self.last_time);

        #print time_since_last_iteration
        if (time_since_last_iteration < 1000.0 / 300.0):
            #print 'RUN',time_since_last_iteration
            #print 'C: %f L: %f Update %f' % (current_time, self.last_time, time_since_last_iteration)
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