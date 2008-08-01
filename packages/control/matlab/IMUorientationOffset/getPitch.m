function pitch = getPitch(q)
% pitch = getPitch(q)
%
% produces estimate of pitch from a quaternion
% based off the "intuitive" definition of yaw
% assuming the orientation is mostly upright
%
% stolen from OGRE quaternion.cpp

x=q(1);
y=q(2);
z=q(3);
w=q(4);

pitch=atan2(2*x*z+2*w*y,1-(2*x*x+2*y*y));

