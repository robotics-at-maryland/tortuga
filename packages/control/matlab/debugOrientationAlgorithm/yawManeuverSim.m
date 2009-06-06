%
% this program simulates a yaw maneuver 
% this program was designed to debug the orientation determination
% algorithm
%
% written by Joseph Gland
% 2007-11-27
% (C) Robotics@Maryland - University of Maryland College Park
%


m_norm=0.2; %magnitude of Earth's magnetic field
m_inclination=66*pi/180; %magnetic inclination (rad) at College Park MD according to NOAA
g_norm=1; %magnitude of Earth's gravitational field


m_inertial=[m_norm*cos(m_inclination); 0; -m_norm*sin(m_inclination)];
g_inertial=[0; 0; -g_norm];

%beginning with sub aligned facing magnetic north, rotate positive yaw one
%complete circle (should point North, West, South, East, North)

theta=0:.1:2*pi;
%spin the sub (the measurements would appear to spin in the opposite
%direction of the sub's movement)
m_sim=m_inertial;
g_sim=g_inertial;

for i=1:length(theta)
    m_sim(:,i)=C3(theta(i))*m_inertial;
end

figure(1)
subplot(3,1,1)
plot(theta,m_sim(1,:))
subplot(3,1,2)
plot(theta,m_sim(2,:))
subplot(3,1,3)
plot(theta,m_sim(3,:))
