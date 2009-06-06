%generate a quaternion from an euler axis/angle formulation (easiest to think in)
%euler parameters
e=[0; 1; 1];%0 0 1
e=e/norm(e);
phi = pi/3;%pi/2

%e=[0 0 1] phi = pi/2 should be (0,0,pi/2) in roll pitch yaw 
%                        and [0 0 .7071 .7071] in quaternion space

%quaternion
q=[e(1)*sin(phi/2);
e(2)*sin(phi/2);
e(3)*sin(phi/2);
cos(phi/2)]

DCM=DCMFromQuaternion(q)

newQ=quaternionFromDCM(DCM)