function cov_mat = covariance(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy, num_trials)
% Determine covariance of x_s, y_s, and z_s, for a particular source
% position and hydrophone configuration, by using the Monte Carlo method.

pos = zeros(num_trials,3);

for i = 1:num_trials
  pos(i,1:3) = gaussian_variant(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy);
end

cov_mat = cov(pos);
