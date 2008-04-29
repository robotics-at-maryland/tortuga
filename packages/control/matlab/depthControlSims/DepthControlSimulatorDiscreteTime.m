% Main Code to run the Discrete Time Depth Control Simulation
%
% dynamics are integrated using ODE45, ie continuous time
% 
% control computed discretely with zero order hold at a frequency set by
% the user
%

clear

% set controlType to desired control (not case sensitive)
% 'p' for P Control
% 'pd' for PD Control
% 'pid' for PID control
% 'oc' for observer control
% 'lqg' for linear quadratic gaussian control
% 'lqgi' for an observer controller with integral augmentation
controlType = 'oc';

%frequency of control/actuation loop
frequency=40;

%global variables for various controllers
global x_hat;%estimated state vector
global kp;%proportional gain
global kd;%derivative gain
global ki;%integral gain
global x_pid;%storage bin for PID controller
global K;%controller gain matrix
global L;%observer gain matrix
global xHat4;%larger estimated state vector
global A_c;%full dynamic controller A matrix
global B_c;%full dynamic controller B matrix
global C_c;%full dynamic controller C matrix

%global variables for dyanmics equations
% x(3,i)=-c/m*x(2,i)+(Fthrust(i)/m)+constant/m;
global c;%drag
global m;%mass
global constant;%gravitational force - buoyant force
global Ftemp;%zero order hold value of thrust force
global depthCheckEpsilon;%depth error check margin to see 
%if sub is above surface of water or submerged

x_hat=[0 0]';%initialize the global variable to rest conditions
xHat4 = [0 0 0 0]';

c=11.5;%vehicle drag parameter
m=20;%mass of sub in kg
p_water=998;%density of water in kg/m^3
g=9.8;%gravitational constant in m/s^2

% max thruster force in Newtons
max_thrust = 40; % 10 lbf

v_displaced=(1+m*g)/(p_water*g);%volume of water displaced by sub in m^3

%constant=(p_water*v_displaced-m)*g;
constant=g*(m-p_water*v_displaced);

%%%%%%%%%%%%%%%%
%%%% timing %%%%
%%%%%%%%%%%%%%%%

ti=0;%initial time
tf=20;%final time
time=ti:1/frequency:tf;
dt=time(2)-time(1);
%sensor delay time
%delay =0.05;

%desired depth
xd=15;
%desired depth_dot (downward velocity)


%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Control Constants %%%
%%%%%%%%%%%%%%%%%%%%%%%%%

if strcmp('P',upper(controlType))==1
    %P Control
    kp=8;
elseif strcmp('PD',upper(controlType))==1
    %PD Control
    kp =40;
    kd =30;
elseif strcmp('PID',upper(controlType))==1
    %PID Control
    kp =40;
    kd =30;
    ki =0.5;
    x_pid=[0 0]';%x_pid=[sum_error previous_position]'
elseif strcmp('OC',upper(controlType))==1
    %Observer Controller - ObserverControllerCoefficients.m
    % K=[16.4 8.1];
    % L=[44.1; 486.2];
      K=[9.5358 16.12];
      L=[43.525; 461.173];
elseif strcmp('LQG',upper(controlType))==1
    %LQG Controller - LQGCoefficients.m
     K = [3.162277660168380   4.585431495820535];
     L = [ 0.229271574791027;
   0.026282727503579];
elseif strcmp('LQGI',upper(controlType))==1
%    LQG Controller - LQGIntegraterCoefficients
    A_c =1.0e+03 *[

  -0.0012  -4.2042  -0.0073                   0;
                   0  -0.0174   0.001000000000000                   0;
   0.00010000  -0.097  -0.0006000000000                   0;
  -0.0012000000000  -0.004200000000000  -0.007300000000                   0];


B_c =1.0e+03 *[

   4.200;
   0.0174;
   0.097;
                   0];


C_c =[0     0     0     1];
end    

%create array to store actual vehicle states
%let x=[x1 x2 x3]' where x1=x, x2=dx/dt, x3=d^2x/dt^2
x=zeros(3,length(time));

%%%%%%%%%%%%%%%%%%%%%%%%%
%%% INITIAL CONDITION %%%
%%%%%%%%%%%%%%%%%%%%%%%%%
x(:,1)=[0 0 0]';


%initial measurement
y=x(1,1);
%measurement array
y_array=zeros(1,length(time));
y_array(1)=y;
%time_measured(1)=0;
%array to store control thrust history
Fthrust=zeros(1,length(time));

%state estimate array (would be made available to AI)
x_hat_array = zeros(2,length(time));

%for varying measurement sampling frequencies
%j=1;

%depth error check margin to see if sub is above surface of water or submerged
depthCheckEpsilon=-0.001;%negative direction is out of water


for i=2:length(time)

    %%%%%%%%%%%%%%%%%
    %% measurement %%
    %%%%%%%%%%%%%%%%%
    
    %simulate measurement
    y=x(1,i-1);
    %store measurement
    y_array(i)=y;

    %%%%%%%%%%%%%%%%%
    %%%% control %%%%
    %%%%%%%%%%%%%%%%%    
    
    %compute control laws
    if strcmp('P',upper(controlType))==1
        %P control
        Fthrust(i) = pController(y,xd,dt);
    elseif strcmp('PD',upper(controlType))==1
        %PD control
        % WE CAN'T ACTUALLY IMPLEMENT THIS
        Fthrust(i) = pdController(x(1,i-1),xd,x(2,i-1));
    elseif strcmp('PID',upper(controlType))==1
        %PID control
        Fthrust(i) = pidController(y,xd,dt);
    elseif strcmp('OC',upper(controlType))==1
        %Observer Control
        Fthrust(i) = ObserverController(y,xd,dt);
        %store current x_hat from ObserverController in x_hat array
        x_hat_array(1,i) = x_hat(1);
        x_hat_array(2,i) = x_hat(2);
    elseif strcmp('LQG',upper(controlType))==1
        %LQG Controller
        Fthrust(i) = ObserverController(y,xd,dt);
        %store current x_hat from ObserverController in x_hat array
        x_hat_array(1,i) = x_hat(1);
        x_hat_array(2,i) = x_hat(2);
    elseif strcmp('LQGI',upper(controlType))==1
        %LQG Controller
        Fthrust(i) = ObserverController_Integrater(y,xd,dt);
        %store current x_hat from ObserverController in x_hat array
        x_hat_array(1,i) = xHat4(2)+xd;
        x_hat_array(2,i) = xHat4(3);
    end
    
    
    %Account for thruster saturation
    if (Fthrust(i) > max_thrust)
        Fthrust(i) = max_thrust;
    elseif (Fthrust(i) < -max_thrust)
        Fthrust(i) =  -max_thrust;
    end

    %%%%%%%%%%%%%%%%%
    %%%% dynamics %%%
    %%%%%%%%%%%%%%%%%
    
    %so now we need to integrate from time(i-1) to time(i)
    
    Ftemp=Fthrust(i);
    
    [int_time, int_x]=ode45(@tortugaDepthDynamics,[time(i-1) time(i)],[x(:,i-1)]);
    x(:,i)=int_x(end,:)';
end


figure (1)
%plot actual and measured position
desired = xd*ones(1,length(time));
subplot(2,1,1)
plot(time,y_array,'.g',time,desired,'r',time,x(1,:),'b','LineWidth',1)
set(gca,'YDir','reverse')
legend('measured','desired','actual')
ylabel('x1 - depth')
subplot(2,1,2)
plot(time,x(2,:))
set(gca,'YDir','reverse')
ylabel('x2 - velocity')
xlabel('time')

figure (2)
%plot position and Control Signal
subplot(2,1,1)
plot(time,y_array,'.g',time,desired,'r',time,x(1,:),'b','LineWidth',1)
set(gca,'YDir','reverse')
legend('measured','desired','actual')
ylabel('x1 - depth')
subplot(2,1,2)
plot(time,Fthrust)
%set(gca,'YDir','reverse')
ylabel('u - control signal')
xlabel('time')

figure(3)
%plot actual and estimated position
subplot(2,1,1)
plot(time,x_hat_array(1,:),'g',time,x(1,:),'b','LineWidth',1)
set(gca,'YDir','reverse')
legend('estimated','actual')
ylabel('x1 - depth')
subplot(2,1,2)
plot(time,x_hat_array(2,:),'g',time,x(2,:),'b','LineWidth',1)
set(gca,'YDir','reverse')
ylabel('x2 - velocity')
xlabel('time')