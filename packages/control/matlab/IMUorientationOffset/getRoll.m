function roll = getRoll(q)
% roll = getRoll(q)
%
% produces estimate of roll from a quaternion
% based off the "intuitive" definition of yaw
% assuming the orientation is mostly upright
%
% stolen from OGRE quaternion.cpp

x=q(1);
y=q(2);
z=q(3);
w=q(4);

roll=atan2(2*z*y+2*w*x,1-(2*x*x+2*z*z));

