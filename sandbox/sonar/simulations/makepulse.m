function adcdata = makepulse(duration, delay)
config;
T_s = 1/f_s;
t = 0:T_s:duration;
omega = 2 * pi * f;
adcdata = signal_amplitude * sin(omega * (t - delay));
envelope = (t > delay & t < (ping_duration + delay));
adcdata = adcdata .* envelope;
adcdata = round(adcdata / adc_amplitude * 2^adc_bits);
