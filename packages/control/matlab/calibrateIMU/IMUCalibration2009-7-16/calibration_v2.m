% The purpose of this script is to calculate calibration coefficients from
% logs of imu data.  One log is to be taken while the robot is stationary
% at the nominal orientation (typically North and level to the ground).
% The other log is to be taken while the robot is being manually rotated
% into as many orientations as realistically possible at as close to a
% constant angular rate as possible.  This second logfile requires
% considerably more data than the first. 10-15 minutes of rotating should
% be sufficient as opposed to 30 seconds to a minute of stationary data.

% This is the second version of the IMU calibration routine.  The main
% improvement is how sensor bias is calculated.  We will fit an ellipsoid
% (which will ideally turn out to be a sphere) to the rotating log data in
% order to calculate the bias from the coordinates of the center of the
% ellipsoid.  If there is no bias, the center will reside at the origin
% since the magnetic and gravitational fields are effectively uniform and
% constant at the distance scales we operate at.  In the presence of sensor
% bias, this center will be shifted.  The bias is to be subtracted from
% measurements before performing orientation estimation with them.

% Copyright (C) 2011 Robotics @ Maryland
% Copyright (C) 2011 Jonathan Wonders
% Author: Jonathan Wonders

%% clear all variables / close figures / and clear the output window
clear all
close all
clc

%% load the log files
imuStationaryLog = load('./stationary/imu.log');
imuRotatingLog = load('./rotating/imu.log');

[cgTimeRot, cgMagRot, cgAccRot, cgGyroRot, ...
    boomTimeRot, boomMagRot, boomAccRot, boomGyroRot] = ...
    parseIMULog(imuRotatingLog);

[cgTimeStat, cgMagStat, cgAccStat, cgGyroStat ...
    boomTimeStat, boomMagStat, boomAccStat, boomGyroStat] = ...
    parseIMULog(imuStationaryLog);

%% calculate the calibration coefficients

cgMagBias = ellipsoid_fit(cgMagRot);
cgAccBias = ellipsoid_fit(cgAccRot);
cgGyroBias = mean(cgGyroStat);
cgR = rotationMatrixFromMagAccel(cgMagStat, cgAccStat);

boomMagBias = ellipsoid_fit(boomMagRot);
boomAccBias = ellipsoid_fit(boomAccRot);
boomGyroBias = mean(boomGyroStat);
boomR = rotationMatrixFromMagAccel(boomMagStat, boomAccStat);

%% display results in a similar format as our config files
fprintf('CG IMU:\n');
fprintf('\taccelXBias: %f\n', cgAccBias(1));
fprintf('\taccelYBias: %f\n', cgAccBias(2));
fprintf('\taccelZBias: %f\n\n', cgAccBias(3));

fprintf('\tmagXBias: %f\n', cgMagBias(1));
fprintf('\tmagYBias: %f\n', cgMagBias(2));
fprintf('\tmagZBias: %f\n\n', cgMagBias(3));

fprintf('\tgyroXBias: %f\n', cgGyroBias(1));
fprintf('\tgyroYBias: %f\n', cgGyroBias(2));
fprintf('\tgyroZBias: %f\n\n', cgGyroBias(3));

fprintf('\timuToVehicleRotMatrix:\n[ %f, %f, %f ],\n[ %f, %f, %f ],\n[ %f, %f, %f ]\n',...
        cgR);

fprintf('\n\n');
fprintf('BOOM IMU:\n');
fprintf('\taccelXBias: %f\n', boomAccBias(1));
fprintf('\taccelYBias: %f\n', boomAccBias(2));
fprintf('\taccelZBias: %f\n\n', boomAccBias(3));

fprintf('\tmagXBias: %f\n', boomMagBias(1));
fprintf('\tmagYBias: %f\n', boomMagBias(2));
fprintf('\tmagZBias: %f\n\n', boomMagBias(3));

fprintf('\tgyroXBias: %f\n', boomGyroBias(1));
fprintf('\tgyroYBias: %f\n', boomGyroBias(2));
fprintf('\tgyroZBias: %f\n\n', boomGyroBias(3));

fprintf('\timuToVehicleRotMatrix:\n[ %f, %f, %f ],\n[ %f, %f, %f ],\n[ %f, %f, %f ]\n',...
        boomR);


%% make plots

figure()
plot_ellipse(cgMagRot);
title('CG IMU Magnetometer Rotating Data With Fit');

figure()
plot_ellipse(cgAccRot);
title('CG IMU Accelerometer Rotating Data With Fit');

figure()
plot_ellipse(boomMagRot);
title('BOOM IMU Magnetometer Rotating Data With Fit');

figure()
plot_ellipse(boomAccRot);
title('BOOM IMU Accelerometer Rotating Data With Fit');
