function doCalibrationPlots(timeRotating,magRotating,accelRotating,timeStationary,magStationary,accelStationary,magBias,name)


%% magnetometer plots
figure('Name',[name ' Mag'])
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
figure('Name',[name ' Mag time'])
magRotatingSize=zeros(length(timeRotating),1);
for i=1:length(timeRotating)
    x=magRotating(i,1)-magBias(1);
    y=magRotating(i,2)-magBias(2);
    z=magRotating(i,3)-magBias(3);
    magRotatingSize(i)=sqrt(x^2+y^2+z^2);
end
subplot(4,1,1)
plot(timeRotating,magRotating(:,1)-magBias(1))
title('Unbiased Mag Readings')
ylabel('m_1')
subplot(4,1,2)
plot(timeRotating,magRotating(:,2)-magBias(2))
ylabel('m_2')
subplot(4,1,3)
plot(timeRotating,magRotating(:,3)-magBias(3))
ylabel('m_3')
subplot(4,1,4)
plot(timeRotating,magRotatingSize)
ylabel('||m||_2')
xlabel('time')

%% accelerometer plots
figure('Name',[name ' Accel'])
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
figure('Name',[name ' Accel time'])
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

