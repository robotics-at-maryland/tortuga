function mags = slidingdft(data, targetfreq, samprate)
% SLIDINGDFT Evaluate a single band sliding DFT at a single frequency
%
%   USAGE:
%   magsq = slidingdft(data, targetfreq, samprate);
%     data is the time series
%     targetfreq is the desired frequency in Hertz
%     samprate is the sampling frequency in Hertz
%     magsq is a time series containing the magnitude squared of
%       the DFT of the input data

nearestperiod = round(samprate/targetfreq);
numperiods = 6;
windowlength = nearestperiod*numperiods;

% circular buffers for recording terms of the DFT
windowreal = zeros(1,windowlength);
windowimag = zeros(1,windowlength);

% pre-evaluated DFT coefficients
windowvals = 0:(windowlength-1);
cosines = cos(windowvals/nearestperiod*2*pi);
sines = sin(windowvals/nearestperiod*2*pi);

% real and imaginary parts of the DFT
sumreal = 0;
sumimag = 0;

curidx = 1;
for i=1:length(data)
  curidx = curidx + 1;
  if curidx > windowlength
    curidx = 1;
  end
  startidx = curidx + 1;
  if startidx > windowlength
    startidx = 1;
  end
  windowreal(curidx) = cosines(curidx)*data(i);
  windowimag(curidx) = sines(curidx)*data(i);
  sumreal = sumreal + windowreal(curidx) - windowreal(startidx);
  sumimag = sumimag + windowimag(curidx) - windowimag(startidx);
  mags(i) = sumreal ^ 2 + sumimag ^2;
end
