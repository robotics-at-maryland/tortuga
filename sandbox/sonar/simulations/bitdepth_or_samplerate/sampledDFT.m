function phase = sampledDFT(f, fs, T, bitDepth, timeDelay)
dt = 1 / fs;
t = 0:dt:T;
y = fix(sin(2 * pi * f * (t - timeDelay)) * 2^(bitDepth-1)) / (2^(bitDepth-1));
dft = fft(y);
amp = dft(f/fs*(length(t)-1));
phase = atan2(imag(amp),real(amp));
