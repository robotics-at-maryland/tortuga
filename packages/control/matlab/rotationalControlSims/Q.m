function Q=Q(q)
%
% Q=Q(q)
%
% a matrix used for computaions of quaternion kinematics
%
% input: 
% a quaternion q=[epsilon; eta] where epsilon is the vector part of
% the quaternion
%
% output: 
% Q=[eta*eye(3)+S(epsilon); -epsilon']
%
% usage:
% dq/dt = 0.5*Q(q)*w
%
   e = q(1:3);
   n = q(4);

   Q1=n*eye(3)+S(e);
   Q=[Q1; -e'];
   
   