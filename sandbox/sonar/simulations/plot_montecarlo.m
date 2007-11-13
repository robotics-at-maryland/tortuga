function plot_montecarlo(source_pos, hydro_pos, ...
                                   hydro_pos_accuracy, tdoa_accuracy, ...
                                   algorithm, num_trials, varargin)
% Determine covariance of x_s, y_s, and z_s, for a particular source
% position and hydrophone configuration, by using the Monte Carlo method.

pos = zeros(num_trials,3);

m = size(hydro_pos);
m = m(1);

for i = 1:num_trials
  tdoas = gaussian_variant(source_pos, hydro_pos, hydro_pos_accuracy, ...
                           tdoa_accuracy);
  pos(i,1:3) = feval(algorithm, hydro_pos, tdoas);
end

scaled_hydro_pos = zeros(m+1,3);
scaled_hydro_pos(2:end,:) = hydro_pos * 20;
fov = 30;

av = mean(pos);
hold on;
cla;
axis([-fov, fov, -fov, fov]);
axis square;
plot(pos(:,1),pos(:,2),'.r','MarkerSize',5);
plot(scaled_hydro_pos(:,1),scaled_hydro_pos(:,2),'dk', ...
     'MarkerFaceColor','k','MarkerSize',6);
plot(source_pos(1),source_pos(2),'ob','MarkerFaceColor','b');
plot(av(1),av(2),'or','MarkerFaceColor','r');
hold off;
xlabel('x (meters)');
ylabel('y (meters)');
title('Multilateration solutions: systematic and random errors');
legend('Location estimate','Hydrophone (enlarged)', ...
       'Actual pinger location','Mean estimate');
if length(varargin) == 1 && strcmp(varargin(1),'interactive')
source_pos(1:2) = ginput(1);
if any(abs(source_pos(1:2))>fov)
  return;
end
plot_montecarlo(source_pos,hydro_pos,hydro_pos_accuracy, ...
                tdoa_accuracy,algorithm,num_trials,'interactive');
end
