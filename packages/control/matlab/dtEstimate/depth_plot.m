% Free rise analysis
close all
load control_log_gui.txt

dt = control_log_gui(:,5);
time = [0.0001 .01 .05 .2 .5 .75 1 1.3 1.8 2];
dt(1) = time(1);
for x = 2:1:(length(time))
    dt(x) = time(x) - time(x-1);
end
min = 10.00;
max = 0.00;
mean = 0;

time=1:1:length(dt);
dt

for x = 1:1:length(time)
    if(dt(x) < min)
        min = dt(x);
    end
    if(dt(x) > max)
        max = dt(x);
    end
end

avg = MEAN(dt)
min
max
%min = MIN(dt)
%max = MAX(dt)

    

plot(time,dt);
%hold on;
%plot(time,depth2);
%plot(time,depth3);
%plot(time,depth4);
%plot(time,depth5);

%plot(time(1:end-1), x(1,:));
%plot(time, xdot);