function e = avgrangeerror(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy, depth_accuracy, algorithm, num_trials)
% Determine covariance of x_s, y_s, and z_s, for a particular source
% position and hydrophone configuration, by using the Monte Carlo method.

pos = zeros(num_trials,3);

for i = 1:num_trials
  [tdoas,zs] = gaussian_variant_depthaugmented(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy, depth_accuracy);
  pos(i,1:3) = feval(algorithm, hydro_pos, tdoas, zs);
end

e = mean(sqrt(sum((pos - repmat(source_pos,num_trials,1)).^2,2)));
