function Q1=Q1(q)
%
% Q=Q(q)
%
% a controller helper function to help compute alphar=dwr.  this is
% actually the top 3x3 block of the Q(q) matrix
%
% input: 
% a quaternion q=[epsilon; eta] where epsilon is the vector part of
% the quaternion
%
% output: 
% Q1=eta*eye(3)+S(epsilon)
%
%
   e = q(1:3);
   n = q(4);

   Q1=n*eye(3)+S(e);

   
   