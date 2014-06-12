function val = getForceValues(front, back, top, bottom, right, left)

%[front back top bottom right left] = getForceValues(F1, F2, F3, T1, T2, T3)

% forward left up roll pitch yaw

rSide = .5; %.5
rFront = 1.5; %1.5
rTop = .5; %.5

F1 = left + right;
F2 = top - bottom;
F3 = front + back;
T1 = 1*rTop*top + rTop*bottom;
T2 = -1*rFront*front + rFront*back;
T3 = -1*rSide*left + rSide*right;

val = [F1 F2 F3 T1 T2 T3];

%front = .5*F3 - (1/rFront)*T2;
%back = = .5*F3 + (1/rFront)*T2;

%left = .5*F1 - (1/rSide)*T3;
%right = .5*F1 + (1/rSide)*T3;

%top = .5*F2 - (1/rTop)*T1;
%bottom = .5*F2 + (1/rTop)*T1;