function [rollPitchYaw] = rollPitchYawFromQuaternion(q)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% [roll; pitch; yaw] = rollPitchYawFromQuaternion(q)
%
% rollPitchYawFromQuaternion converts a quaternion into euler angles
%
% input: 
% q      - a quaternion with the parameterization:
%
%                   [q1] = [e1*sin(et/2)]
%               q = |q2|   |e2*sin(et/2)|
%                   |q3|   |e3*sin(et/2)|
%                   [q4]   [  cos(et/2) ] 
%
%               where euler axis = [e1,e2,e3] and euler angle = et
%
% output:
% roll pitch and yaw in radians
%
% Written by Joseph Gland on June 28, 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%convert to rpy
roll=atan2(2*(q(4)*q(1)+q(2)*q(3)),(1-2*(q(1)^2+q(2)^2)));
pitch=asin(2*(q(4)*q(2)-q(3)*q(1)));
yaw=atan2(2*(q(4)*q(3)+q(1)*q(2)),(1-2*(q(2)^2+q(3)^2)));

rollPitchYaw=[roll; pitch; yaw];