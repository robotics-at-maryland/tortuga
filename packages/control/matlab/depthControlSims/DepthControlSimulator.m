% Main Code to run the Depth Control Simulation

clear

% set controlType to desired control (not case sensitive)
% 'p' for P Control
% 'pd' for PD Control
% 'oc' for observer control
% 'lqg' for linear quadratic gaussian control
% 'lqgi' for an observer controller with integral augmentation
controlType = 'lqgi';


%create a global variable so observer controllers can store variables
%between iterations
global x_hat;
global kp;
global kd;
global K;
global L;
global xHat4;
global A_c;
global B_c;
global C_c;
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

time=linspace(0,20,6000);
dt=time(2)-time(1);
%sensor delay time
%delay =0.05;

%desired depth
xd=2;
%desired depth_dot (downward velocity)


%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Control Constants %%%
%%%%%%%%%%%%%%%%%%%%%%%%%

if strcmp('PD',upper(controlType))==1
    %PD Control
     kp =40;
    kd =30;
elseif strcmp('P',upper(controlType))==1
    %P Control
    kp=8;
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
    LQG Controller - LQGIntegraterCoefficients
    A_c =1.0e+03 *[

  -0.001225000000000  -4.204200000000262  -0.007312500000000                   0;
                   0  -0.017425000000000   0.001000000000000                   0;
   0.000050000000000  -0.096980625000004  -0.000575000000000                   0;
  -0.001225000000000  -0.004200000000000  -0.007312500000000                   0];


B_c =1.0e+03 *[

   4.200000000000262;
   0.017425000000000;
   0.096980625000004;
                   0];


C_c =[0     0     0     1];
end    

%create array to store actual vehicle states
%let x=[x1 x2 x3]' where x1=x, x2=dx/dt, x3=d^2x/dt^3
x=zeros(3,length(time));

%INITIAL CONDITION
x(:,1)=[0 0 0]';

%create array to store measured vehicle states
%y_array=zeros(1,length(time));
y=x(1,1);
y_array=zeros(1,length(time));
y_array(1)=y;
%time_measured(1)=0;
%create array to store control thrusts
Fthrust=zeros(1,length(time));

%create an x_hat array here
x_hat_array = zeros(2,length(time));

%for varying measurement sampling frequencies
%j=1;

%depth error check margin to see if sub is above surface of water or submerged
depthCheckEpsilon=-0.001;%negative direction is out of water


for i=2:length(time)
    %integrate position equation
    %position eq x1=x1+x2*dt
    x(1,i)=x(1,i-1)+x(2,i-1)*dt;
    
    %simulate no velocity or acceleration if at surface
    %THIS CHECK IS DONE IN TWO OTHER LOCATIONS 
    if(x(1,i)<depthCheckEpsilon)
        x(:,i)=[0 0 0]';
    end
    
% for varying measurement sampling frequencies, appears to have magic numbers    
%     if mod(time(i),delay)< dt
%         j=j+1;
%         time_measured(j) = time(i);
%         %simulate sensor measurement of x1
%         %perfect sensor
%         %y_array(j)=x(1,i-1);
%         
%         %random noise sensor
%         %give a random number between -0.5 to 0.5
%         random = rand-0.5;
%         constant = .2;
%         y_array(j)=constant*random+x(1,i-1);
%         
%         %gaussian noise sensor
%         %constant = .25;
%         %random = randn(1);
%         %y_array(j)=constant*random+x(1,i-1);
%         y = y_array(j);
%         x2measured_array(j)=(y_array(j)+y_array(j-1))/delay;
%     end

    %simulate measurement
    y=x(1,i-1);
    %store measurement
    y_array(i)=y;

    %integrate velocity equation so long as we aren't above surface
    %velocity eq xdot1=dx/dt=x2
    if(x(1,i)<=depthCheckEpsilon)
        x(2,i)=0;
    else
        x(2,i)=x(2,i-1)+x(3,i-1)*dt;
    end
    
    %compute control laws
    if strcmp('PD',upper(controlType))==1
        %PD control
        % WE CAN'T ACTUALLY IMPLEMENT THIS
        Fthrust(i) = pdController(x(1,i-1),xd,x(2,i-1));
    elseif strcmp('P',upper(controlType))==1
        %P control
        Fthrust(i) = pController(y,xd,dt);
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
    
    %use control law in simulation of acceleration so long as we aren't
    %above the surface
    %acceleration eq xdot2=xdotdot1=d^2x/dt^2=-c/m+(Fthrust/m)
    if(x(1,i)<=depthCheckEpsilon)
        x(3,i)=0;
    else
        x(3,i)=-c/m*x(2,i)+(Fthrust(i)/m)+constant/m;
    end
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