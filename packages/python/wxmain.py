# Stdlib Imports
import os
import sys
import time

# Library Imports
import wx
import yaml
import ogre.renderer.OGRE as Ogre

# Project Imports
import logloader
import event

import sim.simulation
import gui.wxogre

class SimApp(wx.App):
    def OnInit(self):
        frame = wx.Frame(None, -1, "AUV Sim")
        
        self.config = yaml.load(file(os.path.join('..', 'sim.yml')))
        
        self.first = False
        
        # Start up simulation
        self.ogre = wx.Panel(frame)
        self._init_simulation()
        # Create our windows
        self.ogre = gui.wxogre.wxOgre(None, frame)

        # Now that the window is here initialize resource groups
        #Ogre.ResourceGroupManager.getSingleton().initialiseAllResourceGroups()
        
        # Create out scene
        self.sim.create_scene('Main', self.config['Scenes']['current'],
                              self.config['Scenes']['path'])

        # Setup the camera
        self.ogre.camera = self.sim._scenes['Main'].cameras['Main']
        
        #Ogre.Root.getSingleton().renderOneFrame()
        
        # Setup Update timer
        #self.timer = wx.Timer()
        #self.timer.Start(100)
        #self.Bind(wx.EVT_TIMER, self.on_timer, self.timer)
        
        # Setup Layout on the form
        sizer_1 = wx.BoxSizer(wx.VERTICAL) 
        sizer_1.Add(self.ogre, 2, wx.EXPAND, 0)  
        frame.SetAutoLayout(True) 
        frame.SetSizer(sizer_1) 
        sizer_1.Fit(frame) 
        sizer_1.SetSizeHints(frame) 
        frame.Layout() 
        
        # Show the main frame
        frame.Show(True)
        self.SetTopWindow(frame)
        
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
        
#        self.sim._ogre_root = Ogre.Root('plugins.cfg')
#        
#        rmgr = Ogre.ResourceGroupManager.getSingleton()
#        rmgr.addResourceLocation("../media/packs/OgreCore.zip", "Zip",
#                                 "Bootstrap", False)
#        rmgr.addResourceLocation("../media/packs/cubemapsJS.zip", "Zip",
#                                 "Bootstrap", False)
#        rmgr.addResourceLocation("../media/models", "FileSystem",
#                                 "Bootstrap", False)
#        rmgr.addResourceLocation("../media/primitives", "FileSystem",
#                                 "Bootstrap", False)
#        rmgr.addResourceLocation("../media/materials/textures", "FileSystem",
#                                 "Bootstrap", False)
#        rmgr.addResourceLocation("../media/materials/scripts", "FileSystem",
#                                 "Bootstrap", False)
#        
#        carryOn = self.sim._ogre_root.showConfigDialog() 
#        if not carryOn: 
#            sys.exit('Quit from Config Dialog')
        
        # Now Lets create our window
#        size = self.ogre.GetSize()
#        renderParameters = Ogre.NameValuePairList() 
#        renderParameters['externalWindowHandle'] = str(self.ogre.GetHandle()) 
#        renderWindow = self.sim._ogre_root.createRenderWindow('wxPython render window', size[0], 
#                                               size[1], False, renderParameters) 
#        renderWindow.active = True 
#        self.renderWindow = renderWindow 
        
        self.components = [self.sim]
        #self.components = []
    def on_timer(self, timer_event):
        print 'Update'
        #Ogre.Root.getSingleton().renderOneFrame()
        
        last_time = time.clock()
        time_since_last_iteration = 0

        # Loop over all components updating them, if one returns false exit
        for component in self.components:
            component.update(time_since_last_iteration)
        
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