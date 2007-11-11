function cov_mat = plot_montecarlo(source_pos, hydro_pos, ...
                                   hydro_pos_accuracy, tdoa_accuracy, ...
                                   algorithm, num_trials)
% Determine covariance of x_s, y_s, and z_s, for a particular source
% position and hydrophone configuration, by using the Monte Carlo method.

pos = zeros(num_trials,3);

for i = 1:num_trials
  tdoas = gaussian_variant(source_pos, hydro_pos, hydro_pos_accuracy, ...
                           tdoa_accuracy);
  pos(i,1:3) = feval(algorithm, hydro_pos, tdoas);
end

scaled_hydro_pos = hydro_pos * 20;
fov = 30;

%cov_mat = cov(pos);
av = mean(pos);
hold on;
cla;
axis([-fov, fov, -fov, fov]);
axis square;
plot(pos(:,1),pos(:,2),'.r','MarkerSize',5);
plot(scaled_hydro_pos(:,1),scaled_hydro_pos(:,2),'dk','MarkerFaceColor','k','MarkerSize',6);
plot(source_pos(1),source_pos(2),'ob','MarkerFaceColor','b');
plot(av(1),av(2),'or','MarkerFaceColor','r');
hold off;
source_pos(1:2) = ginput(1);
if any(abs(source_pos(1:2))>fov)
  return;
end
cov_mat = plot_montecarlo(source_pos,hydro_pos,hydro_pos_accuracy, ...
                          tdoa_accuracy,algorithm,num_trials);
