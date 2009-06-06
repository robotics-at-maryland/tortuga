function PlotTestValues(storedData, color)

%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%plot trajectories%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%

%quaternion
figure(1)
subplot(4,1,1); hold on
plot(storedData.time,storedData.quaternion(1,:), color); hold off
ylabel('q_1')
subplot(4,1,2); hold on
plot(storedData.time,storedData.quaternion(2,:), color); hold off
ylabel('q_2')
subplot(4,1,3); hold on
plot(storedData.time,storedData.quaternion(3,:), color); hold off
ylabel('q_3')
subplot(4,1,4); hold on
plot(storedData.time,storedData.quaternion(4,:), color); hold off
ylabel('q_4')
xlabel('time (s)')

%angular rate
figure(2)
subplot(3,1,1); hold on
plot(storedData.time,storedData.angularRate(1,:), color); hold off
ylabel('\omega_1 (rad/s)')
subplot(3,1,2); hold on
plot(storedData.time,storedData.angularRate(2,:), color); hold off
ylabel('\omega_2 (rad/s)')
subplot(3,1,3); hold on
plot(storedData.time,storedData.angularRate(3,:), color); hold off
ylabel('\omega_3 (rad/s)')
xlabel('time (s)')

% %estimated parameters
% figure(3)
% subplot(3,2,1); hold on
% plot(storedData.time,storedData.estimatedParameters(1,:), color); hold off
% ylabel('a_1')
% subplot(3,2,2); hold on
% plot(storedData.time,storedData.estimatedParameters(2,:), color); hold off
% ylabel('a_2')
% subplot(3,2,3); hold on
% plot(storedData.time,storedData.estimatedParameters(3,:), color); hold off
% ylabel('a_3')
% subplot(3,2,4); hold on
% plot(storedData.time,storedData.estimatedParameters(4,:), color); hold off
% ylabel('a_4')
% subplot(3,2,5); hold on
% plot(storedData.time,storedData.estimatedParameters(5,:), color); hold off
% ylabel('a_5')
% xlabel('time (s)')
% subplot(3,2,6)
% plot(storedData.time,storedData.estimatedParameters(6,:), color); hold off
% ylabel('a_6')
% xlabel('time (s)')
% 
% %roll pitch yaw
% rollPitchYaw=zeros(3,length(storedData.time));
% for index=1:1:length(storedData.time)
%     rollPitchYaw(:,index)=(180/pi)*rollPitchYawFromQuaternion(storedData.quaternion(:,index));
% end
% 
% figure(5)
% subplot(3,1,1); hold on
% plot(storedData.time,rollPitchYaw(1,:), color); hold off
% ylabel('roll (\phi) in deg')
% subplot(3,1,2); hold on
% plot(storedData.time,rollPitchYaw(2,:), color); hold off
% ylabel('pitch (\theta) in deg')
% subplot(3,1,3); hold on
% plot(storedData.time,rollPitchYaw(3,:), color); hold off
% ylabel('yaw (\psi) in deg')
% xlabel('time (s)')