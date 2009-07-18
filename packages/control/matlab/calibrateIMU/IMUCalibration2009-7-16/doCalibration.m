function [magBias,magBiasError,accelBias,accelBiasError,gyroBias,R] = doCalibration(magStationary,accelStationary,magRotating,accelRotating,gyroStationary)

%find average values of mag and accel from stationary test
magStationaryAVG=mean(magStationary);
accelStationaryAVG=mean(accelStationary);

%compute mag bias
magBias=[0 0 0];
magBias(1)=(max(magRotating(:,1))-min(magRotating(:,1)))/2+min(magRotating(:,1));
magBias(2)=(max(magRotating(:,2))-min(magRotating(:,2)))/2+min(magRotating(:,2));
magBias(3)=(max(magRotating(:,3))-min(magRotating(:,3)))/2+min(magRotating(:,3));
%compute mag error
magBiasError=norm(magStationaryAVG-magBias);

%compute accel bias
accelBias=[0 0 0];
accelBias(1)=(max(accelRotating(:,1))-min(accelRotating(:,1)))/2+min(accelRotating(:,1));
accelBias(2)=(max(accelRotating(:,2))-min(accelRotating(:,2)))/2+min(accelRotating(:,2));
accelBias(3)=(max(accelRotating(:,3))-min(accelRotating(:,3)))/2+min(accelRotating(:,3));
%compute accel error
accelBiasError=norm(accelStationaryAVG-accelBias);

%compute gyro bias
gyroBias=mean(gyroStationary);

%compute rotation matrix describing how to take measurements from IMU frame
%to vehicle frame
a=[accelStationaryAVG(1) accelStationaryAVG(2) accelStationaryAVG(3)]';
m=[magStationaryAVG(1) magStationaryAVG(2) magStationaryAVG(3)]';
n3=-a
n3=n3/norm(n3);
n2=cross(m,a);
n2=n2/norm(n2);
n1=cross(n2,n3);

R=[n1'; n2'; n3'];


