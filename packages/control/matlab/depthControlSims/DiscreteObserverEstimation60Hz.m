function [x_hat] = DiscreteObserverEstimation60Hz(y,xd,dt)
%only works for 60Hz
%to modify for another frequency, see discrete_observer.m


% Returns force output from the observer controller
Ad=[1 0.0166; 0 0.9905];
Bd=1e-3*[0.0069; 0.8294];
Cd=[1 0];
L=1.0e3*[0.021; 6.6196];

global K;
global x_hat;

% y_hat: estimation of y from previous state estimate
y_hat = Cd*x_hat;

% finding x_dot_prev from x_prev and y
x_dot_prev = Ad*x_hat + Bd*(-(K)*x_hat) + (L)*(y-y_hat);

% x_hat: calculate estimation of new x from x_prev and x_dot_prev
x_hat = x_hat + dt*x_dot_prev;


end