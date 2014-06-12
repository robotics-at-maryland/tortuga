classdef Measurement < handle
%MEASUREMENT Data structure for measured quantities

   properties
       name = Null()
       
       associatedObject = Null()
       
       angleMethodValid = 0
       planeIntersectionValid = 0
       arcLengthValid = 0
       
       isActive = 1
       agrees = 1
       
       % initial x position
       x = Null()
       sigx = .3
       
       % initial y position
       y = Null()
       sigy = .3
       
       % angle between object and North
       phi = Null()
       sigphi = toRad(3)
       
       % depth of object
       D = Null()
       sigD = .05
       
       % height of measurement point
       h = Null()
       sigh = .05
       
       % angle of declination to object
       theta = Null()
       sigtheta = toRad(3)
       
       % arc length measured
       s = Null()
       sigs = .05
       
       % radius of circle
       r = 55
       sigr = .01
       
       % distance between circle edge and center
       d = -5
       sigd = .01
       
       % angle between local coordinate system and North
       alpha = toRad(133)
       sigalpha = toRad(2)
       
       % Bridge coordinates in Global frame
       bridgeNorthEndx = 0;
       bridgeNorthEndy = -5; 
       
       bridgeSouthEndx = 0;
       bridgeSouthEndy = -55;
       
       
       
   end
   
   methods
       function m = clone(obj) % returns a copy of the measurement
           m = Measurement();
           m.name = obj.name;
           m.angleMethodValid = obj.angleMethodValid;
           m.planeIntersectionValid = obj.planeIntersectionValid;
           m.isActive = obj.isActive;
           m.agrees = obj.agrees;
           m.x = obj.x;
           m.sigx = obj.sigx;
           m.y = obj.y;
           m.sigy = obj.sigy;
           m.phi = obj.phi;
           m.sigphi = obj.sigphi;
           m.D = obj.D;
           m.sigD = obj.sigD;
           m.h = obj.h;
           m.sigh = obj.sigh;
           m.theta = obj.theta;
           m.sigtheta = obj.sigtheta;
           m.s = obj.s;
           m.sigs = obj.sigs;
           m.r = obj.r;
           m.sigr = obj.sigr;
           m.d = obj.d;
           m.sigd = obj.sigd;
           m.alpha = obj.alpha;
           m.sigalpha = obj.sigalpha;
           m.bridgeNorthEndx = obj.bridgeNorthEndx;
           m.bridgeNorthEndy = obj.bridgeNorthEndy;
           m.bridgeSouthEndx = obj.bridgeSouthEndx;
           m.bridgeSouthEndy = obj.bridgeSouthEndy;
       end
   end
end 
