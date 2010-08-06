function [Uactual] = limitU(U) 

calibfactor = .4368; %ForeThruster
calibfactor2 = .3612; %AftThruster

motorCount1 = U/calibfactor * 1023/27;
motorCount2 = U/calibfactor2 * 1023/27;

if motorCount1 >1024
    motorCount1 = 1023;
    
elseif motorCount1 < -1024
    motorCount1 = -1023;
end

    
if motorCount2 >1024
    motorCount2 = 1023;
    
elseif motorCount2 < -1024
    motorCount2 = -1023;
end

U1 = motorCount1*27/1023 * calibfactor;
U2 = motorCount2*27/1023 * calibfactor2;

Uactual = (U1 + U2)/2;