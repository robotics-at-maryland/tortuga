function RotationMatrix = C3(theta)
%
% RotationMatrix = C3(theta)
%
% rotation about the 3 axis by angle theta (in radians)
%

RotationMatrix=[cos(theta) sin(theta) 0; 
                -sin(theta) cos(theta) 0;
                0 0 1];
            