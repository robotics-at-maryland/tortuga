import Ogre
import OgreNewt
## constructor.  we must pass the 2 bodies (2nd can be NULL), also the max DOF for the joint, up to 6.

class MyCustomBallSocket ( OgreNewt.CustomJoint ):
    def __init__ ( self, child, parent, point,  pin ) :
      ## init our variables to keep track of the joint, so we get the joint orientation in local orientation of the bodies.
      ## find local orientations relative to each body, based on the global orientation of the joint.
      OgreNewt.CustomJoint.__init__( self, 3, child, parent )
      self.LocalOrient0, self.LocalPos0, self.LocalOrient1, self.LocalPos1 = \
                            OgreNewt.CustomJoint.pinAndDirToLocal( self,  point, pin )


    ## the important function that applies the joint.
    def submitConstraint(self):
      ## find the global orientation of the joint relative to the 2 bodies.
      globalOrient0, globalPos0 = OgreNewt.CustomJoint.localToGlobal( self, self.LocalOrient0, self.LocalPos0,  0 )
      globalOrient1, globalPos1 = OgreNewt.CustomJoint.localToGlobal( self, self.LocalOrient1, self.LocalPos1,  1 )

      ## to make the joint, we simply apply a constraint to connect 2 points on the bodies along all 3 major axis
      self.addLinearRow( globalPos0, globalPos1, globalOrient0 * Ogre.Vector3.UNIT_X )
      self.addLinearRow( globalPos0, globalPos1, globalOrient0 * Ogre.Vector3.UNIT_Y )
      self.addLinearRow( globalPos0, globalPos1, globalOrient0 * Ogre.Vector3.UNIT_Z )
