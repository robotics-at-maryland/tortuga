clc;
close all;
clear all;
warning off all;

plotData = 0;
divergence_criteria = 10;

%covariance of process noise (arbitrarily chosen)
%Rv_cont=1e-112*diag([2 2]);
Rv_cont=1e-4*diag([2 2]);%in N   CONTINUOUS TIME, needs discretization
%Rv_cont=1e-2*diag([8 8]);



%%%%%%%% PARAMETERS %%%%%%%%%%%
xmin = 90;
xmax = 110;
ymin = -70;
ymax = 70;
increment = 1;

xpinger1 = 100; ypinger1 = -50;
xpinger2 = 100; ypinger2 = 50;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%true initial state
        x0 =[0; %x
            0; %y
            0; %x_dot
            0; %y_dot
            xpinger1; %x1
            ypinger1; %y1
            xpinger2; %x2
            ypinger2]; %y2

        xhat0= [0;  % x
            0;  % y
            0;  % x_dot
            0;  % y_dot
            xpinger1;  % x1  (Pinger 1)
            ypinger1;  % y1  (Pinger 1)
            xpinger2;  % x2 (Pinger 2)
            ypinger2];% y2 (Pinger 2)
        
        
        
xtotal = xmin:increment:xmax;
ytotal = ymax:increment:ymax;
DivergeArray = zeros(length(xtotal)*increment,length(ytotal)*increment); % This defines an x,y position and the value at each x,y is the # of cases it failed        
        
        
figure('name','Divergence Map')
AXIS([xmin xmax ymin ymax])
hold on
AXIS square
rectangle ('position', [x0(5,end)-2.5,x0(6,end)-2.5, 5, 5], 'curvature', [1, 1],'LineWidth',2)
rectangle ('position', [x0(7,end)-2.5,x0(8,end)-2.5, 5, 5], 'curvature', [1, 1],'LineWidth',2)
rectangle ('position', [xmin,ymin, xmax-xmin, ymax-ymin],'LineWidth',2)
wbar = waitbar(0,'Plotting the end of humanity...');
daspect([1 1 1])


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



for x = xmin:increment:xmax
    waitbar((x-xmin)/(xmax-xmin),wbar);
    for y = ymin:increment:ymax
        %true initial state
        x0(1:4) =[x; %x
            y; %y
            0; %x_dot
            0]; %y_dot
            %10; %x1
            %50; %y1
            %10; %x2
            %-50]; %y2
            j=0;

        for guess = -.01:.01:.01
            for i = 1:3 %Because we have random forces we run this 3 times
                %initial state estimate
                xhat0(1:4) = [ x-x*guess;  % x
                    y-y*guess;  % y
                    0;  % x_dot
                    0];  % y_dot
                    %10;  % x1  (Pinger 1)
                    %50;  % y1  (Pinger 1)
                    %10;   % x2 (Pinger 2)
                    %-50];% y2 (Pinger 2)
                Divergence = ekfSLAMFunc(xhat0,x0,plotData,Rv_cont,divergence_criteria);
                if Divergence == 1
                    j = j+1;
                    %plot(x,y,'x')
                end

            end
        end
        %DivergeArray(x+length(xtotal),y-ymin+1) = j; %Counts # of diverged pts

        plot(x,y,'s',...
            'MarkerEdgeColor',color(j+1,:),...
            'MarkerFaceColor',color(j+1,:),...
            'MarkerSize',10)

    
    
    
    end
end






rectangle ('position', [x0(5,end)-2.5,x0(6,end)-2.5, 5, 5], 'curvature', [1, 1],'LineWidth',2)
rectangle ('position', [x0(7,end)-2.5,x0(8,end)-2.5, 5, 5], 'curvature', [1, 1],'LineWidth',2)
rectangle ('position', [xmin,ymin, xmax-xmin, ymax-ymin],'LineWidth',2)
close(wbar);
hold off
warning on all;

