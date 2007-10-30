function pos = localize(hydro_pos, tdoas)
% Find source position by one-step least squares algorithm, based on
% Huang's paper.

% If the matrix begins with a zero-filled row, strip it out
if all(hydro_pos(1,:)==[0,0,0])
  hydro_pos = hydro_pos(2:end,:);
end

m = size(hydro_pos);
m = m(1);

A(1:m,1:3) = hydro_pos;
A(1:m,4) = tdoas;
b = 0.5 * (dot(hydro_pos, hydro_pos, 2) - tdoas .^2);
x = pinv(A)*b;
pos = x(1:3);
