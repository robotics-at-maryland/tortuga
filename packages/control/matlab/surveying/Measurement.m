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
       
       angleMethodValid = 0
       planeIntersectionValid = 0
       isActive = 1
       agrees = 1
       
       x = Null()
       sigx = Null()
       
       y = Null()
       sigy = Null()
       
       phi = Null()
       sigphi = Null()
       
       D = Null()
       sigD = Null()
       
       h = Null()
       sigh = Null()
       
       theta = Null()
       sigtheta = Null()
   end
end 
