% Free rise analysis
close all
load free_rise.txt

depth = free_rise(4480:6020,5);

%data taken at 100 Hz

time=1:1:length(depth);
time = time/100;
%plot(time,depth, 'r');
hold on;

%Used to find least square error
%This value just adds all the error of each measurement
val_off = 0;
final = 1000000;%smallest error off by, final will change if
% val_off if smaller than final

%modify the range for c,m,d0 in order to come up
%with a best fit for these values
for c = 11.5%11:.1:12
    
    for m = 20
        for d0 = -5.8%-5:-.1:-6.5
            

            A = [0,1;0,-c/m];
            B = [0;1/m];

            x = [depth(1);0];

            xdot = A*x + B*d0;
            y = 2;

            while y < length(depth)
                x(:,end+1) = x(:,end) + xdot(:,end)*(time(y)-time(y-1));
                xdot(:,end+1) = A*x(:,end) + B*d0;
                val_off = val_off + abs(depth(y)-x(1,y));%error of each point
                y=y+1;
            end
            if(val_off < final)%update final if val_off is smaller
                final = val_off;
                c1 = c;%best c value
                m1 = m;%best m value
                d01 = d0;%best d0 value
            end
            val_off = 0;
            plot(time(1:end-1), x(1,:));
            hold on;
        end
    end
end
plot(time,depth, 'r');
c1
m1
d01
xlabel('Time')
ylabel('Depth')
legend('Measured','Modeled');
set(gca,'YDir','reverse')
title('Free Response')
    %x;

    %plot(time(1:end-1), x(1,:));
    %plot(time, xdot);