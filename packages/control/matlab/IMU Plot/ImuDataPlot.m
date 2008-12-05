clc;
clear;
close all

imu_log = load('no_desired_change/imu.log');

control_log = load('no_desired_change/control.log');

% mag fields are columns 4-6 from img_log
mag = imu_log(:,4:6);
% mag_raw fields are columns 10-12 from imu_log
mag_raw = imu_log(:,10:12);
% accel fields are column 1-3 from imu_log
accel = imu_log(:,1:3);

% time is the last field from imu_log (divide by 1000 to get sec from ms)
time = (imu_log(:,end) - imu_log(1,end))/1000;

% time2 is the last field from conrol_log
time2 = (control_log(:,end) - control_log(1,end))/1000;

% rot torq fields are columns 18-20 from control_log
rot_torq = control_log(:,18:20);

% rotation matrix to transform mag_raw to vehicle's frame
imuToVehicleRotation=[0 0 1; -1 0 0; 0 -1 0];

for i = 1 :length(mag)
   % transform mag_raw to vehicle's frame
   temp=imuToVehicleRotation*mag_raw(i,:)'-[-0.269084; 0.221182; 0.243998];
   %temp=imuToVehicleRotation*mag_raw(i,:)';
   mag_raw(i,:)=temp';
   
   % find norm of each mag reading
   mag_norm(i) = norm(mag(i,:),2);
   mag_raw_norm(i) = norm(mag_raw(i,:),2);
   % find angle b/w accel(grav) and mag
   theta(i) = acos(dot(mag(i,:),accel(i,:))/(norm(mag(i,:))*norm(accel(i,:))));
end

for j = 1:length(rot_torq)
    % find magnitude of torq vector
    torq_mag(j) = norm(rot_torq(j,:));
end

% plots mag and mag_raw in 3 frames
figure(1)
subplot(3,1,1); plot(time,mag(:,1),time,mag_raw(:,1)); 
axis tight;
legend('mag','mag-raw');
ylabel('mag_1')
subplot(3,1,2); plot(time,mag(:,2),time,mag_raw(:,2));
axis tight
ylabel('mag_2')
subplot(3,1,3); plot(time,mag(:,3),time,mag_raw(:,3)); 
axis tight
ylabel('mag_3')
xlabel('time(s)')

% plots norm of mag and mag_raw
figure(2)
plot(time,mag_norm,time,mag_raw_norm)
legend('mag','mag-raw');
axis tight
ylabel('Mag Norm')
xlabel('time(s)')

% plots angle b/w accel and mag vectors
figure(3)
subplot(2,1,1); plot(time2,torq_mag);
ylabel('rot torq magnitude')
subplot(2,1,2); plot(time,theta)
axis([0 time(end) 0 pi/4])
ylabel('theta(radians)')
xlabel('time(s)')