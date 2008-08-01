function Rout=R(q)
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

%force quaternion into a column vector
qformatted=[q(1) q(2) q(3) q(4)]';

%split into vector and scalar portion
e = qformatted(1:3);
n = qformatted(4);

%generate rotation matrix
Rout = (n^2-e'*e)*eye(3)+2*e*e'-2*n*S(e);
   