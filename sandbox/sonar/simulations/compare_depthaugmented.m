function compare_depthaugmented(unit_source_pos, hydro_pos, num_trials)
config;
ranges = 1:0.5:20;

uncert_nodepth = zeros(length(ranges));
uncert_depth   = zeros(length(ranges));

hydro_pos_depth = hydro_pos(1:3,:);

for i = 1:length(ranges)
    source_pos = unit_source_pos * ranges(i);
    uncert_nodepth(i) = sqrt(max(eigs(covariance(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy, @localize, num_trials))));
    uncert_depth(i)   = sqrt(max(eigs(covariance_depthaugmented(source_pos, hydro_pos_depth, hydro_pos_accuracy, tdoa_accuracy, depth_accuracy, @localize_depthaugmented, num_trials))));
end

clf
semilogy(ranges,uncert_depth,'b',ranges,uncert_nodepth,'r');
xlabel('Range (meters)');
ylabel('Range uncertainty (meters - log scale)');
title('Depth augmented localizer compared with standard OSLS');
legend('Depth augmented','OSLS');
print -r600 -depsc depthaugmented.eps
