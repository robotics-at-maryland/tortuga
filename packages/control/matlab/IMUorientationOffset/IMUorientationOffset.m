function Roffset = IMUorientationOffset(q)
%hacky way to determine IMU orientation within vehicle

%first get the vehicle *perfectly* level and record 
%the quaternion off the IMU

Rot=R(q);

yaw=getYaw(q);

unYawAxis=[0 0 1]';
unYawAngle=-yaw;
unYawQ=[unYawAxis*sin(unYawAngle/2); cos(unYawAngle/2)];
unYaw=R(unYawQ);


Roffset=unYaw*Rot;