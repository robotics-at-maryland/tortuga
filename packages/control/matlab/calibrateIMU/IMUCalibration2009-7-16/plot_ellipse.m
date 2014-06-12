function [ plot_handle ] = plot_ellipse( data )
%PLOT_ELLIPSE Summary of this function goes here
%   Detailed explanation goes here

plot3(data(:,1), data(:,2), data(:,3), '-.k');

[center, radii, evecs, v] = ellipsoid_fit(data);

maxd = max(radii);
step = maxd / 50;
[ x, y, z ] = meshgrid( ...
    (center(1)-radii(1)*1.5) : step : (center(1) + radii(1)*1.5), ...
    (center(2)-radii(2)*1.5) : step : (center(2) + radii(2)*1.5), ...
    (center(3)-radii(3)*1.5) : step : (center(3) + radii(3)*1.5));


Ellipsoid = v(1) *x.*x +   v(2) * y.*y + v(3) * z.*z + ...
          2*v(4) *x.*y + 2*v(5)*x.*z + 2*v(6) * y.*z + ...
          2*v(7) *x    + 2*v(8)*y    + 2*v(9) * z;
p = patch( isosurface( x, y, z, Ellipsoid, 1 ) );
set( p, 'FaceColor', 'g', 'EdgeColor', 'none' );
view( -70, 40 );
axis equal;
camlight;
lighting gouraud;
plot_handle = p;
end

