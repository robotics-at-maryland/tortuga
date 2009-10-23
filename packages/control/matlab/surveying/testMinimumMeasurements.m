%%
clear
clc
numstdvx = [];
numstdvy = [];
for n = 1:1
    sigphi = 2;
    sigxy = 1;
    
    x1 = -10;
    y1 = 5;
    
    x2 = 10;
    y2 = 5;
    
    xobj = 0;
    yobj = 15;
    
    phi1 = toDeg(asin((xobj-x1)/sqrt((xobj-x1)^2+(yobj-y1)^2)));
    phi2 = toDeg(asin((xobj-x2)/sqrt((xobj-x2)^2+(yobj-y2)^2)));
    
    m1 = Measurement();
    m1.name = 'm1';
    m1.x = x1;
    m1.sigx = 0;
    m1.y = y1;
    m1.sigy = 0;
    m1.phi = toRad(phi1);
    m1.sigphi = 0;
    %m1.h
    %m1.sigh
    m1.D = 3;
    m1.sigD = 0;
    %m1.theta
    %m1.sigtheta
    
    m2 = Measurement();
    m2.name = 'm2';
    m2.x = x2;
    m2.sigx = 0;
    m2.y = y2;
    m2.sigy = 0;
    m2.phi = toRad(phi2);
    m2.sigphi = 0;
    %m1.h
    %m1.sigh
    m2.D = 3;
    m2.sigD = 0;
    %m1.theta
    %m1.sigtheta
    
    [r1,e1] = PlaneIntersection(m1,m2);
    
    
    m1.name = 'm1';
    m1.x = random('Normal',x1,sigxy);
    m1.sigx = sigxy;
    m1.y = random('Normal',y1,sigxy);
    m1.sigy = sigxy;
    m1.phi = toRad(random('Normal',phi1,sigphi));
    m1.sigphi = toRad(sigphi);
    %m1.h
    %m1.sigh
    m1.D = 3.04;
    m1.sigD = .1;
    %m1.theta
    %m1.sigtheta
    
    
    m2.name = 'm2';
    m2.x = random('Normal',x2,sigxy);
    m2.sigx = sigxy;
    m2.y = random('Normal',y2,sigxy);
    m2.sigy = sigxy;
    m2.phi = toRad(random('Normal',phi2, sigphi));
    m2.sigphi = toRad(sigphi);
    %m1.h
    %m1.sigh
    m2.D = 3.04;
    m2.sigD = .1;
    %m1.theta
    %m1.sigtheta
    
    [r2,e2] = PlaneIntersection(m1,m2);
  
    r1
    r2
    
    delx = r2.x-r1.x;
    sigx = r2.sigx;
    difx = sigx-abs(delx);
    numstdvx(end+1) = delx/sigx
    
    dely = r2.y-r1.y;
    sigy = r2.sigy;
    dify = sigy-abs(dely);
    numstdvy(end+1) = dely/sigy
end
%%
figure(1)
hist(numstdvx);
figure(gcf)
figure(2)
hist(numstdvy);
figure(gcf)