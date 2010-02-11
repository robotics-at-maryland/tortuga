%% Object at (-1,6)
clear
clc

map = Map(44,0);

% True Position
xobj = -1;
yobj = 6;


% Measurements and Errors
% A valid measurement for the plane intersection method requires real
% values for x,sigx,y,sigy,phi,sigphi.  There must be two valid plane
% intersection measurements to use the plane intersection method.  The
% angle method requires the aforementioned parameters and h, sigh, D, sigD,
% theta, sigtheta
% The measurement parameters are entered into arrays with each index
% corresponding to a single measurement.  That is, x(1) corresponds to y(1)
% and phi(1),etc. and x(2) corresponds to y(2), phi(2), etc.  This allows
% for the measurements to be created easily and selectively.
x = [-5 10];
sigx = [.3 .4];

y = [-1 2];
sigy = [.3 .3];

h = [1.3 1.3];
sigh = [.05 .05];

% There is only one value for depth since it is considered to be known
D = 4;
sigD = .05;

% phi can be solved for in terms of the other parameters
phi = asin(((xobj-x)./(sqrt((xobj-x).^2+(yobj-y).^2))));
sigphi = [toRad(4),toRad(3)];
phiA = asin(((xobj-x)./(sqrt((xobj-x).^2+(yobj-y).^2))));
% As far as I can tell, it is not possible to solve for theta in closed
% form so it must be iteratively estimated and checked.  In order to do
% this, set all of the uncertainties to 0 and use the angle method with
% this measurement.  Adjust the angle until the result agrees with the
% truth and then set the uncertainties to proper values.  The reason that
% the uncertainties had to be set to 0 in order to find the correct angle
% is because the noisy measurements are constructed based on those
% uncertainties.
theta = [toRad(71.9589)];
sigtheta = [toRad(3)];



% Single Angle Method Measurement

map.addObject('obj(-1,6)');
obj = map.objectMap('obj(-1,6)');
am = Measurement();
am.name = 'tmp';

% random('Normal', m, s) produces a pseudorandom value based on a
% normal (gaussian) distribution with mean m and standard deviation s
% This way of generating random measurements is more realistic than
% generating them from a uniform distribution.

i = 1; % this can be changed into a for loop to produce multiple measurements
am.x = random('Normal',x(i),sigx(i));
am.sigx = sigx(i);
am.y = random('Normal',y(i),sigy(i));
am.sigy = sigy(i);
am.h = random('Normal',h(i),sigh(i));
am.sigh = sigh(i);
am.D = D;
am.sigD = sigD;
am.phi = random('Normal',phiA(i),sigphi(i));
am.sigphi = sigphi(i);
am.theta = theta(i);
am.sigtheta = sigtheta(i);


rA = AngleMethod(am)

% Number of standard deviations away from true value
% This should be < 1 68% of the time
%                < 2 95% of the time
%                < 3 99% of the time
% If the results are considerably different than these, the error
% calculations will need to be adjusted.


% One Pair Plane Intersection

for i = 1:2
    pm = Measurement();
    pm.name = getTime();
    
    pm.x = random('Normal',x(i),sigx(i));
    pm.sigx = sigx(i);
    pm.y = random('Normal',y(i),sigy(i));
    pm.sigy = sigy(i);
    pm.phi = random('Normal',phi(i),sigphi(i));
    pm.sigphi = sigphi(i);
    
    obj.addMeasurement(pm);
end

rP = PlaneIntersection(obj.getMeasurementByIndex(1),obj.getMeasurementByIndex(2))

% Number of standard deviations away from true value
% This should be < 1 68% of the time
%                < 2 95% of the time
%                < 3 99% of the time
% If the results are considerably different than these, the error
% calculations will need to be adjusted.
%numstdvX = (xobj-r.xobj)/r.sigxobj;
%numstdvY = (yobj-r.yobj)/r.sigyobj;

% Combination of Angle and Plane Results
obj.addMeasurement(am);
obj.updateLocation();
rC = CalculatePosition(obj.getAllMeasurements)

% Number of standard deviations away from true value
% This should be < 1 68% of the time
%                < 2 95% of the time
%                < 3 99% of the time
% If the results are considerably different than these, the error
% calculations will need to be adjusted.
%numstdvX = (xobj-r.xobj)/r.sigxobj;
%numstdvY = (yobj-r.yobj)/r.sigyobj;

%% Object at (-5,19)
clear
clc

% Set up data
xobj = -5;
yobj = 19;


% Measurements and Errors
x = [-5 -18 10 -27];
sigx = [.4 .7 .5 .8];
%sigx = [0 0 0 0];

y = [-1 8 2 18];
sigy = [.2 .7 .3 .8];
%sigy = [0 0 0 0];

h = [1.3 1.3 1.3 1.3];
sigh = [.05 .05 .05 .05];
%sigh = [0 0 0 0];

D = 4;
sigD = .05;
%sigD = 0;

phi = asin(((xobj-x)./(sqrt((xobj-x).^2+(yobj-y).^2))));
sigphi = [toRad(3),toRad(3),toRad(3),toRad(3)];
%sigphi = [0 0 0 0];

theta = [toRad(85.20513),toRad(84.0842),toRad(85.90535),toRad(85.7545)];
sigtheta = [toRad(3),toRad(3),toRad(3),toRad(3)];
%sigtheta = [0 0 0 0];


% Generate Measurements

obj = Object('obj(-5,19)');


for i = 1:4
    
    m = Measurement();
    m.name = getTime();
    m.x = random('Normal',x(i),sigx(i));
    m.sigx = sigx(i);
    m.y = random('Normal',y(i),sigy(i));
    m.sigy = sigy(i);
    m.h = random('Normal',h(i),sigh(i));
    m.sigh = sigh(i);
    m.D = D;
    m.sigD = sigD;
    m.phi = random('Normal',phi(i),sigphi(i));
    m.sigphi = sigphi(i);
    m.theta = random('Normal',theta(i),sigtheta(i));
    m.sigtheta = sigtheta(i);
    
    obj.addMeasurement(m)
end

obj.updateLocation();
obj.location

% Number of standard deviations away from true value
% This should be < 1 68% of the time
%                < 2 95% of the time
%                < 3 99% of the time
% If the results are considerably different than these, the error
% calculations will need to be adjusted.
%numstdvX = (xobj-r.x)/r.sigx
%numstdvY = (yobj-r.y)/r.sigy
