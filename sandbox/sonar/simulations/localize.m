function pos = localize(hydro_pos, tdoas)
% Find source position by one-step least squares algorithm, based on
% Huang's paper.

hydro_pos = strip_first_zeros(hydro_pos);

m = size(hydro_pos);
m = m(1);

A(1:m,1:3) = hydro_pos;
A(1:m,4) = tdoas;
b = 0.5 * (dot(hydro_pos, hydro_pos, 2) - tdoas' .^2);
x = pinv(A)*b;
pos = (x(1:3))';
