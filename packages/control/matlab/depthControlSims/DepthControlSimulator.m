% Main Code to run the Depth Control Simulation
% set controlType to 'p' (not case sensitive) for P control
controlType = 'oc';
% set controlType to 'pd' (not case sensitive) for PD control
% controlType = 'pd';

%create a global variable so observer controllers can store variables
%between iterations
global x_hat;
x_hat=[0 0]';%initialize the global variable to rest conditions


m=20;%mass of sub in kg
p_water=998;%density of water in kg/m^3
g=9.8;%gravitational constant in m/s^2

v_displaced=(1+m*g)/(p_water*g);%volume of water displaced by sub in m^3

constant=(p_water*v_displaced-m)*g;

time=linspace(0,40,4000);
dt=time(2)-time(1);
%sensor delay time
delay =0.05;

%desired depth
xd=2;
%desired depth_dot (downward velocity)

%control gain
kp=40;%proportional control
kd=30;%derivative gain

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
j=1;



for i=2:length(time)
    %integrate position equation
    %position eq x1=x1+x2*dt
    x(1,i)=x(1,i-1)+x(2,i-1)*dt;
    
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
        y_array(j)=constant*random+x(1,i-1);
        
        %gaussian noise sensor
        %constant = .25;
        %random = randn(1);
        %y_array(j)=constant*random+x(1,i-1);
        y = y_array(j);
        x2measured_array(j)=(y_array(j)+y_array(j-1))/delay;
    end
    %integrate velocity equation
    %velocity eq xdot1=dx/dt=x2
    x(2,i)=x(2,i-1)+x(3,i-1)*dt;
    %compute control laws
    if strcmp('PD',upper(controlType))==1
        %PD control
        Fthrust(i) = pdController(kp,kd,x(1,i-1),xd,x(2,i-1));
    elseif strcmp('P',upper(controlType))==1
        %P control
        Fthrust(i) = pController(kp,y,xd);
    elseif strcmp('OC',upper(controlType))==1
        %Observer Control
        Fthrust(i) = ObserverController(y,xd,dt);
    end
    %use control law in simulation of acceleration
    %acceleration eq xdot2=xdotdot1=d^2x/dt^2=-c/m+(Fthrust/m)
    x(3,i)=-constant/m+(Fthrust(i)/m);
end
figure (1)
%plot actual and measured position
desired = ones(1,length(time));
subplot(2,1,1)
plot(time,xd,'r',time,x(1,:),'b',time_measured,y_array,'.g','LineWidth',1)
set(gca,'YDir','reverse')
legend('desired','actual','measured')
ylabel('x1 - depth')
subplot(2,1,2)
plot(time,x(2,:))
set(gca,'YDir','reverse')
ylabel('x2 - velocity')
xlabel('time')