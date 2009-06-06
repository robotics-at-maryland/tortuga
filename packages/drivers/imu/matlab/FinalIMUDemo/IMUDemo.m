%analyzes June16Test1 data from the IMU

clear

clc

%run the dataset from the IMU
June16Test1

raw = June16Test1Data;
[rows cols]=size(raw);

%imu to vehicle rotation

thetaPitch=pi/2;
thetaYaw=-pi/2;

PitchRotation=[cos(thetaPitch) 0 -sin(thetaPitch);
               0 1 0;
               sin(thetaPitch) 0 cos(thetaPitch)];

YawRotation=[cos(thetaYaw) sin(thetaYaw) 0;
             -sin(thetaYaw) cos(thetaYaw) 0;
             0 0 1];
IMUToVehicleRotation=YawRotation*PitchRotation;

time=0:rows-1;

%accelerometer data
accel1=raw(:,1);
accel2=raw(:,2);
accel3=raw(:,3);

%rotate raw accelerometer to vehicle orientation
for index=1:1:length(time)
    accel=[accel1(index) accel2(index) accel3(index)]';
    accel=IMUToVehicleRotation*accel;
    accel1(index)=accel(1);
    accel2(index)=accel(2);
    accel3(index)=accel(3);
end

%filter accelerometer
filterSize=10;
filteredAccel1=filterSequence(accel1,filterSize);
filteredAccel2=filterSequence(accel2,filterSize);
filteredAccel3=filterSequence(accel3,filterSize);

%plot accelerometer (in g's)
figure(1)
subplot(3,1,1)
plot(time,accel1,':',time,filteredAccel1);
title('Accelerometer')
ylabel('a_x')
subplot(3,1,2)
plot(time,accel2,':',time,filteredAccel2);
ylabel('a_y')
subplot(3,1,3)
plot(time,accel3,':',time,filteredAccel3);
ylabel('a_z')
xlabel('time')



% angular rate data (comes in deg/s, convert to rad/s
omega1=raw(:,4)*pi/180;
omega2=raw(:,5)*pi/180;
omega3=raw(:,6)*pi/180;

%rotate raw angular rate to vehicle orientation
for index=1:1:length(time)
    omega=[omega1(index) omega2(index) omega3(index)]';
    omega=IMUToVehicleRotation*omega;
    omega1(index)=omega(1);
    omega2(index)=omega(2);
    omega3(index)=omega(3);
end

%filter angular rate
filterSize=10;
filteredOmega1=filterSequence(omega1,filterSize);
filteredOmega2=filterSequence(omega2,filterSize);
filteredOmega3=filterSequence(omega3,filterSize);

%plot angular rate
figure(2)
subplot(3,1,1)
plot(time,omega1,':',time,filteredOmega1)
ylabel('\omega_x')
title('Angular Rate Sensors')
subplot(3,1,2)
plot(time,omega2,':',time,filteredOmega2)
ylabel('\omega_y')
subplot(3,1,3)
plot(time,omega3,':',time,filteredOmega3)
ylabel('\omega_z')



% magenetometer data (in gauss)
mag1=raw(:,7);
mag2=raw(:,8);
mag3=raw(:,9);

% rotate magnetometer to vehicle orientation
for index=1:1:length(time)
    mag=[mag1(index) mag2(index) mag3(index)]';
    mag=IMUToVehicleRotation*mag;
    mag1(index)=mag(1);
    mag2(index)=mag(2);
    mag3(index)=mag(3);
end

%filter angular rate
filterSize=10;
filteredMag1=filterSequence(mag1,filterSize);
filteredMag2=filterSequence(mag2,filterSize);
filteredMag3=filterSequence(mag3,filterSize);

%plot magnetometer data
figure(3)
subplot(3,1,1)
plot(time,mag1,':',time,filteredMag1)
ylabel('Mag_x in Gauss')
title('Magnetometer')
subplot(3,1,2)
plot(time,mag2,':',time,filteredMag2)
ylabel('Mag_y in Gauss')
subplot(3,1,3)
plot(time,mag3,':',time,filteredMag3)
ylabel('Mag_z in Gauss')

%find quaternion from IMU data
quaternion=zeros(4,length(time));
%magnetic pitch at SSL = -70 deg
magneticPitch=70*pi/180;

for index=1:1:length(time)
    magData=[filteredMag1(index) filteredMag2(index) filteredMag3(index)]';
    accelData=[filteredAccel1(index) filteredAccel2(index) filteredAccel3(index)]';
    quaternion(:,index) = quaternionFromIMU(magData, accelData, magneticPitch);
end

%plot quaternion
figure(4)
subplot(4,1,1)
plot(time,quaternion(1,:));
title('Quaternion from Triad Algorithm')
ylabel('q_1')
subplot(4,1,2)
plot(time,quaternion(2,:));
ylabel('q_2')
subplot(4,1,3)
plot(time,quaternion(3,:));
ylabel('q_3')
subplot(4,1,4)
plot(time,quaternion(4,:));
ylabel('q_4')
xlabel('time')
