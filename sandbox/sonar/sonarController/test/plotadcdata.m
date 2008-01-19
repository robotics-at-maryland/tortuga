function plotadcdata(filename,nsamp,stride)

config;
f = fopen(filename, 'rb');
dat = fread(f, [nchannels, nsamp], 'int8');
fclose(f);
clf;
for i=1:nchannels
  subplot(nchannels, 1, i);
  ylabel(sprintf('Channel %d',i-1));
  plot(dat(i,1:stride:nsamp));
end
