#Below is an example of the class in use 
        
if __name__=="__main__": 
    from OgreWindowWx import *
    import wx.py as py #This modules gives access to the pyCrust interpreter widget 
    
    class DemoOgreWindow(OgreWindow): 
        """This is a demo of how to subclass OgreWindow. 
        Many parts of the code are directely inspired from the beginner to 
        advance tutorials""" 
        
        fishIsSwiming=False #A variable to keep track whether the fish is swiming or not 
        
        def _PopulateScene(self):  
            "Creation of scene objects"        
            # create scene 
            sceneManager=self.sceneManager 
            sceneManager.AmbientLight = ogre.ColourValue(0.7, 0.7, 0.7 )
            sceneManager.setSkyDome(True, 'Examples/CloudySky', 4.0, 8.0) 
    
            light = sceneManager.createLight('MainLight') 
            light.setPosition  ( ogre.Vector3(20, 80, 130) )
            self.sceneEntities.MainLight = light 
           
            # add some fog 
            sceneManager.setFog(ogre.FOG_EXP, ogre.ColourValue.White, 0.0002) 
    
            plane = ogre.Plane() 
            plane.normal = ogre.Vector3(0, 1, 0) 
            plane.d = 200 
            ogre.MeshManager.getSingleton().createPlane('FloorPlane', "General",plane, 200000.0, 200000.0, 
                                                        20, 20, True, 1, 50.0, 50.0,ogre.Vector3(0, 0, 1),
                                                        ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY, 
                                                        ogre.HardwareBuffer.HBU_STATIC_WRITE_ONLY, 
                                                        True,True )
    
            # create floor entity 
            entity = sceneManager.createEntity('floor', 'FloorPlane') 
            entity.setMaterialName('Examples/RustySteel') 
            sceneManager.getRootSceneNode().createChildSceneNode().attachObject(entity) 
            self.sceneEntities.floor = entity 
            
    
            # create fish entity 
            fishNode = sceneManager.getRootSceneNode().createChildSceneNode("fishNode") 
            entity = sceneManager.createEntity('Wanda', 'fish.mesh') 
            fishNode.attachObject(entity) 
            fishNode.setScale(10.0,10.0,10.0) 
            
            self.sceneEntities.fishNode = fishNode 
            self.sceneEntities.fish = entity 
            self.swimAnimationState = entity.getAnimationState('swim') 
            self.swimAnimationState.Enabled = True 
            
        
             
            #create fish swiming path 
            fishAnimationNode = sceneManager.getRootSceneNode().createChildSceneNode("fishAnimationNode")
            self.sceneEntities.fishAnimationNode = fishAnimationNode
            
            animation = sceneManager.createAnimation('fishTrack', 10) 
            animation.interpolationMode = ogre.Animation.IM_SPLINE 
            animationTrack = animation.createNodeTrack(0, fishAnimationNode) 
            key = animationTrack.createNodeKeyFrame(0.0) 
            key = animationTrack.createNodeKeyFrame(1.0) 
            key.setTranslate (ogre.Vector3(80.0, 0.0, -40.0) )
            key = animationTrack.createNodeKeyFrame(2.0) 
            key.setTranslate ( ogre.Vector3(120.0, 0.0, 0.0)) 
            key = animationTrack.createNodeKeyFrame(3.0) 
            key.setTranslate(ogre.Vector3(80.0, 0.0, 80.0) )
            key = animationTrack.createNodeKeyFrame(5.0) 
            key.setTranslate ( ogre.Vector3(-80.0, 0.0, -40.0) )
            key = animationTrack.createNodeKeyFrame(6.0) 
            key.setTranslate ( ogre.Vector3(-120.0, 0.0, 0.0) )
            key = animationTrack.createNodeKeyFrame(7.0) 
            key.setTranslate ( ogre.Vector3(-80.0, 0.0, 100.0)) 
            key = animationTrack.createNodeKeyFrame(8.0) 
            key.setTranslate ( ogre.Vector3(30.0, 0.0, 90.0) )
            key = animationTrack.createNodeKeyFrame(9.0) 
            key.setTranslate( ogre.Vector3(-20.0, 0.0, 40.0) )
            key = animationTrack.createNodeKeyFrame(10.0) 
            key.setTranslate ( ogre.Vector3(0.0, 0.0, 0.0) )
            
            self.fishAnimationState = sceneManager.createAnimationState('fishTrack') 
            self.fishAnimationState.Enabled = True 
            
            #create a duplicate fish swiming path for determining where the fish has to look to 
            self.sceneEntities.fishLookAtNode = fishLookAtNode = sceneManager.getRootSceneNode().createChildSceneNode("fishLookAtNode") 
            animation = sceneManager.createAnimation('fishLookAtTrack', 10) 
            animation.interpolationMode = ogre.Animation.IM_SPLINE 
            animationTrack = animation.createNodeTrack(0, fishLookAtNode) 
            key = animationTrack.createNodeKeyFrame(0.0) 
            key = animationTrack.createNodeKeyFrame(1.0) 
            key.Translate = ogre.Vector3(80.0, 0.0, -40.0) 
            key = animationTrack.createNodeKeyFrame(2.0) 
            key.Translate = ogre.Vector3(120.0, 0.0, 0.0) 
            key = animationTrack.createNodeKeyFrame(3.0) 
            key.Translate = ogre.Vector3(80.0, 0.0, 80.0) 
            key = animationTrack.createNodeKeyFrame(5.0) 
            key.Translate = ogre.Vector3(-80.0, 0.0, -40.0) 
            key = animationTrack.createNodeKeyFrame(6.0) 
            key.Translate = ogre.Vector3(-120.0, 0.0, 0.0) 
            key = animationTrack.createNodeKeyFrame(7.0) 
            key.Translate = ogre.Vector3(-80.0, 0.0, 100.0) 
            key = animationTrack.createNodeKeyFrame(8.0) 
            key.Translate = ogre.Vector3(30.0, 0.0, 90.0) 
            key = animationTrack.createNodeKeyFrame(9.0) 
            key.Translate = ogre.Vector3(-20.0, 0.0, 40.0) 
            key = animationTrack.createNodeKeyFrame(10.0) 
            key.Translate = ogre.Vector3(0.0, 0.0, 0.0) 
            self.fishLookAtAnimationState = sceneManager.createAnimationState('fishLookAtTrack') 
            self.fishLookAtAnimationState.Enabled = True 
            self.fishLookAtAnimationState.addTime(0.033) #Gives a 33ms headstart to the lookat node 
            
            
        def OnFrameStarted(self,event): 
            "In this example, we will make the fish swim along a path" 
            if self.fishIsSwiming : 
                #First : updates the position of the fish to the position of the moving node on the animation path 
                #The reason that we did not create an animationTrack for fishNode is that we are interested in 
                #keyframing only the node position, not the scale, orientation etc... 
                self.sceneEntities.fishNode.setPosition( self.sceneEntities.fishAnimationNode.Position )
                
                #Then, takes care of the direction that the fish has to face to 
                directionToGo = self.sceneEntities.fishLookAtNode.Position - self.sceneEntities.fishAnimationNode.Position 
                src = self.sceneEntities.fishNode.Orientation * (-ogre.Vector3.UNIT_X) #the fish is originally 
                                                                                       #facing the negative X direction 
                
                
                quat = src.getRotationTo(directionToGo) #Calculates the quaternion of the appropriate rotation 
                self.sceneEntities.fishNode.rotate(quat)#Rotates, the fish. Here, we know that no rotation of 180 
                                                        #degrees will occur so we do not have to care about it 

        
        
        def ToggleSwimAnimation(self): 
            "Toggles on and off the animation" 
            self.fishIsSwiming = not self.fishIsSwiming 
            
            if self.fishIsSwiming : #Adds the animation states for use 
                self.animStates["fishAnimation"] = (self.fishAnimationState, 1.0) 
                self.animStates["fishLookAtAnimation"] = (self.fishLookAtAnimationState, 1.0) 
                self.animStates["swimAnimation"] = (self.swimAnimationState, 4) 
            else : #Removes animation states 
                self.animStates.pop("fishAnimation") 
                self.animStates.pop("fishLookAtAnimation") 
                self.animStates.pop("swimAnimation") 
        
    class DemoFrame(wx.Frame): 
        """Creates a frame with an Ogre window and a pyCrust window 
        pyCrust is always nice to have around for study/debug purposes""" 
        def __init__(self, *args, **kwds): 
            kwds["style"] = wx.DEFAULT_FRAME_STYLE 
            wx.Frame.__init__(self, *args, **kwds) 
            self.pyCrust = wx.py.crust.Crust(self, -1) 
            self.ogreWin = DemoOgreWindow(self, -1) 
            self.swimButton = wx.Button(self, -1, "Start/Stop Swiming") 
            self.trackButton = wx.ToggleButton(self, -1, "Camera Auto Tracking") 
            self.__set_properties() 
            self.__do_layout() 
            self.Bind(wx.EVT_BUTTON, self.swimButtonCallback, self.swimButton) 
            self.Bind(wx.EVT_TOGGLEBUTTON, self.ToggleAutoTracking, self.trackButton) 
            
            
            
            
        def __set_properties(self): 
            self.SetTitle("wxPyOgre Demo") 
            self.ogreWin.SetMinSize((640,480)) 
            self.trackButton.SetValue(False) 
    
        def __do_layout(self): 
            sizer_1 = wx.BoxSizer(wx.VERTICAL) 
            sizer_1.Add(self.ogreWin, 2, wx.EXPAND, 0) 
            sizer_1.Add(self.swimButton,0, wx.ALIGN_CENTER, 0) 
            sizer_1.Add(self.trackButton,0, wx.ALIGN_CENTER, 0) 
            sizer_1.Add(self.pyCrust, 1, wx.EXPAND, 0) 
            self.SetAutoLayout(True) 
            self.SetSizer(sizer_1) 
            sizer_1.Fit(self) 
            sizer_1.SetSizeHints(self) 
            self.Layout() 
            
        def swimButtonCallback(self,event): 
            self.ogreWin.ToggleSwimAnimation() 
            
        def ToggleAutoTracking(self,event): 
            "Toggles on and off the automatic tracking of the fish by the camera" 
            self.ogreWin.sceneEntities.Camera.setAutoTracking(event.Checked(), self.ogreWin.sceneEntities.fishNode) 
    
    
    
    DemoApp = wx.PySimpleApp(0) 
    wx.InitAllImageHandlers() #you may or may not need this 
    MainFrame = DemoFrame(None, -1, "") 
    
    #put all the entities in self.ogreWin.sceneEntities in the local variables so that 
    #introspection with pyCrust is more user friendly 
    localVariableDic=locals() 
    localVariableDic.update(MainFrame.ogreWin.sceneEntities.__dict__) 
    
    DemoApp.SetTopWindow(MainFrame) 
    MainFrame.ogreWin.StartRendering() 
    MainFrame.Show() 
    
    DemoApp.MainLoop() 
