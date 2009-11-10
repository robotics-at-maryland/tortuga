classdef Measurement < handle
%MEASUREMENT Data structure for measured quantities
%
%   all properties are initialized to Null() object
%
%   x        - x cooreinate of measurement location
%   sigx     - error in x
%   y        - y coordinate of measurement location
%   sigy     - error in y
%   phi      - angle between measurement vector and magnetic North
%   sigphi   - error in phi
%   D        - depth of object
%   sigD     - error in D
%   h        - height measurement is taken from
%   sidh     - error in h
%   theta    - angle between vertical and measurement vector
%   sigtheta - error in theta
%

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
       alpha = toRad(-90)
       sigalpha = toRad(2)
   end
   
   methods
       function m = clone(obj)
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
       end
   end
end 
