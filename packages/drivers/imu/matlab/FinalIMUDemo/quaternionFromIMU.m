function q = quaternionFromIMU(mag, accel, magneticPitch)
%*****************************************
% q = quaternionFromIMU(mag, accel)
%
% finds a quaternion from magnetometer and accelerometer data
%
% input: 
%           mag    - the local magnetic field vector in any units
%           accel  - the local gravity vector in any units
%           magneticPitch - the pitch of the local gravity vector
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
% Written by Joseph Gland on June 30, 2007
%******************************************

%normalize accelerometer data
accel=accel/norm(accel);

%normalize magnetometer data
mag=mag/norm(mag);

%x component is magnetic vector
xComponent=mag;

%y component is xComponent crossed with gravity vector
yComponent=cross(xComponent,accel);
yComponent=yComponent/norm(yComponent);

%z component is xComponent crossed with yComponent
zComponent=cross(xComponent,yComponent);
zComponent=zComponent/norm(zComponent);

%direction cosine matrix is composed of the x, y, and z components
DCM=[xComponent'; yComponent'; zComponent'];

%rotate DCM by magnetic pitch
magneticPitchMatrix=[cos(magneticPitch) 0 -sin(magneticPitch);
                     0 1 0;
                     sin(magneticPitch) 0 cos(magneticPitch)];
%DCM
%magneticPitchMatrix
DCM=magneticPitchMatrix*DCM;
invDCM=inv(DCM);

q = quaternionFromDCM(invDCM);
