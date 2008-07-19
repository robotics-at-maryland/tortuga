function q = eulerAnglesToQuaternion(phi,theta,psi)
%*****************************************
% q = eulerAnglesToQuaternion(phi,theta,psi)
%
% Converts a set of euler angles to quaternion space
%
% input: 
%           phi    - roll in radians
%           theta  - pitch in radians
%           psi    - yaw in radians
% output: 
%           q      - a quaternion with the parameterization:
%
%                   [q1] = [e1*sin(et/2)]
%               q = |q2|   |e2*sin(et/2)|
%                   |q3|   |e3*sin(et/2)|
%                   [q4]   [  cos(et/2) ] 
%
%               where euler axis = [e1,e2,e3] and euler angle = et
%
% Written by Joseph Gland on June 12, 2007
%******************************************

% %use shorthand to minimize trig lookups and make easier to read
% cphi=cos(phi/2);
% ctheta=cos(theta/2);
% cpsi=cos(psi/2);
% sphi=sin(phi/2);
% stheta=sin(theta/2);
% spsi=sin(psi/2);

% %first go to DCM (Direction Cosine Matrix)
% 
% DCM=[ctheta*cpsi ctheta*spsi -stheta;
%      -spsi*cphi+sphi*stheta*cpsi cphi*cpsi+sphi*stheta*spsi sphi*ctheta;
%      spsi*sphi+cphi*stheta*cpsi -sphi*cpsi+cphi*stheta*spsi cphi*ctheta];
 
alpha=psi+phi;
beta=psi-phi;

q=[sin(theta/2)*cos(beta/2);
    sin(theta/2)*sin(beta/2);
    cos(theta/2)*sin(alpha/2);
    cos(theta/2)*cos(alpha/2)];

% ******* believed to be incorrect *******************
% % derivation is long and painful.  see hand notes.
% q =[cphi*ctheta*spsi-sphi*stheta*cpsi;
%     sphi*ctheta*cpsi-cphi*stheta*spsi;
%     cphi*stheta*cpsi+sphi*ctheta*spsi;
%     cphi*ctheta*cpsi+sphi*stheta*spsi];
