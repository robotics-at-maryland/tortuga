function pos = localize_depthaugmented(hydro_pos, tdoas, zs)
% Find source position by one-step least squares algorithm, based on
% Huang's paper.

hydro_pos = strip_first_zeros(hydro_pos);

m = size(hydro_pos);
m = m(1);

A = hydro_pos(1:m,1:2);
A(1:m,3) = tdoas;
b = 0.5 * (dot(hydro_pos, hydro_pos, 2) - tdoas' .^2 - hydro_pos(:,3)*zs);
x = pinv(A)*b;
pos = (x(1:2))';
pos(3) = zs;
