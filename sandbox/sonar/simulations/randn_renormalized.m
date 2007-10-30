function M = randn_renormalizedZ(mu, sigma, siz)
% RANDN_RENORMALIZED
%  randn_renormalized(mu, sigma, siz) returns a gaussian-distributed
%  random matrix with mean mu, standard deviation sigma, and dimensions
%  given by siz.
M = randn(siz) * sigma + mu;
