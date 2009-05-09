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

xtotal = -124:2:124;
ytotal = 80:2:120;
DivergeArray = zeros(length(xtotal)*2,length(ytotal)*2); % This defines an x,y position and the value at each x,y is the # of cases it failed        
        
        
figure('name','Divergence Map')
AXIS([-150 150 50 150])
hold on
AXIS square
rectangle ('position', [x0(5,end)-2.5,x0(6,end)-2.5, 5, 5], 'curvature', [1, 1],'LineWidth',2)
rectangle ('position', [x0(7,end)-2.5,x0(8,end)-2.5, 5, 5], 'curvature', [1, 1],'LineWidth',2)
rectangle ('position', [min(xtotal),min(ytotal), max(xtotal)-min(xtotal), max(ytotal)-min(ytotal)],'LineWidth',2)
wbar = waitbar(0,'Plotting the end of humanity...');



color = [1 1 1;  %This uses RGB values to give intensity for the # of failures
         0.7 1 1;  
         0.3 1 1;  
         0 0.5 1;  
         0.3 0.3 1; 
         1 0.9 0.2; 
         1 0.7 0; 
         1 0.5 0; 
         1 0.3 0; 
         1 0 0]; 



for x = -124:2:124
    waitbar((x+max(xtotal))/(2*max(xtotal)),wbar);
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
                    %plot(x,y,'x')
                end

            end
        end
        DivergeArray(x+length(xtotal),y-min(ytotal)+1) = j; %Counts # of diverged pts

        plot(x,y,'s',...
            'MarkerEdgeColor',color(j+1,:),...
            'MarkerFaceColor',color(j+1,:),...
            'MarkerSize',10)

    
    
    
    end
end






rectangle ('position', [x0(5,end)-2.5,x0(6,end)-2.5, 5, 5], 'curvature', [1, 1],'LineWidth',2)
rectangle ('position', [x0(7,end)-2.5,x0(8,end)-2.5, 5, 5], 'curvature', [1, 1],'LineWidth',2)
rectangle ('position', [min(xtotal),min(ytotal), max(xtotal)-min(xtotal), max(ytotal)-min(ytotal)],'LineWidth',2)
close(wbar);
AXIS square
hold off
warning on all;

