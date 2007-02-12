import os,sys 
if sys.platform == 'win32': 
    os.environ['PATH'] += ';' + __file__[0] 

import Ogre as ogre 
import SampleFramework 
import math 

class TutorialApplication(SampleFramework.Application): 
    def _createScene(self): 
        sm = self.sceneManager 
        sm.ambientLight = (1,1,1) 
        ent1 = sm.createEntity("Robot","robot.mesh") 
        node1 = sm.rootSceneNode.createChildSceneNode("RobotNode") 
        node1.attachObject(ent1) 
        ent2 = sm.createEntity("Robot2",'robot.mesh') 
        node2 = node1.createChildSceneNode("Robot2Node",(50,0,0)) 
        node2.attachObject(ent2) 
        node1.setScale = (2,2,2) 
        node2.setScale = (.5,.5,.5) 
        node2.roll(math.pi/2.0) 
        bb = ent1.getBoundingBox() 
        print "\n\nThis should now print 8 sets of vectors (Corners)"
        for vect in bb.allCorners:
            print vect.x, vect.y, vect.z  
        print "\n\n"
        sys.exit()

if __name__ == '__main__': 
    ta = TutorialApplication() 
    ta.go() 
