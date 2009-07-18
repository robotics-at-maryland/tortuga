%a matlab script to view relevant IMU calibration data from 
%log files
%currently requires python program to perform some of the analysis

clear
clc
close all


%% load log files
imuStationary = load('./stationary/imu.log');
imuRotating   = load('./rotating/imu.log');

%% parse log file data 
% IMU.log
% IMU#(0=main,1=boom) Accel Mag Gyro Accel-Raw Mag-Raw Gyro-Raw Quat TimeStamp
% 1                     3     3   3      3         3       3       4    1
% 1                    2:4  5:7  8:10   11:13    14:16  17:19

%pull out mag and accel data and separate for CG and boom
%rotating data
indCG=1;
indBoom=1;
[imuRow imuCol]=size(imuRotating);
%create blank arrays
timeRotatingCGt=zeros(imuRow,1);
magRotatingCGt=zeros(imuRow,3);
accelRotatingCGt=zeros(imuRow,3);
timeRotatingBoomt=zeros(imuRow,1);
magRotatingBoomt=zeros(imuRow,3);
accelRotatingBoomt=zeros(imuRow,3);
%parse log
for i=1:imuRow
    %CG imu
    if imuRotating(i,1)==0
        timeRotatingCGt(indCG)=imuRotating(i,end)/1000;
        magRotatingCGt(indCG,:)=imuRotating(i,14:16);
        accelRotatingCGt(indCG,:)=imuRotating(i,11:13);
        indCG=indCG+1;
    end
    %boom imu
    if imuRotating(i,1)==1
        timeRotatingBoomt(indBoom)=imuRotating(i,end)/1000;
        magRotatingBoomt(indBoom,:)=imuRotating(i,14:16);
        accelRotatingBoomt(indBoom,:)=imuRotating(i,11:13);
        indBoom=indBoom+1;
    end
end
%copy arrays to correct sized arrays w/o zeros at the end
timeRotatingCG=timeRotatingCGt(1:indCG-1);
magRotatingCG=magRotatingCGt(1:indCG-1,:);
accelRotatingCG=accelRotatingCGt(1:indCG-1,:);
timeRotatingBoom=timeRotatingBoomt(1:indBoom-1);
magRotatingBoom=magRotatingBoomt(1:indBoom-1,:);
accelRotatingBoom=accelRotatingBoomt(1:indBoom-1,:);


%pull out mag and accel data and separate for CG and boom
%stationary data
indCG=1;
indBoom=1;
[imuRow imuCol]=size(imuStationary);
%create blank arrays
timeStationaryCGt=zeros(imuRow,1);
magStationaryCGt=zeros(imuRow,3);
accelStationaryCGt=zeros(imuRow,3);
timeStationaryBoomt=zeros(imuRow,1);
magStationaryBoomt=zeros(imuRow,3);
accelStationaryBoomt=zeros(imuRow,3);
gyroStationaryCGt=zeros(imuRow,3);
gyroStationaryBoomt=zeros(imuRow,3);
%parse log
for i=1:imuRow
    %CG imu
    if imuStationary(i,1)==0
        timeStationaryCGt(indCG)=imuStationary(i,end)/1000;
        magStationaryCGt(indCG,:)=imuStationary(i,14:16);
        accelStationaryCGt(indCG,:)=imuStationary(i,11:13);
        gyroStationaryCGt(indCG,:)=imuStationary(i,17:19);
        indCG=indCG+1;
    end
    %boom imu
    if imuStationary(i,1)==1
        timeStationaryBoomt(indBoom)=imuStationary(i,end)/1000;
        magStationaryBoomt(indBoom,:)=imuStationary(i,14:16);
        accelStationaryBoomt(indBoom,:)=imuStationary(i,11:13);
        gyroStationaryBoomt(indBoom,:)=imuStationary(i,17:19);
        indBoom=indBoom+1;
    end
end
%copy arrays to correct sized arrays w/o zeros at the end
timeStationaryCG=timeStationaryCGt(1:indCG-1);
magStationaryCG=magStationaryCGt(1:indCG-1,:);
accelStationaryCG=accelStationaryCGt(1:indCG-1,:);
gyroStationaryCG=gyroStationaryCGt(1:indCG-1,:);
timeStationaryBoom=timeStationaryBoomt(1:indBoom-1);
magStationaryBoom=magStationaryBoomt(1:indBoom-1,:);
accelStationaryBoom=accelStationaryBoomt(1:indBoom-1,:);
gyroStationaryBoom=gyroStationaryBoomt(1:indBoom-1,:);


%%

[mBiasBoom,mBiasErrorBoom,aBiasBoom,aBiasErrorBoom,gyroBiasBoom,Rboom]=doCalibration(magStationaryBoom,accelStationaryBoom,magRotatingBoom,accelRotatingBoom,gyroStationaryBoom)
%mBiasBoom
%mAVGBoom=mean(magStationaryBoom)
%mBiasErrorBoom
[mBiasCG,mBiasErrorCG,aBiasCG,aBiasErrorCG,gyroBiasCG,RCG]=doCalibration(magStationaryCG,accelStationaryCG,magRotatingCG,accelRotatingCG,gyroStationaryCG)

doCalibrationPlots(timeRotatingBoom,magRotatingBoom,accelRotatingBoom,timeStationaryBoom,magStationaryBoom,accelStationaryBoom,mBiasBoom,'Boom')
doCalibrationPlots(timeRotatingCG,magRotatingCG,accelRotatingCG,timeStationaryCG,magStationaryCG,accelStationaryCG,mBiasCG,'CG')

