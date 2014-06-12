function pos = localize_simpleiterative(hydro_pos, tdoas, varargin)
% Find source position by one-step least squares algorithm, based on
% Huang's paper.

num_iters = 10;
  
hydro_pos = strip_first_zeros(hydro_pos);

m = size(hydro_pos);
m = m(1);

A(1:m,1:3) = hydro_pos;
A(1:m,4) = tdoas;
b = 0.5 * (dot(hydro_pos, hydro_pos, 2) - tdoas' .^2);
x = pinv(A)*b;
pos = (x(1:3))';

for i=1:num_iters
  A(m+1,1:3) = pos;
  A(m+1,4) = -x(4);
  b(m+1) = 0;
  x = pinv(A)*b;
  pos = (x(1:3))';
end
