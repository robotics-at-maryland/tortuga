function [true_measured_depth       time         depth_measured         x_hat_array] = DepthEstimationSamplingRateComparison(frequency)


clc
close all
% Main Code to run the Depth Control Simulation
% set controlType to 'p' (not case sensitive) for P control
controlType = 'pd';
% set controlType to 'pd' (not case sensitive) for PD control
% controlType = 'pd';

%create a global variable so observer controllers can store variables
%between iterations
global x_hat;
global kp;
global kd;
global K;
global L;
global W;
global A_c;
global B_c;
global C_c;
x_hat=[0 0]';%initialize the global variable to rest conditions
W = [0 0 0 0]';

c=11.5;
m=20;%mass of sub in kg
p_water=998;%density of water in kg/m^3
g=9.8;%gravitational constant in m/s^2

v_displaced=(1+m*g)/(p_water*g);%volume of water displaced by sub in m^3

constant=(p_water*v_displaced-m)*g;

time=linspace(0,10,frequency);

dt=time(2)-time(1);

%sensor delay time
%Setting as 10khz
delay = 1/frequency;

%Vector composed of the measured depth,depth_dot but with noise
depth_measured = zeros(2,length(time));
true_measured_depth = zeros(2,length(time));





%desired depth
xd=2;
K=[9.5358 16.12];
L=[43.525; 461.173];
%desired depth_dot (downward velocity)


%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Control Constants %%%
%%%%%%%%%%%%%%%%%%%%%%%%%

if strcmp('PD',upper(controlType))==1
    %PD Control
     kp =40;
     kd =30;
end

%create array to store actual vehicle states
%let x=[x1 x2 x3]' where x1=x, x2=dx/dt, x3=d^2x/dt^3
x=zeros(3,length(time));
%create array to store measured vehicle states
%y_array=zeros(1,length(time));
y=x(1,1);
y_array=y(1);
time_measured(1)=0;
%create array to store control thrusts
Fthrust=zeros(1,length(time));

%create an x_hat array here
x_hat_array = zeros(2,length(time));

j=1;


for i=2:length(time)
    %integrate position equation
    %position eq x1=x1+x2*dt
    x(1,i)=x(1,i-1)+x(2,i-1)*dt;
    
    
    
    %integrate velocity equation
    %velocity eq xdot1=dx/dt=x2
    x(2,i)=x(2,i-1)+x(3,i-1)*dt;
    
    
    if mod(time(i),delay)< dt
        j=j+1;
        time_measured(j) = time(i);
        %simulate sensor measurement of x1
        %perfect sensor
        %y_array(j)=x(1,i-1);
        
        %random noise sensor
        %give a random number between -0.5 to 0.5
        random = rand-0.5;
        constant = .2;
        y_array(j)=constant*random+x(1,i);
        
        %gaussian noise sensor
        %constant = .25;
        %random = randn(1);
        %y_array(j)=constant*random+x(1,i-1);
        y = y_array(j);
        depth_measured(1,i) = y;
        depth_measured(2,i)= (depth_measured(1,i)-depth_measured(1,i-1))/dt;    
        
        % This is the measured position with sensor noise       
        %    =(y_array(j)+y_array(j-1))/delay;
        % True meassured depth is an array of the position,velocity vectors
        % without noise
        true_measured_depth(1,i) = x(1,i);
        true_measured_depth(2,i) = x(2,i);
        
    end
    
           
    %compute control laws
    if strcmp('PD',upper(controlType))==1
        %PD control
        Fthrust(i) = pdController(x(1,i-1),xd,x(2,i-1));
    
        
     %This is calling the observer to estimate the values for comparison.   
     ObserverEstimation(y,xd,dt);
     x_hat_array(:,i) = x_hat; 
         
     
    %
    %use control law in simulation of acceleration
    %acceleration eq xdot2=xdotdot1=d^2x/dt^2=-c/m+(Fthrust/m)
    x(3,i)=-c/m+(Fthrust(i)/m);
    end
end

end

    