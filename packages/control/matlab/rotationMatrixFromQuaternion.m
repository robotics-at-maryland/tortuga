function rotationMatrix = rotationMatrixFromQuaternion(q)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% rotationMatrix = rotationMatrixFromQuaternion(q)
%
% function rotationMatrixFromQuaternion.m creates a rotation matrix from
% a quaternion (bodyFrame = rotationMatrix*inertialFrame)
%
% assumes the quaternion is written as 
%
% q = [e(0)*sin(phi/2); e(1)*sin(phi/2); e(2)*sin(phi/2); cos(phi/2)]
%
% written by Joseph Gland
% June 27 2007
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

rotationMatrix = [q(4)^2+q(1)^2-q(2)^2-q(3)^2 2*(q(1)*q(2)+q(3)*q(4)) 2*(q(1)*q(3)-q(2)*q(4));
                  2*(q(1)*q(2)-q(3)*q(4)) q(4)^2-q(1)^2+q(2)^2-q(3)^2 2*(q(2)*q(3)+q(1)*q(4));
                  2*(q(1)*q(3)+q(2)*q(4)) 2*(q(2)*q(3)-q(1)*q(4)) q(4)^2-q(1)^2-q(2)^2+q(3)^2];
              