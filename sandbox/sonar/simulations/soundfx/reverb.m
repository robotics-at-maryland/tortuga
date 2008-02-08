function y = reverb(x, delays, scale_factors)
% REVERB(X, DELAYS, SCALE_FACTORS)
%   Simulate an echo by summing several different versions of X, shifted
%   by different time delays and modulated by different scale factors.
%   x may be an MxN vector, where M is the number of samples, and N is the 
%   number of channels.

y = x;

for i = 1:length(delays)
    y = y + shiftrows(x, delays(i)) * scale_factors(i);
end

