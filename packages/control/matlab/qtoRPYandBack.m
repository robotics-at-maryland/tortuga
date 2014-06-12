%generate a quaternion from an euler axis/angle formulation (easiest to think in)
%euler parameters
e=[0; 1; 1];%0 0 1
e=e/norm(e)
phi = pi/3;%pi/2

%e=[0 0 1] phi = pi/2 should be (0,0,pi/2) in roll pitch yaw 
%                        and [0 0 .7071 .7071] in quaternion space

%quaternion
q=[e(1)*sin(phi/2);
e(2)*sin(phi/2);
e(3)*sin(phi/2);
cos(phi/2)]

%convert to rpy
roll=atan2(2*(q(4)*q(1)+q(2)*q(3)),(1-2*(q(1)^2+q(2)^2)))
pitch=asin(2*(q(4)*q(2)-q(3)*q(1)))
yaw=atan2(2*(q(4)*q(3)+q(1)*q(2)),(1-2*(q(2)^2+q(3)^2)))

%rename components
phi=roll;
theta=pitch;
psi=yaw;

%back to quaternion
alpha=psi+phi;
beta=psi-phi;

q=[sin(theta/2)*cos(beta/2);
    sin(theta/2)*sin(beta/2);
    cos(theta/2)*sin(alpha/2);
    cos(theta/2)*cos(alpha/2)]