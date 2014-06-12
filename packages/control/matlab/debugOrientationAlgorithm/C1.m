function RotationMatrix = C1(theta)
%
% RotationMatrix = C1(theta)
%
% rotation about the 1 axis by angle theta (in radians)
%

RotationMatrix=[1 0 0; 
                0 cos(theta) sin(theta);
                0 -sin(theta) cos(theta)];
            