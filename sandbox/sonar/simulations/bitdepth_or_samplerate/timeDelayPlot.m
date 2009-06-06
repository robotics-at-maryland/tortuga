function timeDelayPlot(f, fs, T, bitDepth)
period = 1 / f;
dTimeDelay = 10^-6;
timeDelays = 0:dTimeDelay:period;
for i = 1:length(timeDelays)
	phase(i) = sampledDFT(f, fs, T, bitDepth, timeDelays(i));
end
plot(timeDelays(1:10), phase(1:10)/2/pi/f);
