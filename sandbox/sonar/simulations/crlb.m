function pos_error = crlb(source_pos, hydro_pos)
% Acoustic Properties %

snr  = 10000;   % signal to noise ratio, unitless
freq = 30000;  % pinger frequency, in hertz
vs   = 1400;   % speed of sound in water, in meters per second

% DSP Properties %

freq_sample = 1000000; % sample rate, in samples per second
sample_size = 1024; % length of sample
phase_rez = 0.01; % phase resolution in radians

% Physical Properties

lambda = vs / freq;

% There is always a hydrophone at (0,0,0), but it is not listed.
% hydro_pos = [
%     0, sqrt(3)/3, sqrt(2/3);
%     0.5, -sqrt(3)/6, sqrt(2/3);
%     -0.5, -sqrt(3)/6, sqrt(2/3);
%     0, 0, 2*sqrt(2/3)
% ] * lambda;
hydro_pos = hydro_pos * lambda;

% source_pos = [1,1,1]; % Rectangular coordinate of pinger, in meters

sigma_s = 0.0000635;  % Machining tolerance of hydrophone mounts, in meters
% 0.0000635 is 2.5 mils (thousandths of an inch)



%%%%%%%%%%%%%%%%%%%%%%%%%
% Begin simulation here %
%%%%%%%%%%%%%%%%%%%%%%%%%

m = size(hydro_pos);
m = m(1);

% Basic definitions %

hydro_to_source_vec = hydro_pos - repmat(source_pos, m, 1);
dists = sqrt(dot(hydro_to_source_vec, hydro_to_source_vec, 2));
dist0 = sqrt(dot(source_pos, source_pos));
ddoas = dists - dist0;
sigma_x = phase_rez/2/pi * lambda;

% Computing the Jacobian matrix %
% d(d10,d20,...,dm0,x1,y1,z1,x2,y2,z2,...,xm,ym,zm)
% -------------------------------------------------
%    d(xs,ys,zs,x1,y1,z1,x2,y2,z2,...,xm,ym,zm)

deriv11 = (repmat(source_pos,m,1)-hydro_pos)./repmat(dists,1,3)+repmat(source_pos,m,1)/dist0;

deriv12 = -(repmat(source_pos,m,1)-hydro_pos)./repmat(dists,1,3);
deriv12(:,4:((m+1)*3)) = 0;
deriv12 = reshape(transpose(deriv12),prod(size(deriv12)),1);
deriv12 = transpose(reshape(deriv12(1:(end-m*3)),3*m,m));

deriv21 = zeros(3*m,3);

deriv22 = eye(3*m);

deriv = zeros(4*m,3*(m+1));
deriv(1:m,1:3) = deriv11;
deriv(1:m,4:end) = deriv12;
deriv(m+1:end,1:3) = deriv21;
deriv(m+1:end,4:end) = deriv22;

% Computing the covariance matrix %

cov = zeros(4*m,4*m);
cov(1:m,1:m) = eye(m) * sigma_x^2 + (1 - eye(m)) * 0.5 * sigma_x^2;
cov(m+1:end,m+1:end) = eye(3*m) * sigma_s^2;

% Computing the Cramer-Rao lower bound

cr_lowerbound = inv(transpose(deriv)*inv(cov)*deriv);
subcr = cr_lowerbound(1:3,1:3);
pos_error = sqrt(dot(diag(subcr),diag(subcr)));
