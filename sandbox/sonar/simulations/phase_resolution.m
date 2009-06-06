function rez = phase_resolution(snr, freq_samp, bits)
amp = 1;
freq = 30000;
% freq_samp = 1000000;
% snr = 1000;
pulse_duration = 1e-03; % in seconds
samp_size = floor(pulse_duration * freq_samp);
freq_samp = freq_samp;
m = 10;
datarange = 2^bits;

phi_stdev = 0;
for j=1:m
  phi = rand() * 2 * pi;
  ping1 = floor(datarange * amp * (sin(2 * pi * freq * (1:samp_size) / freq_samp + phi) + rand(1,samp_size)  / snr));
  ping2 = floor(datarange * amp * (sin(2 * pi * freq * (1:samp_size) / freq_samp + phi) + rand(1,samp_size)  / snr));
  spectrum = fft(ping1);
  phi1 = phase(max(spectrum(2:end)));
  spectrum = fft(ping2);
  phi2 = phase(max(spectrum(2:end)));
  deltaphi = phi2 - phi1;
  phi_stdev = phi_stdev + deltaphi^2;
end
rez = sqrt(phi_stdev / (m - 1));
