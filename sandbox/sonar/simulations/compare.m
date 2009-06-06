function compare(unit_source_pos, hydro_pos, num_trials)
config;
ranges = 1:0.5:20;

algs = {@localize,@localize_simpleiterative,@localize_iterative, ...
        @localize_nonlinear};

nalgs = length(algs);

cramerb = zeros(length(ranges),1);
uncert = zeros(length(ranges),length(algs));

hydro_pos_depth = hydro_pos(1:3,:);

for i = 1:length(ranges)
    source_pos = unit_source_pos * ranges(i);
    cramerb(i) = sqrt(max(eigs(crlb(source_pos,hydro_pos,hydro_pos_accuracy, ...
                 tdoa_accuracy))));
    for j = 1:nalgs
      uncert(i,j) = avgrangeerror(source_pos, hydro_pos, ...
                   hydro_pos_accuracy,tdoa_accuracy,depth_accuracy,algs{j},num_trials);
    end
end

clf
semilogy(ranges,cramerb,ranges,uncert(:,1),ranges,uncert(:,2),ranges,uncert(:,3),ranges,uncert(:,4));
h = legend('CRLB',func2str(algs{1}),func2str(algs{2}),func2str(algs{3}),func2str(algs{4}));
set(h, 'Interpreter', 'none');
xlabel('Range (meters)');
ylabel('Range std. dev. (meters from real pinger - log scale)');
title('Range accuracy of multilateration solvers compared');
print -r600 -depsc compare.eps
