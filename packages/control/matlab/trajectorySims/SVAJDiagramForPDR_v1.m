%This tests and prints out SVAJ Diagrams for a 4-5-6-7 polynomial;
close all;
clear all;
clc;

act_depth = input('Please input the current depth: ');
des_depth = input('Please inpute the desired depth: ');
% vel_max = input('Please inpute the desired max vel: ');

%----Later this will be the update time of the controller------
     deltat = .1;
%--------------------------------------------------------------

vel_max = .7291;

[s v] = SVAJ_generator(deltat,act_depth,des_depth,vel_max);
hold on;
s;
%v(150) 



  subplot(2,1,1); plot(s,'.');
set(gca,'YDir','reverse')
xlabel('step'); ylabel('position');
  subplot(2,1,2); plot(v,'.');
xlabel('step'); ylabel('velocity');
set(gca,'YDir','reverse')
%   subplot(4,1,3); plot(time,a);
%xlabel('time'); ylabel('acceleration');
%   subplot(4,1,4); plot(time,j);
%xlabel('time'); ylabel('jerk');
hold off;

%   s(count) = -1*h*   (35*                   (time/beta)^4  -84*                   (time/beta)^5  +70*                      (time/beta)^6 -20*                   (time/beta)^7);
%   v(count) = -1*h*   (35*4*    (1/beta)*    (time/beta)^3  -84*5*     (1/beta)   *(time/beta)^4  +70*6*       (1/beta)*    (time/beta)^5 -20*7*     (1/beta)*   (time/beta)^6);
%   a(count) = -1*h*   (35*4*3*  ((1/beta)^2)*(time/beta)^2  -84*5*4*   (1/beta)^2*(time/beta)^3   +70*6*5*     ((1/beta)^2)*(time/beta)^4 -20*7*6*  ((1/beta)^2)*(time/beta)^5);
%   j(count) = -1*h*   (35*4*3*2*((1/beta)^3)*(time/beta)^1  -84*5*4*3* (1/beta)^3*(time/beta)^2   +70*6*5*4*   ((1/beta)^3)*(time/beta)^3 -20*7*6*5*((1/beta)^3)*(time/beta)^4);
%end

