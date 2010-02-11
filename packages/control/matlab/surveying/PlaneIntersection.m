function [r,e] = PlaneIntersection(m1, m2)
%PLANEINTERSECTION Calculates the (X,Y) coordinates of an object
%
%   Consider an (underwater) object of known depth.  Choose two measurment
%   points of known positions with respect to some origin.  Connect each of
%   these measurement points to the object with lines.  Since the depth is
%   known, project the lines onto the surface at that depth.  The
%   intersection of these projections uniquely determines the position of
%   the object on the surface.  In order to find the intersection between
%   these lines, measure the angle between both projections and some
%   reference angle.
%
%   Notes: All Angles must be in Radians
%   
%   PARAMETER
%   m1, m2 - Measurement Objects
%       See Measurement.m
%
%   Output
%
%   r.xobj     - X position of the object
%   r.sigxobj  - error in xObj
%   r.yobj     - Y position of the object
%   r.sigyobj  - error in yObj
%
%   e.d*Objd*  - shows contributions of each partial to the error
%
%

r.xobj = m2.x + ( (m2.x-m1.x)-tan(m1.phi)*(m2.y-m1.y) ) / ( tan(m1.phi)*cot(m2.phi) - 1 );
r.yobj = m2.y + ( (m2.x-m1.x)-tan(m1.phi)*(m2.y-m1.y) ) / ( tan(m1.phi) - tan(m2.phi) );

e.dxObjdx1 = -1/(tan(m1.phi)*cot(m2.phi)-1);
e.dxObjdx2 = 1+(1/(tan(m1.phi)*cot(m2.phi)-1));
e.dxObjdy1 = tan(m1.phi)/(tan(m1.phi)*cot(m2.phi)-1);
e.dxObjdy2 = -tan(m1.phi)/(tan(m1.phi)*cot(m2.phi)-1);
e.dxObjdphi1 = (sec(m1.phi))^2*((m2.y-m1.y)-cot(m2.phi)*(m2.x-m1.x))/(cot(m2.phi)*tan(m1.phi)-1)^2;
e.dxObjdphi2 = (csc(m2.phi))^2*tan(m1.phi)*((m2.x-m1.x)-tan(m1.phi)*(m2.y-m1.y))/(cot(m2.phi)*tan(m1.phi)-1)^2;

e.dyObjdx1 = -1/(tan(m1.phi)-tan(m2.phi));
e.dyObjdx2 = 1/(tan(m1.phi)-tan(m2.phi));
e.dyObjdy1 = tan(m1.phi)/(tan(m1.phi)-tan(m2.phi));
e.dyObjdy2 = 1 - tan(m1.phi)/(tan(m1.phi)-tan(m2.phi));
e.dyObjdphi1 = (sec(m1.phi))^2*(tan(m2.phi)*(m2.y-m1.y)-(m2.x-m1.x))/(tan(m1.phi)-tan(m2.phi))^2;
e.dyObjdphi2 = (sec(m2.phi))^2*(-tan(m1.phi)*(m2.y-m1.y)-(m2.x-m1.x))/(tan(m1.phi)-tan(m2.phi))^2;

r.sigxobj = sqrt((m1.sigx*e.dxObjdx1)^2+(m2.sigx*e.dxObjdx2)^2+(m1.sigy*e.dxObjdy1)^2+(m2.sigy*e.dxObjdy2)^2+(m1.sigphi*e.dxObjdphi1)^2+(m2.sigphi*e.dxObjdphi2)^2);
r.sigyobj = sqrt((m1.sigx*e.dyObjdx1)^2+(m2.sigx*e.dyObjdx2)^2+(m1.sigy*e.dyObjdy1)^2+(m2.sigy*e.dyObjdy2)^2+(m1.sigphi*e.dyObjdphi1)^2+(m2.sigphi*e.dyObjdphi2)^2);

end
