%% Object at (-1,6)
clear
clc

% Set up data
xobj = -1;
yobj = 6;


% Measurements and Errors
x = [-5 10];
sigx = [.3 .4];

y = [-1 2];
sigy = [.3 .3];

h = [1.3 1.3];
sigh = [.05 .05];

D = 4;
sigD = .05;

phi = asin(((xobj-x)./(sqrt((xobj-x).^2+(yobj-y).^2))));
sigphi = [toRad(4),toRad(3)];

theta = [toRad(71.9589)];
sigtheta = [toRad(3)];



% Single Angle Method Measurement

obj = Object('obj(-1,6)');
am = Measurement();

i = 1;
am.x = random('Normal',x(i),sigx(i));
am.sigx = sigx(i);
am.y = random('Normal',y(i),sigy(i));
am.sigy = sigy(i);
am.h = random('Normal',h(i),sigh(i));
am.sigh = sigh(i);
am.D = D;
am.sigD = sigD;
am.phi = random('Normal',phi(i),sigphi(i));
am.sigphi = sigphi(i);
am.theta = theta(i);
am.sigtheta = sigtheta(i);


r = AngleMethod(am)

numstdvX = (xobj-r.xObj)/r.sigxObj
numstdvY = (yobj-r.yObj)/r.sigyObj


% One Pair Plane Intersection

for i = 1:2
    pm = Measurement();
    
    pm.x = random('Normal',x(i),sigx(i));
    pm.sigx = sigx(i);
    pm.y = random('Normal',y(i),sigy(i));
    pm.sigy = sigy(i);
    pm.phi = random('Normal',phi(i),sigphi(i));
    pm.sigphi = sigphi(i);
    
    obj.addMeasurement(pm);
end

r = PlaneIntersection(obj.getMeasurementByIndex(1),obj.getMeasurementByIndex(2));
numstdvX = (xobj-r.x)/r.sigx;
numstdvY = (yobj-r.y)/r.sigy;

% Combination of Angle and Plane Results
obj.addMeasurement(am);

r = CalculatePosition(obj.getAllMeasurements);
numstdvX = (xobj-r.x)/r.sigx;
numstdvY = (yobj-r.y)/r.sigy;

%% Object at (-5,19)
clear
clc

% Set up data
xobj = -5;
yobj = 19;


% Measurements and Errors
x = [-5 -18 10 -27];
sigx = [.5 .8 .7 1];

y = [-1 8 2 18];
sigy = [.5 .6 .5 .7];

h = [1.3 1.3 1.3 1.3];
sigh = [.05 .05 .05 .05];

D = 4;
sigD = .05;

phi = asin(((xobj-x)./(sqrt((xobj-x).^2+(yobj-y).^2))));
sigphi = [toRad(4),toRad(3),toRad(3),toRad(3)];

theta = [toRad(85.20513),toRad(84.0842),toRad(85.90535),toRad(85.7545)];
sigtheta = [toRad(3),toRad(3),toRad(3),toRad(3)];



% Generate Measurements

obj = Object('obj(-5,19)');


for i = 1:4
    
    m = Measurement();
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

[r a] = CalculatePosition(obj.getAllMeasurements)

numstdvX = (xobj-r.x)/r.sigx
numstdvY = (yobj-r.y)/r.sigy
