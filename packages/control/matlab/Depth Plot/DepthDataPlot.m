function DepthDataPlot()
clc
close all
figure (1)
hold on
r = zeros(5,8);
r(:,1) = plotDepthData('pc-2-5.log',947,'black');
r(:,2) = plotDepthData('contOC-lowgains-2-5.log',445,'r');
r(:,3) = plotDepthData('disOC-lowgains-2-5.log',860,'g');
r(:,4) = plotDepthData('disOC-L15-2-5.log',300,'b');
plot(0:0.05:35,5)
title('Observer Controllers')
ylabel('Depth Units')
xlabel('Time (s)')
legend('P Controller','Continous Observer Controller (lowgains)','Discrete Observer Controller (lowgains)','Discrete Observer Controller (K=2,L=15)')
hold off;

figure(2)
hold on
plotDepthData('pc-2-5.log',947,'black');
r(:,5) = plotDepthData('pid1-2-5.log',1244,'g');
r(:,6) = plotDepthData('pid4-2-5.log',708','b');
r(:,7) = plotDepthData('pid5-2-5.log',514,'m');
r(:,8) = plotDepthData('pid6-2-5.log',422,'r');
plot(0:0.05:18,5)
title('PID Controllers')
ylabel('Depth Units')
xlabel('Time (s)')
legend('P Controller','PID Controller (10,0.2,4)','PID Controller (4)','PID Controller (5)','PID Controller (6)');
hold off

r = r'
%'steady state rise time settling time overshoot sserror';
save depthData.txt r -ASCII

function [result] = plotDepthData(file,offset,color)

a = load (file);

% Measured Depth field is column 8
depth_m = a(offset+4:end,8);
depth_a = 1/5*(a(offset:end-4,8)+a(offset+1:end-3,8)+a(offset+2:end-2,8)+a(offset+3:end-1,8)+a(offset+4:end,8));
depth_d = a(end,16);

% time
t = a(offset+4:end,end)/1000;
t = t - t(1)*ones(1:length(t),1);

% steady state start point
i = round(length(t)*0.93);

result = zeros(1,5);
[result(1) result(2) result(3) result(4) result(5)] = FindStepInfo(t,depth_m,i,depth_d,0.02);

% plots measured and desired depth
plot(t,depth_a,color);
set(gca,'YDir','reverse')
%plot(t(i),depth_a(i),'*')


function [steady_state,rise_time,settling_time,overshoot,sserror] = FindStepInfo(t,depth_a,i,depth_d,settlingThreshold)

steady_state = mean(depth_a(i:end));
sserror = abs(depth_d-steady_state);
overshoot = (max(depth_a) - depth_d)/depth_d;

for j = length(depth_a):-1:1
    if (abs(depth_a(j) - steady_state) > abs(settlingThreshold*steady_state))
        settling_time = t(j)-t(1);
        break;
    end
end

for k = 1:length(depth_a)
   if (depth_a(k) > 2)
        break;
   end
end

minRiseTime = interp1(depth_a(k-1:k),t(k-1:k),2);

for l = k:length(depth_a)
    if (depth_a(l) > depth_d)
        break;
    end
end
maxRiseTime = interp1(depth_a(l-1:l),t(l-1:l),depth_d);
rise_time = maxRiseTime - minRiseTime;

%stepinfo(depth_m,t,5,'SettlingTimeThreshold',0.02)