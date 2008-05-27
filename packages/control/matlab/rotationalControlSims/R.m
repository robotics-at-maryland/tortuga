function R=R(q)
%
% R=R(q)
%
% a rotation matrix representing the attitude of the quaternion wrt the
% quaternion's reference frame
%
% input: 
% a quaternion q=[epsilon; eta] where epsilon is the vector part of
% the quaternion
%
% output: 
% rotation matrix R = (n^2-e'*e)*eye(3)+2*e*e'-2*n*S(e)
%

   e = q(1:3);
   n = q(4);

   R = (n^2-e'*e)*eye(3)+2*e*e'-2*n*S(e);