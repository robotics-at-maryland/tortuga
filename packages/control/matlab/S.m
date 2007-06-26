function mat = S(a)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% function S.m is used to find a skew symmetric matrix based off a 3x1
% vector
%
% written by Joseph Gland
% June 22 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if(length(a)~=3)
    disp('argument not 3x1')
else
    mat=[0 -a(3) a(2);
         a(3) 0 -a(1);
         -a(2) a(1) 0];
end