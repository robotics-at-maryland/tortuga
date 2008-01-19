function adcdata = makedataset(filename, envtype, duration)
% adcdata = MAKEDATASET(filename, envtype, duration)
% 
% Output simulated acoustic data for a SONAR array.
% Physical and ADC parameters are stored in config.m.
% 
%   Input arguments:
%     filename  Write output to a binary file with this path
%     envtype   Envelope form.
%               'bump' for a smooth cosine-shaped envelope
%               'rect' for a rectangular or square-wave envelope
%     duration  Amount of acoustic data to generate, in seconds
%
%    Output arguments:
%      adcdata  A matrix containing the ADC data

config;

numsamples = round(duration*f_s);
adcdata = zeros(nchannels,numsamples);

for tping = tfirstping:pingrate:duration
  startindex = round(tping * f_s);
  oneping = getOnePing(tping, min(duration-tping,ping_duration), envtype);
  m = size(oneping);
  adcdata(:,startindex:(startindex+m(2)-1)) = oneping;
end

clear('oneping');

adcdata = adcdata(:,1:numsamples);
adcdata = adcdata + signal_amplitude / snr * 2 * (0.5-rand(size(adcdata)));
% warning: if the noise level or signal level is set too high, then
% the following operation could overflow
adcdata = int8(round((adcdata) / adc_amplitude * 2^adc_bits));

f = fopen(filename, 'wb');
fwrite(f, adcdata, 'int8');
fclose(f);

end

function adcdata = getOnePing(tping, duration, envtype)
config;
omega = 2 * pi * f;

% Cause the robot to wait for (trainingtime) seconds and follow a
% "spira mirabilis," a logarithmic spiral in the plane z = 0,
% parameterized for constant speed.  A logarithmic spiral has the
% special property that the angle formed between the tangent line
% (the velocity) and the raidal line from the origin is constant, 
% and equal to arctan(1/b).
v = max_speed;
A = max_range;
b = tan(bearing);
T_s = 1/f_s;
t = tping + (0:T_s:duration);
tau = - log(1 - b * (t - trainingtime) * v / A ./ sqrt(b^2 + 1)) / b;
x = A * cos(tau) .* exp(-b * tau) .* (t > trainingtime) + (t <= trainingtime) * A;
y = A * sin(tau) .* exp(-b * tau) .* (t > trainingtime);
z = 0 * t;

hydroRotationMatrix = [cos(bearing),sin(bearing),0;-sin(bearing),cos(bearing),0;0,0,1];
hydro_pos = zeros(nchannels,3);
hydro_pos(2:end,:) = hydroRotationMatrix * baseline;
hydro_ranges=repmat([shiftdim(x,-1),shiftdim(y,-1),shiftdim(z,-1)],[nchannels,1,1]) ...
    + repmat(hydro_pos, [1,1,length(t)]);
hydro_ranges = squeeze(sqrt(dot(hydro_ranges, hydro_ranges, 2)));

trelping = repmat(t,nchannels,1) - hydro_ranges / c - tping;

switch lower(envtype)
 case {'rect'}
  envelope = (trelping > 0 & trelping < ping_duration);
 case {'bump'}
  envelope = (trelping > 0 & trelping < ping_duration) ...
      .* sin(trelping/ping_duration*pi).^2;
 otherwise
  error('Invalid envelope type.');
end

phase = (0.5-rand())*2*pi;

adcdata = signal_amplitude * sin(omega * trelping + phase) .* envelope;
end
