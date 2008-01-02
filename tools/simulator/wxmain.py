# Stdlib Imports
import os
import sys
import time
import warnings

# Library Imports
import wx
import wx.aui
import yaml

warnings.simplefilter('ignore', RuntimeWarning)
import ogre.renderer.OGRE as Ogre
import ogre.physics.OgreNewt as OgreNewt
import ogre.io.OIS
warnings.simplefilter('default', RuntimeWarning)

# Project Imports
import logloader
import event

import ram.sim.simulation
import sim.robot
import gui.wxogre
import gui.input
import ram.module as module
import competition

import event
import ram.sim.input

event.add_event_types(['THRUST_FORE', 'THRUST_BACK',
                       'THRUST_LEFT', 'THRUST_RIGHT','THRUST_PITCH_DOWN',
                       'THRUST_UP', 'THRUST_DOWN','THRUST_PITCH_UP',
                       'THRUST_KILL'])

class Vehicle(object):
    def __init__(self, robot):
        self.robot = robot
    
    def getDepth(self):
        return self.robot._main_part._node.position.y
    
    def getOrientation(self):
        return self.robot._main_part._node.orientation
    
    def acceleration(self):
        return self.robot._main_part.acceleration
    
    def angularAcceleration(self):
        return self.robot._main_part.angular_accel
    
#    Implement:
#    def getForce(self):
#        return 
    
    def applyForcesAndTorque(self, force, torque):
        star = force[2] / 2 + 0.5 * torque[1] #/ 0.1905
        port = force[2] / 2 - 0.5 * torque[1] #/ 0.1905
        fore = force[1] / 2 + 0.5 * torque[0] #/ 0.3366
        aft = force[1] / 2 - 0.5 * torque[0] #/ 0.3366
  
        #print 'Port;', port , ' Star;', star
          
#        hor_force_difference = 0
#        if star < port:
#            hor_force_difference = (port - star)
#            port += hor_force_difference * 4
#        elif star > port:
#            hor_force_difference = (star - port)
#            port -= hor_force_difference * 4
        
            
        self.robot.parts.right_thruster.force = star
        self.robot.parts.left_thruster.force = port
        self.robot.parts.front_thruster.force = fore
        self.robot.parts.aft_thruster.force = aft
        

#        self.robot.parts.right_thruster.force = 10
#        self.robot.parts.left_thruster.force = -10

class TestDepthController(object):
    def __init__(self, robot):
        #self.robot = robot
        self.vehicle = Vehicle(robot) #vehicle declaration
        self._desired_depth = 0;
        self._desired_pitch = 0;
        self._desired_yaw = 0;
        
        watched_buttons = {'_forward' : ['THRUST_FORE'],
                           '_backward' : ['THRUST_BACK'],
                           '_left' : ['THRUST_LEFT'],
                           '_right' : ['THRUST_RIGHT'],
                           '_up' : ['THRUST_UP'],
                           '_down' : ['THRUST_DOWN'],
                           '_pitch_up' : ['THRUST_PITCH_UP'],
                           '_pitch_down' : ['THRUST_PITCH_DOWN']}
        self.key_observer = ram.sim.input.ButtonStateObserver(self, watched_buttons)
        
    def update(self, time_since_last_frame):
        if self._up:
            self._desired_depth += 3 * time_since_last_frame
            print self._desired_depth
        elif self._down:
            self._desired_depth -= 3 * time_since_last_frame
        
        
        # Simple P type controller for the depth
        current_depth = self.vehicle.getDepth()
        error = self._desired_depth - current_depth
#        print 'Current Depth: ', current_depth, ' Desired: ', self._desired_depth, ' Error: ', error
        depth_force = error * 50 #- self.robot._main_part.acceleration.y * 10
        #print 'Depth Force',force
        
        force = Ogre.Vector3(0,depth_force,0)
        
        
        if self._left:
            self._desired_yaw += 5 * time_since_last_frame
        elif self._right:
            self._desired_yaw -= 5 * time_since_last_frame
        current_yaw = self.vehicle.getOrientation().getYaw().valueDegrees()
        yaw_error = self._desired_yaw - current_yaw
        print 'Desired', self._desired_yaw
        print 'Current', current_yaw
        
        
        current_pitch = self.vehicle.getOrientation().getPitch().valueDegrees()
        error = self._desired_pitch - current_pitch
        torque = Ogre.Vector3(error,yaw_error,0)
        
        
        if self._forward:
            force += Ogre.Vector3(0,0,10) 
        if self._backward:       
            force -= Ogre.Vector3(0,0,10)
#<<<<<<< .mine
#            self.robot.parts.right_thruster.force += (7.0 * time_since_last_frame)
#            self.robot.parts.left_thruster.force += (7.0 * time_since_last_frame)
#        if self._backward:
#            self.robot.parts.right_thruster.force -= (7.0 * time_since_last_frame)
#            self.robot.parts.left_thruster.force -= (7.0 * time_since_last_frame)
#
#        if self._left:
#            self.robot.parts.left_thruster.force -= (5 * time_since_last_frame)
#            self.robot.parts.right_thruster.force += (5 * time_since_last_frame)
#        if self._right:
#            self.robot.parts.left_thruster.force += (5 * time_since_last_frame)
#            self.robot.parts.right_thruster.force -= (5 * time_since_last_frame)
#>>>>>>> .r1688
#            
        if self._left:
            torque += Ogre.Vector3(0,5,0)
        if self._right:
            torque -= Ogre.Vector3(0,5,0)
        print force[1]
        self.vehicle.applyForcesAndTorque(force, torque)

        
#        print 'E: %5.4f A: %5.4f D: %5.4f AD: %5.4f FC: %06.4f FA: %06.4f' % \
#            (error, self.robot._main_part.acceleration.y, self._desired_depth, 
#             current_depth, force, actual_force)
        
#        print 'E: %5.4f A: %5.4f' % (error, self.robot._main_part.acceleration.y)
        # Simple P type controller for the pitch
#        current_pitch = self.vehicle.getOrientation().getPitch().valueDegrees()
#        error = self._desired_pitch - current_pitch
#        
#        fbefore = self.robot.parts.front_thruster.force
#        abefore = self.robot.parts.aft_thruster.force
#        
#        force = (error * 5)
#        self.robot.parts.front_thruster.force -= force
#        self.robot.parts.aft_thruster.force += force
#        
#        fafter = self.robot.parts.front_thruster.force
#        aafter = self.robot.parts.aft_thruster.force
#        
##        print 'E: %5.4f D: %5.4f A: %5.4f C: %6.5f | FB: %06.4f FA: %06.4f AB: %06.4f AA: %06.4f' % \
##            (error, self._desired_pitch, current_pitch, 0, fbefore, fafter, abefore, aafter)
#            
#        if self._forward:
#            self.robot.parts.right_thruster.force += (7.0 * time_since_last_frame)
#            self.robot.parts.left_thruster.force += (7.0 * time_since_last_frame)
#        if self._backward:
#            self.robot.parts.right_thruster.force -= (7.0 * time_since_last_frame)
#            self.robot.parts.left_thruster.force -= (7.0 * time_since_last_frame)
#        
#        if self._left:
#            self.robot.parts.left_thruster.force -= (5 * time_since_last_frame)
#            self.robot.parts.right_thruster.force += (5 * time_since_last_frame)
#        if self._right:
#            self.robot.parts.left_thruster.force += (5 * time_since_last_frame)
#            self.robot.parts.right_thruster.force -= (5 * time_since_last_frame)
#            
#        if self._pitch_up:
#            self.robot.parts.aft_thruster.force -= (5 * time_since_last_frame)
#            self.robot.parts.front_thruster.force += (5 * time_since_last_frame)
#        if self._pitch_down:
#            self.robot.parts.aft_thruster.force += (5 * time_since_last_frame)
#            self.robot.parts.front_thruster.force -= (5 * time_since_last_frame)

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

        self.key_observer = ram.sim.input.ButtonStateObserver(self, watched_buttons)
    
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


class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        event.register_handlers('MODULE_START',self.on_module_start)
        self.log = log
        wx.Panel.__init__(self, parent, -1, wx.DefaultPosition, wx.Size(100,400))

        self.nb = wx.aui.AuiNotebook(self)
        self.page = wx.TextCtrl(self.nb, -1, "Test", style=wx.TE_MULTILINE)
        self.nb.AddPage(self.page, "Welcome")

        sizer = wx.BoxSizer()
        sizer.Add(self.nb, 1, wx.EXPAND)
        self.SetSizer(sizer)
        
    def on_module_start(self, mod):
        self.page.write('\nModule: %s of type: "%s" started\n' % (mod.name, type(mod)))

class MainFrame(gui.wxogre.wxOgreFrame):
    def __init__(self, activate_callback):
        self._activate_callback = activate_callback       
        gui.wxogre.wxOgreFrame.__init__(self, None, -1, "AUV Sim", 
                                        wx.DefaultPosition, wx.Size(800,600))
        
        
        self._mgr = wx.aui.AuiManager()
        self._mgr.SetManagedWindow(self)
        
        self.ogre = gui.wxogre.wxOgre(None, self)#, -1, wx.DefaultPosition
                                      #wx.Size(400,400))
        
        self._mgr.AddPane(self.ogre, wx.aui.AuiPaneInfo().
                          Name("test1").Caption("AUV").Center().
                          CloseButton(True).MaximizeButton(True))
        self._mgr.AddPane(TestPanel(self, None), wx.aui.AuiPaneInfo().
                          Name("test2").Caption("Status").Right().
                          CloseButton(True).MaximizeButton(True))
        
        if wx.Platform == '__WXMSW__':
            self._input_forwarder = gui.input.InputForwarder(\
                 ram.sim.simulation.Simulation.get().input_system)
            self._input_forwarder.forward_window(self.ogre)
        elif wx.Platform == '__WXGTK__':
            # Handled in on_activate
            pass
        else:
            raise "Error platform not supported"
        
        self.SetMinSize(self.GetSize())
        self._mgr.Update()
        # Setup Layout
        #sizer_1 = wx.BoxSizer(wx.VERTICAL) 
        #sizer_1.Add(self.ogre, 2, wx.EXPAND, 0)  
        #self.SetAutoLayout(True) 
        #self.SetSizer(sizer_1) 
        #self.Layout() 
        

        
    def on_activate(self, event):
        gui.wxogre.wxOgreFrame.on_activate(self, event)
        if wx.Platform == '__WXGTK__':
            self._input_forwarder = ram.sim.input.OISInputForwarder({}, \
                 ram.sim.simulation.Simulation.get().input_system,
                 self.ogre._render_window)
        
        self._activate_callback()
        

class SimApp(wx.App):
    def OnInit(self):
        self.last_time = 0
        self.frame = None
        self.sim = None
        self.components = []
        self.timer = None
        self.update_interval = (1.0 / 30.0 * 1000) # In milliseconds

        self.config = yaml.load(file(os.path.join('data', 'config','sim.yml')))
        # Create simulation
        self.sim = ram.sim.simulation.Simulation(self.config['Modules']['Simulation'])
        self.components = [self.sim]
        
        # Create our main frame
        self.frame = MainFrame(self.create_scenes)

        # Setup Update timer
        self.sim.start()
        self.timer = wx.Timer()
        self.Bind(wx.EVT_TIMER, self.on_timer, self.timer)

        # Show the main frame
        self.frame.Show(True)
        self.SetTopWindow(self.frame)
        self.frame.Bind(wx.EVT_CLOSE, self.on_close)
        
        temp = competition.StartingGate()
        event.add_event_types(['GATE_ENTERED', 'GATE_EXITED'])
        event.register_handlers({'GATE_ENTERED' : temp._entered_gate,
                                 'GATE_EXITED' : temp._left_gate })

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
        self.timer.Start(self.update_interval, True)
    
    def on_timer(self, timer_event): 
        current_time = self._get_time()
        time_since_last_iteration = (current_time - self.last_time);

        self.frame.ogre._update()
        OgreNewt.Debugger.getSingleton().showLines(self.sim.get_scene('Main').world)
            
        # Loop over all components updating them, if one returns false exit
        for component in self.components:
            component.update(time_since_last_iteration)
        
        self._test_controller.update(time_since_last_iteration)
        event.process_events()
        
        self.last_time = current_time
        
        update_time = self.update_interval - (self._get_time() - current_time)
        if update_time < 0:
            update_time = self.update_interval
        self.timer.Start(update_time, True)
        
    def on_close(self, close_event):
        OgreNewt.Debugger.getSingleton().deInit()
        self.sim.delete()
        del self.sim
        del self.components
        
        self.timer.Stop()
        close_event.GetEventObject().Destroy()
            
def main():
    m = module.ModuleManager()            
    app = SimApp(0)
    app.MainLoop()

if __name__ == '__main__':
    try:
        sys.exit(main())
    except yaml.scanner.ScannerError, e:
        print str(e)