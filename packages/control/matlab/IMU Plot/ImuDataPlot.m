clc;
clear;

load imu_log.txt

% mag fields are columns 4-6 from log
mag = imu_log(:,4:6);
% mag_raw fields are columns 10-12 from log
mag_raw = imu_log(:,10:12);

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
end

t = 1:length(mag);

% plots mag and mag_raw in 3 frames
figure(1)
subplot(3,1,1); plot(t,mag(:,1),t,mag_raw(:,1)); 
axis tight;
legend('mag','mag-raw');
ylabel('mag_1')
subplot(3,1,2); plot(t,mag(:,2),t,mag_raw(:,2));
axis tight
ylabel('mag_2')
subplot(3,1,3); plot(t,mag(:,3),t,mag_raw(:,3)); 
axis tight
ylabel('mag_3')

% plots norm of mag and mag_raw
figure(2)
plot(t,mag_norm,t,mag_raw_norm)
legend('mag','mag-raw');
axis tight
ylabel('Mag Norm')