function nCb=nCbFromIMU(mag, accel)
%
% nCb=nCbFromIMU(mag, accel)
%
% input: 
%   mag - a 3x1 column vector indicating the components
%           of the local magnetic vector in the body frame
%   accel - a 3x1 column vector indicating the components
%           of the local gravitational vector in the    
%           body frame
%
% output:
%   nCb - a 3x3 direction cosine matrix representing the 
%           rotation of the body frame with respect to the 
%           inertial frame, ie [v]_b = bCn * [v]_n  and 
%           nCb = bCn'
%


%find [n3]_b
n3 = -1*accel;
n3 = n3/norm(n3);

%find [n2]_b
n2 = cross(mag,accel);
n2 = n2/ norm(n2);

%find [n1]_b
n1 = cross(n2,n3);
n1 = n1/norm(n1);

%now create bCn
bCn=zeros(3);
bCn(1,1) = n1(1);
bCn(1,2) = n2(1);
bCn(1,3) = n3(1);
bCn(2,1) = n1(2);
bCn(2,2) = n2(2);
bCn(2,3) = n3(2);
bCn(3,1) = n1(3);
bCn(3,2) = n2(3);
bCn(3,3) = n3(3);

%transpose for the DCM
nCb=bCn';