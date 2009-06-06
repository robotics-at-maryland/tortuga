%  a   and d are named so because they are unused variables in the graphing
%  process
[       a              time_low_freq    depth_measured_low_freq     estimated_x_low_freq  ] = DepthEstimationSamplingRateComparison(60);
[true_measured_depth   time_high_freq   depth_measured_high_freq    estimated_x_high_freq ] = DepthEstimationSamplingRateComparison(1000);


figure (1)
%One subplotshould show actual depth, low frequency obv. estimated depth, and high frequency observer estimated depth all vs. time.




% Of course, please include a legend to show which line means which.  It
% would probably make the most sense to plot actual as a line, high
% frequency observer estimates as ':', and low frequency estimates as
% '*', but do what you think looks best.
% 
 subplot(3,1,1)
 plot(time_high_freq,true_measured_depth(1,:),'*r',time_low_freq,estimated_x_low_freq(1,:),'g',time_high_freq,estimated_x_high_freq(1,:),':b','LineWidth',1)
 set(gca,'YDir','reverse')
 legend('Actual','Low-freq OC','High-freq OC')
 title('Positions')
 ylabel('x1 - depth')
 xlabel('time (sec)')





%the other subplot should show actual depth vel, low frequency observer estimated depth vel, and high frequency observer estimated depth vel
%again with time on the horizontal axis.
%x_hat_array(2,:)
subplot(3,1,2)
plot(time_high_freq,true_measured_depth(2,:),'*r',time_low_freq,estimated_x_low_freq(2,:) ,'g',time_high_freq,estimated_x_high_freq,':b','LineWidth',1)
set(gca,'YDir','reverse')
legend('Actual','Low_freq OC','High_freq OC')
title('Velocities')
ylabel('x1 - depth vel')
xlabel('time (sec)')


subplot(3,1,3)
plot(time_high_freq,true_measured_depth(2,:),'*r',time_low_freq,estimated_x_low_freq(2,:),'g','LineWidth',1)
set(gca,'YDir','reverse')
legend('Actual','Low_freq OC')
title('Velocities without High_freq')
ylabel('x1 - depth vel')
xlabel('time (sec)')