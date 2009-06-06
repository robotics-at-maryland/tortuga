Simulation Startup Notes

When using wxWidgets and Ogre toghether (as we do with the simulation) there are
several subtilities that must be taken into account.  The start up order is as 
follows:
  * Create wx.App object
  * Create Simulation object, but do not load the scenes, this starts up Ogre
    all the way to the point of creating the render window
  * Create your main wxFrame
  * Create a wxOgre window, this internally creates an Ogre render window
  * Wait for the wxActivate on the Main wxFrame event to load anything, when 
    this happens you can tell the simulation to load all the scenes
    
