clc
close all
figure (1)
r = zeros(5,8);
r(:,1) = plotDepthData('./DepthTest_2_17_2009/depth_p/control.log',2,'black');
r(:,2) = plotDepthData('./DepthTest_2_17_2009/depth_obs_low/control.log',2,'r');
r(:,3) = plotDepthData('./DepthTest_2_17_2009/depth_obs_7/control.log',2,'g');
r(:,4) = plotDepthData('./DepthTest_2_17_2009/depth_obs_scale_low/control.log',2,'b');
r(:,5) = plotDepthData('./DepthTest_2_17_2009/depth_obs_scale_7/control.log',2,'m');
subplot(2,1,1);
hold on
plot([0,60],[5,5],'black')
hold off
axis([0 60 2 7])
title('Observer Controllers')
ylabel('Depth Units')
xlabel('Time (s)')
subplot(2,1,2)
legend('P Controller','Continous Observer Controller (lowgains)','Continous Observer Controller (K=0.7,L=7)','Continous Observer Controller With Scaling (lowgains)','Continous Observer Controller With Scaling(K=0.7,L=7)')


figure(2)
r(:,1) = plotDepthData('./DepthTest_2_17_2009/depth_p/control.log',2,'black');
r(:,6) = plotDepthData('./DepthTest_2_17_2009/depth_pid_1/control.log',2,'red');
r(:,7) = plotDepthData('./DepthTest_2_17_2009/depth_pid_2/control.log',2,'green');
r(:,8) = plotDepthData('./DepthTest_2_17_2009/depth_pid_3/control.log',2,'blue');
r(:,9) = plotDepthData('./DepthTest_2_17_2009/depth_pid_4/control.log',2,'magenta');
r(:,10) = plotDepthData('./DepthTest_2_17_2009/depth_pid_5/control.log',2,'cyan');
r(:,11) = plotDepthData('./DepthTest_2_17_2009/depth_pid_6/control.log',2,'yellow');
r(:,12) = plotDepthData('./DepthTest_2_17_2009/depth_pid_7/control.log',2,'black');
subplot(2,1,1);
hold on
plot([0,60],[5,5],'black')
hold off
axis([0 60 2 6])
title('PID Controllers')
ylabel('Depth Units')
xlabel('Time (s)')
subplot(2,1,2)
legend('P Controller','PID Controller (1)','PID Controller (2)','PID Controller (3)','PID Controller (4)','PID Controller (5)','PID Controller (6)','PID Controller (7)');

r = r'
%'steady state rise time settling time overshoot sserror';
save depthData.txt r -ASCII