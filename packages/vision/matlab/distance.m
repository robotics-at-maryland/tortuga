function length = distance( pt1, pt2 )
%DISTANCE Find the distance between two points
%   Convenience function for finding the difference between two
%   points. The points are in the form of [x, y].
xdiff = pt1(1) - pt2(1);
ydiff = pt1(2) - pt2(2);
length = sqrt(xdiff.^2 + ydiff.^2);
end
