%get euler angles from quaternion test cases

clear

%test getYaw

axis=[0,0,1]';
angle=pi/5
q=[axis*sin(angle/2); cos(angle/2)];

yaw=getYaw(q)

%test getRoll

axis=[1,0,0]';
angle=pi/3
q=[axis*sin(angle/2); cos(angle/2)];

roll=getRoll(q)

%test getPitch

axis=[0,1,0]';
angle=-pi/3
q=[axis*sin(angle/2); cos(angle/2)];

pitch=getPitch(q)

%test IMUorientationOffset

%create big yaw
axis=[0,0,1]';
angle=-pi/3;
qyaw=[axis*sin(angle/2); cos(angle/2)]
%create small roll offset to detect
axis=[1,0,0]';
angle=-pi/35;
qroll=[axis*sin(angle/2); cos(angle/2)]
%combine the rotations
%recall quaternionFromnCb(R(q)')=q
qvehicle=quaternionFromnCb((R(qroll)*R(qyaw))')
Roffset = IMUorientationOffset(qvehicle)

%check answer, should be equal to qroll
qanswer=quaternionFromnCb(Roffset)
