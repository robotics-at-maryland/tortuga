function makeadcdata(filename, envtype)
config;
num_sensors = 5;
dur = 0.003;
delayrange = max_sensor_separation / c;
delays = delayrange*rand(1,num_sensors);

dat = makepulse(dur, delays, envtype);
f = fopen(filename, 'wb');
s = size(dat);
fwrite(f, reshape(dat, 1, s(1)*s(2)), 'int8');
fclose(f);

save(strcat(filename,'.delays'),'delays');
