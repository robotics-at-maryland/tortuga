function M = animate_montecarlo(hydro_pos, hydro_pos_accuracy, ...
                                tdoa_accuracy, algorithm, num_trials)
% Determine covariance of x_s, y_s, and z_s, for a particular source
% position and hydrophone configuration, by using the Monte Carlo method.

numturns = 4;
thstep = pi/8;

th = 0:thstep:(numturns*2*pi);
rh = th;

[X Y] = pol2cart(th,rh);

for i=1:length(th)
  source_pos = [X(i),Y(i),0];
  plot_montecarlo(source_pos,hydro_pos,hydro_pos_accuracy, ...  
                  tdoa_accuracy,algorithm,num_trials);
  M(i) = getframe(gcf);
end

movie2avi(M,'montecarlo.avi');
