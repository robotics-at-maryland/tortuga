function DepthDataPlot()
clc
close all
figure (1)
hold on
r = zeros(5,8);
r(:,1) = plotDepthData('pc-2-5.log',2,'black');
r(:,2) = plotDepthData('contOC-lowgains-2-5.log',2,'r');
r(:,3) = plotDepthData('disOC-lowgains-2-5.log',2,'g');
r(:,4) = plotDepthData('disOC-L15-2-5.log',2,'b');
plot(0:0.05:35,5)
title('Observer Controllers')
ylabel('Depth Units')
xlabel('Time (s)')
legend('P Controller','Continous Observer Controller (lowgains)','Discrete Observer Controller (lowgains)','Discrete Observer Controller (K=2,L=15)')
hold off;

figure(2)
hold on
plotDepthData('pc-2-5.log',2,'black');
r(:,5) = plotDepthData('pid1-2-5.log',2,'g');
r(:,6) = plotDepthData('pid4-2-5.log',2','b');
r(:,7) = plotDepthData('pid5-2-5.log',2,'m');
r(:,8) = plotDepthData('pid6-2-5.log',2,'r');
plot(0:0.05:18,5)
title('PID Controllers')
ylabel('Depth Units')
xlabel('Time (s)')
legend('P Controller','PID Controller (10,0.2,4)','PID Controller (4)','PID Controller (5)','PID Controller (6)');
hold off

r = r'
%'steady state rise time settling time overshoot sserror';
save depthData.txt r -ASCII