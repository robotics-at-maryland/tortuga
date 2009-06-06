function y = reverb(x, delays, scale_factors)
% REVERB(X, DELAYS, SCALE_FACTORS)
%   Simulate an echo by summing several different versions of X, shifted
%   by different time delays and modulated by different scale factors.
%   x may be an MxN vector, where M is the number of samples, and N is the 
%   number of channels.
%
%   Example:
%     >> fs = 100;             % sampling rate of 100 Hz
%     >> dt = 1/fs;            % sample period of 0.01 seconds
%     >> t  = 0:dt:10;         % 10 second time period
%     >> f  = 2;               % signal frequency 2 Hz
%     >> y  = sin(2*pi*f*t);   % generate a sine wave
%     >> y  = (t > 1 & t < 3) .* y;  % add a rectangular envelope
%     >> delays = [0,1.5,4] * dt;    % original signal + two echoes
%     >> scale_factors = [1,0.5,0.1] % decreasing in amplitude
%     >> yWithReverb = reverb(y,delays,scale_factors);

y = x;

for i = 1:length(delays)
    y = y + shiftrows(x, delays(i)) * scale_factors(i);
end
