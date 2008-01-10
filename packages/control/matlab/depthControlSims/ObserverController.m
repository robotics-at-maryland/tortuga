function out = ObserverController(y,x_prev,mu_controller,mu_observer,dt)
% Returns force output from the observer controller
A = [0 1;0 0];
B = [0; 1];
C = [1 0];
K = place(A,B,mu_controller);
L = (place(A',C',mu_observer))';

% y_hat: estimation of y from previous state estimate
y_hat = x_prev(1);

% finding x_dot_prev from x_prev and y
x_dot_prev = A*x_prev + B*(-K*x_prev) + L*(y-y_hat);

% x_hat: calculate estimation of new x from x_prev and x_dot_prev
x_hat = x_prev + dt*x_dot_prev;

% Fthrust (U) : new control signal
Fthrust = -K*x_hat;

out = [x_hat;Fthrust];

end