%Influence of phi1 and phi2 on the uncertainty in x1,y1,x2,y2
clear
clc
syms phi1 phi2 dxobjdx1

[phi1,phi2] = meshgrid(toRad(-90):toRad(3):toRad(90), toRad(-90):toRad(3):toRad(90));
dxObjdx1 = (-1./(tan(phi1).*cot(phi2)-1)).^2;
dxObjdx2 = (1+(1./(tan(phi1).*cot(phi2)-1))).^2;
dxObjdy1 = (tan(phi1)./(tan(phi1).*cot(phi2)-1)).^2;
dxObjdy2 = (-tan(phi1)./(tan(phi1).*cot(phi2)-1)).^2;

dyObjdx1 = (-1./(tan(phi1)-tan(phi2))).^2;
dyObjdx2 = (1./(tan(phi1)-tan(phi2))).^2;
dyObjdy1 = (tan(phi1)./(tan(phi1)-tan(phi2))).^2;
dyObjdy2 = (1 - tan(phi1)./(tan(phi1)-tan(phi2))).^2;

Z = sqrt(dxObjdx1 + dxObjdx2 + dxObjdy1 + dxObjdy2 + dyObjdx1 + dyObjdx2 + dyObjdy1 + dyObjdy2);
surf(phi1,phi2, Z)
caxis([1 4])
axis([toRad(-90) toRad(90) toRad(-90) toRad(90) -1 5])

% Influence of phi1 and phi2 on uncertainty in phi1 and phi2

clear clc

dxObjdphi1 = (sin(phi2).*(csc(phi2-phi1)).^2).^2;
dxObjdphi2 = (sin(phi1).*(-csc(phi2-phi1)).^2).^2;
dyObjdphi1 = (cos(phi2).*(csc(phi2-phi1)).^2).^2;
dyObjdphi2 = (cos(phi1).*(csc(phi2-phi1)).^2).^2;

Z = sqrt(dxObjdphi1 + dxObjdphi2 + dyObjdphi1 + dyObjdphi2);

figure(2)
surf(phi1,phi2,Z)
caxis([1 5])
axis([toRad(-90) toRad(90) toRad(-90) toRad(90) -1 20])

%% Angle Method

% Influence of theta on uncertainty of L
n1 = 1;
n2 = 1.333;

syms theta

theta = 0:90;

dLdtheta1 = (sec(toRad(theta))).^2 + ((n1/n2).*cos(toRad(theta)))./((1-(((n1/n2)^2).*((sin(toRad(theta))).^2))).^(3/2));

plot(theta,dLdtheta1)
axis([0 90 0 20])

