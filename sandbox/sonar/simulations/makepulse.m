function adcdata = makepulse(duration, delay)
config;
T_s = 1/f_s;
omega = 2 * pi * f;
t = 0:T_s:duration;

[T,D] = meshgrid(t,delay);
adcdata = signal_amplitude * sin(omega * (T - D));
envelope = (T > D & T < (ping_duration + D));
adcdata = adcdata .* envelope;
adcdata = round(adcdata / adc_amplitude * 2^adc_bits / 2);
