function [time    x] = simuguy(frequency)

%initial and final times (in seconds)
ti=0;
tf=10;

%frequency is in units of number points per second
time=linspace(ti,tf,tf*frequency);
dt = 1/frequency;
x = zeros(2,length(time));

%initial conditions
%position
x(1,1) = 0;
%velocity
x(2,1) = 100;


%model of system dynamics
A =[0    1.0000;0   -0.5750];

 %implementation of modified system dynamics 
 if frequency==1000
     A = (60/1000)*A
 end

for i=2:length(time)

x_dot = A*x(:,i-1);    
x(:,i)= x(:,i-1)+x_dot*dt;

end

end



