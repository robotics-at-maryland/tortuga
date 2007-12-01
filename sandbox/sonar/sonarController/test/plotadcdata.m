function plotadcdata(filename)
config;
num_sensors = 5;
f = fopen(filename, 'rb');
dat = fread(f, [num_sensors, Inf], 'int8');
fclose(f);
nsamp = length(dat) / num_sensors;
clf;
for i=1:num_sensors
  subplot(num_sensors, 1, i);
  ylabel(sprintf('Channel %d',i-1));
  axis('tic[x]');
  plot(dat(i,:));
end
