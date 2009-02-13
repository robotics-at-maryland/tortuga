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
% 'lqgid' for a DISCRETE TIME observer controller with integral augmentation
controlType = 'LQGID';

%frequency of control/actuation loop
frequency=25;%Hz

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
tf=40;%final time
time=ti:1/frequency:tf;
dt=time(2)-time(1);
%sensor delay time
%delay =0.05;

%desired depth
xd=5;
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
% K_a = place(A_a,B_a,[-2 -2.1 -2.9]);
% L_a = (place(A_a',C_a',[-10 -10.1 -9.9]))';
A_c = 1.0e+04 * [-0.0006   -2.0242   -0.0248         0;
         0   -0.0029    0.0001         0;
    0.0000   -0.0283   -0.0001         0;
   -0.0006   -0.0244   -0.0248         0];
B_c = 1.0e+04 *[1.9998;
    0.0029;
    0.0283;
         0];
C_c = [0     0     0     1];
elseif strcmp('LQGID',upper(controlType))==1
    
    % Observer Controller w/ integral augmentation for DISCRETE TIME
    %K_a = place(A_a,B_a,[-2 -2.1 -2.9]);
    %L_a = (place(A_a',C_a',[-10 -10.1 -9.9]))';
    % designed for 40 Hz
%     
% A_c =[0.8462 -300.9784  -10.1814         0;
%     0.0000    0.4236    0.0170         0;
%     0.0011   -5.0480    0.9116         0;
%    -0.1517   42.5212   -5.3177    1.0000];
% B_c = [ 295.3628;
%     0.5763;
%     5.0445;
%   -48.1402];
% C_c = [ 0     0     0     1];
 

% K_a = place(A_a,B_a,[-1 -1.1 -0.9]);
 % L_a = (place(A_a',C_a',[-100 -100.1 -99.9]))'; 
%  A_c = 1.0e+04 *[0.0001    1.0884   -0.0494         0;
%     0.0000   -0.0000   -0.0000         0;
%     0.0000   -0.0010   -0.0000         0;
%    -0.0000    0.1246    0.0021    0.0001];
% B_c = 1.0e+04 * [-1.0884;
%     0.0001;
%     0.0010;
%    -0.1247];
% C_c =[ 0     0     0     1];
% K_a = place(A_a,B_a,[-2 -2.1 -2.9]);
% L_a = (place(A_a',C_a',[-15 -15.2 -15.4]))';
% A_c = [0.8419 -857.9510  -19.5524         0;
%     0.0000    0.2218    0.0138         0;
%     0.0011   -9.9348    0.8339         0;
%    -0.1515  123.2679   -4.5555    1.0000];
% B_c = [852.3424;
%     0.7782;
%     9.9313;
%  -128.8872];
% C_c = [0     0     0     1];

% K_a = place(A_a,B_a,[-2 -2.1 -2.2]);
% L_a = (place(A_a',C_a',[-100 -100.2 -100.4]))';
  
% A_c = 1.0e+04 *[0.0000    1.2217   -0.0469         0;
%     0.0000   -0.0000   -0.0000         0;
%     0.0000   -0.0009   -0.0000         0;
%    -0.0000    0.2932    0.0049    0.0001];
% B_c = 1.0e+04 *[-1.2221;
%     0.0001;
%     0.0009;
%    -0.2937];
% C_c = [0     0     0     1];

% K_a = place(A_a,B_a,[-0.7 -0.6 -0.5]);
% L_a = (place(A_a',C_a',[-7 -6 -5]))';
A_c = [0.9501 -114.3028   -2.8764         0;
    0.0000    0.4482    0.0277         0;
    0.0019   -2.8158    0.9146         0;
   -0.0481    3.5461   -0.2331    1.0000];
B_c =[  114.1389;
    0.5518;
    2.8157;
   -3.7101];
C_c = [0     0     0     1];
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
    %add gaussian white noise
    y=y+0.3*(rand-0.5);
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
        %LQG Controller w/ integral augmentation
        Fthrust(i) = ObserverController_Integrater(y,xd,dt);
        %store current x_hat from ObserverController in x_hat array
        x_hat_array(1,i) = xHat4(2)+xd;
        x_hat_array(2,i) = xHat4(3);
    elseif strcmp('LQGID',upper(controlType))==1
        %observer controller with integral augmentation 
        %designed for discrete time
        Fthrust(i) = ObserverControllerIntegratorDiscrete(y,xd);
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


figure (4)
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

figure (5)
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

figure (6)
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