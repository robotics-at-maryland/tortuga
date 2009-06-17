%a matlab script to view relevant IMU calibration data from 
%log files
%currently requires python program to perform some of the analysis

clear

close all


%% load log files
imuStationary = load('stationary_imu.log');
imuRotating   = load('rotating_imu.log');

%% parse log file data 
%Accel Mag Gyro Accel-Raw Mag-Raw Gyro-Raw Quat TimeStamp 
%  3    3   3       3         3        3    4     1
timeRotating=imuRotating(:,end)/1000;
magRotating=imuRotating(:,13:15);
accelRotating=imuRotating(:,10:12);

magStationary=imuStationary(:,13:15);
accelStationary=imuStationary(:,10:12);
magStationaryAVG=mean(magStationary)
accelStationaryAVG=mean(accelStationary)

%% find mag biases
magXBias=(max(magRotating(:,1))-min(magRotating(:,1)))/2+min(magRotating(:,1))
magYBias=(max(magRotating(:,2))-min(magRotating(:,2)))/2+min(magRotating(:,2))
magZBias=(max(magRotating(:,3))-min(magRotating(:,3)))/2+min(magRotating(:,3))

magStationaryAVGUnbiased=magStationaryAVG-[magXBias magYBias magZBias]


%% magnetometer plots
figure(1)
subplot(2,2,1)
plot(magRotating(:,2),magRotating(:,1));
title('                               Magnetometer Data')
xlabel('y')
ylabel('x')
axis equal
set(gca, 'XDir', 'reverse')

subplot(2,2,2)
plot3(magRotating(:,1),magRotating(:,2),magRotating(:,3))
xlabel('x')
ylabel('y')
zlabel('z')
axis equal

subplot(2,2,3)
plot(magRotating(:,2),magRotating(:,3));
xlabel('y')
ylabel('z')
axis equal
set(gca, 'XDir', 'reverse')

subplot(2,2,4)
plot(magRotating(:,1),magRotating(:,3));
xlabel('x')
ylabel('z')
axis equal

%% rotating magnetometer data
figure(2)
magRotatingSize=zeros(length(timeRotating),1);
for i=1:length(timeRotating)
    x=magRotating(i,1)-magXBias;
    y=magRotating(i,2)-magYBias;
    z=magRotating(i,3)-magZBias;
    magRotatingSize(i)=sqrt(x^2+y^2+z^2);
end
subplot(4,1,1)
plot(timeRotating,magRotating(:,1)-magXBias)
title('Unbiased Mag Readings')
ylabel('m_1')
subplot(4,1,2)
plot(timeRotating,magRotating(:,2)-magYBias)
ylabel('m_2')
subplot(4,1,3)
plot(timeRotating,magRotating(:,3)-magZBias)
ylabel('m_3')
subplot(4,1,4)
plot(timeRotating,magRotatingSize)
ylabel('||m||_2')
xlabel('time')

%% accelerometer plots
figure(3)
subplot(2,2,1)
plot(accelRotating(:,2),accelRotating(:,1));
title('                               Accelerometer Data')
xlabel('y')
ylabel('x')
axis equal
set(gca, 'XDir', 'reverse')

subplot(2,2,2)
plot3(accelRotating(:,1),accelRotating(:,2),accelRotating(:,3))
xlabel('x')
ylabel('y')
zlabel('z')
axis equal

subplot(2,2,3)
plot(accelRotating(:,2),accelRotating(:,3));
xlabel('y')
ylabel('z')
axis equal
set(gca, 'XDir', 'reverse')

subplot(2,2,4)
plot(accelRotating(:,1),accelRotating(:,3));
xlabel('x')
ylabel('z')
axis equal

%% rotating accelerometer data
figure(4)
accelRotatingSize=zeros(length(timeRotating),1);
for i=1:length(timeRotating)
    %found negligible accelerometer bias, so commenting out
%    x=accelRotating(i,1)-accelXBias;
%    y=accelRotating(i,2)-accelYBias;
%    z=accelRotating(i,3)-accelZBias;
%    accelRotatingSize(i)=sqrt(x^2+y^2+z^2);
    accelRotatingSize(i)=norm(accelRotating(i,:));
end
subplot(4,1,1)
%plot(timeRotating,accelRotating(:,1)-accelXBias)
plot(timeRotating,accelRotating(:,1))
title('Unbiased Accel Readings')
ylabel('a_1')
subplot(4,1,2)
%plot(timeRotating,accelRotating(:,2)-accelYBias)
plot(timeRotating,accelRotating(:,2))
ylabel('a_2')
subplot(4,1,3)
%plot(timeRotating,accelRotating(:,3)-accelZBias)
plot(timeRotating,accelRotating(:,3))
ylabel('a_3')
subplot(4,1,4)
plot(timeRotating,accelRotatingSize)
ylabel('||a||_2')







