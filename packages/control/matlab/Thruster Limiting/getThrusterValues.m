function val = getThrusterValues(F1,   F2, F3, T1,  T2,  T3)
%                             forward left up roll pitch yaw

rSide = .5;
rFront = 1.5;
rTop = .5;

front = .5*F3 - (1/rFront)*T2/2;
back = .5*F3 + (1/rFront)*T2/2;

left = .5*F1 - (1/rSide)*T3/2;
right = .5*F1 + (1/rSide)*T3/2;

top = .5*F2 + (1/rTop)*T1/2;
bottom = -.5*F2 + (1/rTop)*T1/2;
val = [front back top bottom right left];