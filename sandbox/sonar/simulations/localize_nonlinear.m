function pos = localize_nonlinear(hydro_pos, tdoas)
% Find source position using MATLAB's nonlinear minimization algorithms.

hydro_pos = strip_first_zeros(hydro_pos);
m = size(hydro_pos);
m = m(1);


% Produce initial guess using one-step least squares (OSLS) solver
pos_linear = localize(hydro_pos, tdoas);

diffs     = @(v) (hydro_pos - repmat(v,m,1));
ranges_sq = @(v) dot(diffs(v),diffs(v),2);
errs      = @(v) sqrt(ranges_sq(v)) - sqrt(dot(v,v)) - tdoas';
toterr    = @(v) sum(errs(v).^2);

pos = fminsearch(toterr,pos_linear);
