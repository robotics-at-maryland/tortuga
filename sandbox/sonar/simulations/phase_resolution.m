amp = 1;
freq = 30000;
freq_samp = 1000000;
snr = 10;
samp_size = 1024;
freq_samp = freq_samp;
samp_size = samp_size;

for i=1:10
phi = rand() * 2 * pi;
phase_difference = 10^-i;
ping1 = amp * (sin(2 * pi * freq * (1:samp_size) / freq_samp + phi) + rand(1,samp_size)  / snr);
ping2 = amp * (sin(2 * pi * freq * (1:samp_size) / freq_samp + phi + phase_difference) + rand(1,samp_size)  / snr);
spectrum = fft(ping1);
phase1 = phase(max(spectrum(2:end)));
spectrum = fft(ping2);
phase2 = phase(max(spectrum(2:end)));
disp('Phase difference:');
disp(phase_difference);
disp(phase2 - phase1);
end
plot(ping1);
