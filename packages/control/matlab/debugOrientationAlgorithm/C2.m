function RotationMatrix = C2(theta)
%
% RotationMatrix = C2(theta)
%
% rotation about the 2 axis by angle theta (in radians)
%

RotationMatrix=[cos(theta) 0 -sin(theta); 
                0 1 0;
                sin(theta) 0 cos(theta)];
            