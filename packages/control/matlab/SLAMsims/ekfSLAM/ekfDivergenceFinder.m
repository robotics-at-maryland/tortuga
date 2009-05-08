clc;
close all;
clear all;
warning off all;

plotData = 0;
divergence_criteria = 10;

%covariance of process noise (arbitrarily chosen)
%Rv_cont=1e-112*diag([2 2]);
Rv_cont=1e-2*diag([2 2]);%in N   CONTINUOUS TIME, needs discretization
%Rv_cont=1e-8*diag([8 8]);

        %true initial state
        x0 =[0; %x
            0; %y
            0; %x_dot
            0; %y_dot
            -50; %x1
            100; %y1
            50; %x2
            100]; %y2

figure('name','Divergence Map')
AXIS([-100 100 50 150])
hold on
plot(x0(5,end),x0(6,end),'ro')
plot(x0(7,end),x0(8,end),'ro')
wbar = waitbar(0,'Plotting the end of humanity...');

xtotal = -70:2:70;
ytotal = 80:2:120;
DivergeArray = zeros(length(xtotal)*2,length(ytotal)*2); % This defines an x,y position and the value at each x,y is the # of cases it failed
size(DivergeArray)


for x = -70:2:70
    waitbar((x+70)/140,wbar);
    for y = 80:2:120
        %true initial state
        x0 =[x; %x
            y; %y
            0; %x_dot
            0; %y_dot
            -50; %x1
            100; %y1
            50; %x2
            100]; %y2
            j=0;

        for guess = -.01:.01:.01
            for i = 1:3 %Because we have random forces we run this 3 times
                %initial state estimate
                xhat0 = [ x-x*guess;  % x
                    y-y*guess;  % y
                    0;  % x_dot
                    0;  % y_dot
                    -50;  % x1  (Pinger 1)
                    100;  % y1  (Pinger 1)
                    50;   % x2 (Pinger 2)
                    100];% y2 (Pinger 2)
                Divergence = ekfSLAMFunc(xhat0,x0,plotData,Rv_cont,divergence_criteria);
                if Divergence == 1
                    j = j+1;
                    DivergeArray(x+length(xtotal),y-min(ytotal)+1) = j; %Counts # of diverged pts
                    %plot(x,y,'x')                 
                end
                
            end
        end
    end
end

close(wbar);
hold off
warning on all;

