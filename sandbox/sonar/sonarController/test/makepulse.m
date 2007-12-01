function adcdata = makepulse(duration, delay, envtype)
config;
T_s = 1/f_s;
omega = 2 * pi * f;
t = 0:T_s:duration;

[T,D] = meshgrid(t,delay);
adcdata = signal_amplitude * sin(omega * (T - D));

switch tolower(envtype)
 case {'rect'}
  envelope = (T > D & T < (ping_duration + D));
 case {'bump'}
  envelope = (T > D & T < (ping_duration + D)) ...
      .* sin((T-D)/ping_duration*pi).^2;
 otherwise
  error('Invalid envelope type.');
end

adcdata = adcdata .* envelope + signal_amplitude / snr * 2 * (0.5 - ...
                                                  rand(size(T)));
adcdata = round(adcdata / adc_amplitude * 2^adc_bits / 2);
