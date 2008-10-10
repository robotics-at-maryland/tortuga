function DepthDataPlot()

close all
figure (1)
hold on
plotDepthData('pc-2-5.log',947,'black');
plotDepthData('contOC-lowgains-2-5.log',445,'r');
plotDepthData('disOC-lowgains-2-5.log',860,'g');
plotDepthData('disOC-L15-2-5.log',300,'b');
plot(1:0.1:35,5)
legend('P Controller','Continous Observer Controller (lowgains)','Discrete Observer Controller (lowgains)','Discrete Observer Controller (K=2,L=15)')
hold off;

figure(2)
hold on
plotDepthData('pc-2-5.log',947,'black');
plotDepthData('pid1-2-5.log',1244,'g');
plotDepthData('pid4-2-5.log',708','b');
plotDepthData('pid5-2-5.log',514,'m');
plotDepthData('pid6-2-5.log',422,'r');
plot(1:0.1:18,5)
legend('P Controller','PID Controller (10,0.2,4)','PID Controller (4)','PID Controller (5)','PID Controller (6)');
hold off

function plotDepthData(file,offset,color)

clc;

a = load (file);

% Measured Depth field is column 8
depth_m = a(offset:end,8);

% Desired Depth field is column 16
depth_d = a(offset:end,16);

t = a(offset:end,end)/1000;
t = t - t(1)*ones(1:length(t),1);

% plots measured and desired depth
plot(t,depth_m,color);
set(gca,'YDir','reverse')