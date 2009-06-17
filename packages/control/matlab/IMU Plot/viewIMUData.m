%file to view data from an IMU after a dive
%modified from calibration.m used for IMU calibration

clear

close all


%% load log files
imuData = load('imu.log');
controlData = load('control.log');
%imuRotating   = load('rotating_imu.log');

%% parse log file data 
% IMU.log
%Accel Mag Gyro Accel-Raw Mag-Raw Gyro-Raw Quat TimeStamp 
%  3    3   3       3         3        3    4     1


timeIMU=imuData(:,end)/1000;
mag=imuData(:,4:6);
accel=imuData(:,1:3);

% Control.log
% M-Quat M-AngRate M-Depth D-Quat D-AngRate D-Depth D-Speed RotTorq TranForce AdaptCtrlParams Time
%    4      3        1       4       3        1        1       3       3           12 
q_m  = controlData(:,1:4); %measured quaternion (unitless)
w_m  = (180/pi)*controlData(:,5:7); %measured angular rate (we're converting to deg/s)
depth_m = controlData(:,8); %measured depth (magical units ~= 2.3ft?)
q_d = controlData(:,9:12); %desired quaternion (unitless)
w_d = (180/pi)*controlData(:,13:15); %desired angular rate (we're converting to deg/s)
depth_d = controlData(:,16); %desired depth  (magical units again)
speed_d = controlData(:,17); %desired speed  (we don't even pretend to know units here)
rot_torq = controlData(:,18:20); %commanded torque output (N*m)
tran_force = controlData(:,21:23); %commanded force output  (N)
timeControl=controlData(:,end)/1000;


%% circle points
%generate a set of points to represent a unit circle
tCir=0:0.1:2*pi+0.1;
xCir=cos(tCir);
yCir=sin(tCir);


%% magnetometer plots
figure(1)
subplot(2,2,1)
plot(mag(:,2),mag(:,1),0.22*xCir,0.22*yCir);
title('                               Magnetometer Data (filtered)')
xlabel('y')
ylabel('x')
%axis equal
axis([-.3 .3 -.3 .3])
set(gca, 'XDir', 'reverse')

subplot(2,2,2)
plot3(mag(:,1),mag(:,2),mag(:,3))
xlabel('x')
ylabel('y')
zlabel('z')
axis([-.3 .3 -.3 .3 -.3 .3])

subplot(2,2,3)
plot(mag(:,2),mag(:,3), 0.22*xCir, 0.22*yCir);
xlabel('y')
ylabel('z')
%axis equal
axis([-.3 .3 -.3 .3])
set(gca, 'XDir', 'reverse')

subplot(2,2,4)
plot(mag(:,1),mag(:,3),0.22*xCir,0.22*yCir);
xlabel('x')
ylabel('z')
%axis equal
axis([-.3 .3 -.3 .3])

%% rotating magnetometer data
figure(2)
magRotatingSize=zeros(length(timeIMU),1);
for i=1:length(timeIMU)
    magSize(i)=sqrt(mag(i,1)^2+mag(i,2)^2+mag(i,3)^2);
end
subplot(4,1,1)
plot(timeIMU,mag(:,1))
title('Mag Readings')
ylabel('m_1')
axis([0 timeIMU(end) -.3 .3])
subplot(4,1,2)
plot(timeIMU,mag(:,2))
ylabel('m_2')
axis([0 timeIMU(end) -.3 .3])
subplot(4,1,3)
plot(timeIMU,mag(:,3))
ylabel('m_3')
axis([0 timeIMU(end) -.3 .3])
subplot(4,1,4)
plot(timeIMU,magSize)
ylabel('||m||_2')
xlabel('time')
axis([0 timeIMU(end) 0 .4])

%% accelerometer plots
figure(3)
subplot(2,2,1)
plot(accel(:,2),accel(:,1),xCir,yCir);
title('                               Accelerometer Data')
xlabel('y')
ylabel('x')
%axis equal
axis([-1.1 1.1 -1.1 1.1])
set(gca, 'XDir', 'reverse')

subplot(2,2,2)
plot3(accel(:,1),accel(:,2),accel(:,3))
xlabel('x')
ylabel('y')
zlabel('z')
%axis equal
axis([-1.1 1.1 -1.1 1.1 -1.1 1.1])

subplot(2,2,3)
plot(accel(:,2),accel(:,3),xCir,yCir);
xlabel('y')
ylabel('z')
%axis equal
axis([-1.1 1.1 -1.1 1.1])
set(gca, 'XDir', 'reverse')

subplot(2,2,4)
plot(accel(:,1),accel(:,3),xCir,yCir);
xlabel('x')
ylabel('z')
%axis equal
axis([-1.1 1.1 -1.1 1.1])

%% rotating accelerometer data
figure(4)
accelSize=zeros(length(timeIMU),1);
for i=1:length(timeIMU)
    accelSize(i)=sqrt(accel(i,1)^2+accel(i,2)^2+accel(i,3)^2);
end
subplot(4,1,1)
plot(timeIMU,accel(:,1))
title('Accel Readings')
ylabel('a_1')
axis([0 timeIMU(end) -1.1 1.1])
subplot(4,1,2)
plot(timeIMU,accel(:,2))
ylabel('a_2')
axis([0 timeIMU(end) -1.1 1.1])
subplot(4,1,3)
plot(timeIMU,accel(:,3))
ylabel('a_3')
axis([0 timeIMU(end) -1.1 1.1])
subplot(4,1,4)
plot(timeIMU,accelSize)
ylabel('||a||_2')
axis([0 timeIMU(end) 0 1.3])


%% 







