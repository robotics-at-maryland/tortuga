m.x1 = -10;
m.sigx1 = 1;

m.y1 = 4;
m.sigy1 = 1;

m.x2 = 6;
m.sigx2 = 1;

m.y2 = 8;
m.sigy2 = 1;

m.phi1 = toRad(-40);
m.sigphi1 = toRad(5);

m.phi2 = toRad(25);
m.sigphi2 = toRad(5);

[r,e] = PlaneIntersection(m);