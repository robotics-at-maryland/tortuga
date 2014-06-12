function out=S(e)
%
% out=S(e)
%
% skew symmetric matrix
%
% input: a 3 vector
%
% output: a skew symmetric matrix
%
 out = [0 -e(3) e(2); e(3) 0 -e(1); -e(2) e(1) 0];
