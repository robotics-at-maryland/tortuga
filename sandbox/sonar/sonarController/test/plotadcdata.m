function plotadcdata(filename,nsamp,stride)

config;
num_sensors = 4;
f = fopen(filename, 'rb');
dat = fread(f, [num_sensors, nsamp], 'int8');
fclose(f);
clf;
for i=1:num_sensors
  subplot(num_sensors, 1, i);
  ylabel(sprintf('Channel %d',i-1));
  plot(dat(i,1:stride:nsamp));
end
