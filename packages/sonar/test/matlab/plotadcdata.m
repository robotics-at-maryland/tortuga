function plotadcdata(filename,nsamp,stride)
% PLOTADCDATA(filename, nsamp, stride
%   Plot ADC data records stored to a file
%
%   Input arguments:
%     filename  The name of the file where the data is stored
%     nsamp     The number of samples to plot
%     stride    Number of samples to skip between plot points

config;
f = fopen(filename, 'rb');
dat = fread(f, [nchannels, nsamp], 'int8');
fclose(f);
clf;
for i=1:nchannels
  subplot(nchannels, 1, i);
  plot(dat(i,1:stride:nsamp));
  ylabel(sprintf('Channel %d',i-1));
end
