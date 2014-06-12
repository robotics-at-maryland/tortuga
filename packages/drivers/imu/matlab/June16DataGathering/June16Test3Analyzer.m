%analyzes June16Test3 data from the IMU

%run the dataset from the IMU
June16Test3

raw = June16Test3Data;
[rows cols]=size(raw);

time=0:rows-1;

%accel
figure(1)
plot(time,raw(:,1),time,raw(:,2),time,raw(:,3))
legend('X','Y','Z')
title('Accelerometer')

%gyro
omegaX=raw(:,4);
omegaY=raw(:,5);
omegaZ=raw(:,6);
filterSize=10;
filteredX=filterSequence(omegaX,filterSize);
filteredY=filterSequence(omegaY,filterSize);
filteredZ=filterSequence(omegaZ,filterSize);
figure(2)
subplot(3,1,1)
plot(time,omegaX,':',time,filteredX)
legend('raw','filtered')
ylabel('\omega_x')
subplot(3,1,2)
plot(time,omegaY,':',time,filteredY)
legend('raw','filtered')
ylabel('\omega_y')
subplot(3,1,3)
plot(time,omegaZ,':',time,filteredZ)
legend('raw','filtered')
ylabel('\omega_z')




%mag
magMagnitude=zeros(rows,1);
for i=1:1:rows
    magMagnitude(i)=sqrt(raw(i,7)^2+raw(i,8)^2+raw(i,9)^2);
end
magX=raw(:,7);
magY=raw(:,8);
magZ=raw(:,9);
figure(3)
plot(time,magX,time,magY,time,magZ,time,magMagnitude)
legend('X','Y','Z','magnitude')
title('Magnetometer')

%from MEMSense
roll=zeros(rows,1);
for i=1:1:rows
    roll(i)=atan2(magY(i),magX(i));
end
pitch=zeros(rows,1);
for i=1:1:rows
    pitch(i)=atan2(magZ(i),magY(i));
end
yaw=zeros(rows,1);
for i=1:1:rows
    yaw(i)=atan2(magX(i),magZ(i));
end
 
figure(4)
plot(time,roll,time,pitch,time,yaw)
legend('theta_x','theta_y','theta_z')
title('Incorrectly Mapped Orientation')

%what the data sheet says (like code used above) but implemented in the
%imuapi.c
%
%thetaX=raw(:,13);
%thetaY=raw(:,14);
%thetaZ=raw(:,15);

%mapping to go to the correct orientation
thetaX=-raw(:,14);
thetaY=-raw(:,15);
thetaZ=-raw(:,13);

figure(5)
plot(time,thetaX,time,thetaY,time,thetaZ)
legend('theta_x','theta_y','theta_z')
title('Remapped Orientation')

%go to quaternion space
quaternion_measured=zeros(4,rows);
for i=1:1:rows
    quaternion_measured(:,i)=eulerAnglesToQuaternion(thetaX(i),thetaY(i),thetaZ(i));
end

%quaternion magnitude
quaternion_magnitude=zeros(1,rows);
for i=1:1:rows
    quaternion_magnitude(i)=quaternion_measured(1,i)^2+quaternion_measured(2,i)^2+quaternion_measured(3,i)^2+quaternion_measured(4,i)^2;
end

figure(6)
subplot(4,1,1)
plot(time,quaternion_measured(1,:));
title('Measured Quaternion')
ylabel('q_1')
subplot(4,1,2)
plot(time,quaternion_measured(2,:));
ylabel('q_2')
subplot(4,1,3)
plot(time,quaternion_measured(3,:));
ylabel('q_3')
subplot(4,1,4)
plot(time,quaternion_measured(4,:),time,quaternion_magnitude);
ylabel('q_4')
xlabel('time')

%force quaternions to have exclusively positive components
quaternion_positive=zeros(4,rows);
for i=1:1:rows
    quaternion_positive(:,i)=abs(quaternion_measured(:,i));
end

figure(7)
subplot(4,1,1)
plot(time,quaternion_positive(1,:));
title('Forced Positive Quaternion')
ylabel('q_1')
subplot(4,1,2)
plot(time,quaternion_positive(2,:));
ylabel('q_2')
subplot(4,1,3)
plot(time,quaternion_positive(3,:));
ylabel('q_3')
subplot(4,1,4)
plot(time,quaternion_positive(4,:),time,quaternion_magnitude);
ylabel('q_4')
xlabel('time')

quaternion_from_omega=zeros(4,rows);

quaternion_from_omega(:,1)=quaternion_positive(:,1);
for i=2:rows
    %rename angular velocities for quaternion propagation
    w=[filteredX(i) filteredY(i) filteredZ(i)];
    %skew symmetric angular rate
    OMEGA = [0 w(3) -w(2) w(1);
             -w(3) 0 w(1) w(2);
             w(2) -w(1) 0 w(3);
             -w(1) -w(2) -w(3) 0];
    %differential quaternion
    q_dot = (1/2)*OMEGA*quaternion_from_omega(:,i-1);
    %integration
    delta=1;%timestep undefined, use 1 since all time steps are discrete
    q_new = quaternion_from_omega(:,i-1)+q_dot*delta;
    %renormalize
    q_new=q_new/norm(q_new);
    quaternion_from_omega(:,i)=q_new;
end
 
figure(8)
subplot(4,1,1)
plot(time,quaternion_from_omega(1,:));
title('Quaternion from \omega')
ylabel('q_1')
subplot(4,1,2)
plot(time,quaternion_from_omega(2,:));
ylabel('q_2')
subplot(4,1,3)
plot(time,quaternion_from_omega(3,:));
ylabel('q_3')
subplot(4,1,4)
plot(time,quaternion_from_omega(4,:));
ylabel('q_4')
xlabel('time')
