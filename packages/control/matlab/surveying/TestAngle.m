clc

m.x = 0;
m.sigx = .05;

m.y = 0;
m.sigy = .05;

m.phi = toRad(-30);
m.sigphi = toRad(5);

m.h = 1.5;
m.sigh = .05;

m.D = 3;
m.sigD = .05;

m.theta1 = toRad(80);
m.sigtheta1 = toRad(5);

[r,e] = AngleMethod(m);