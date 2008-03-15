function mat = J(q)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% mat = J(q)
%
% function J.m is used to find a matrix required for the calculation of the
% derivative of a quaternion
%
% assumes the quaternion is written as 
%
% q = [e(0)*sin(phi/2); e(1)*sin(phi/2); e(2)*sin(phi/2); cos(phi/2)]
%
% written by Joseph Gland
% June 22 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if(length(q)~=4)
    disp('argument not 4x1')
else
    mat=(1/2)*[q(4)*eye(3)+S(q(1:3));  -q(1:3)'];%4x4skew-symm matrix
end