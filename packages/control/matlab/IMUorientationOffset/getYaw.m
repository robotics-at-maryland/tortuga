function yaw = getYaw(q)
% yaw = getYaw(q)
%
% produces estimate of yaw from a quaternion
% based off the "intuitive" definition of yaw
% assuming the orientation is mostly upright
%
% stolen from OGRE quaternion.cpp

x=q(1);
y=q(2);
z=q(3);
w=q(4);

yaw=atan2(2*y*x+2*z*w,1-(2*y*y+2*z*z));

