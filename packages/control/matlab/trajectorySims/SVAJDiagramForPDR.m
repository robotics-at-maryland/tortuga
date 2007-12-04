%This creates and prints out SVAJ Diagrams for a 4-5-6-7 polynomial;
%function

clear all
clc


%beta = how long in time till its done
%h = total change in position

h = 5;
beta = 50;

hold on;


count =0;
% for time=0:.1:48
%     count = count+1;
%     s(count) = -1*h*   (25*                   (time/beta)^4  -60*                   (time/beta)^5  +10*                      (time/beta)^6 -16*                   (time/beta)^7);
%     v(count) = -1*h*   (25*4*    (1/beta)*    (time/beta)^3  -60*5*     (1/beta)   *(time/beta)^4  +10*6*       (1/beta)*    (time/beta)^5 -16*7*     (1/beta)*   (time/beta)^6);
%     a(count) = -1*h*   (25*4*3*  ((1/beta)^2)*(time/beta)^2  -60*5*4*   (1/beta)^2*(time/beta)^3   +10*6*5*     ((1/beta)^2)*(time/beta)^4 -16*7*6*  ((1/beta)^2)*(time/beta)^5);
%     j(count) = -1*h*   (25*4*3*2*((1/beta)^3)*(time/beta)^1  -60*5*4*3* (1/beta)^3*(time/beta)^2   +10*6*5*4*   ((1/beta)^3)*(time/beta)^3 -16*7*6*5*((1/beta)^3)*(time/beta)^4);
% 
% 
%    drawnow;
%     pause(.001);
% end

for time=0:.1:50
    count = count+1;
    s(count) = -1*h*   (35*                   (time/beta)^4  -84*                   (time/beta)^5  +70*                      (time/beta)^6 -20*                   (time/beta)^7);
    v(count) = -1*h*   (35*4*    (1/beta)*    (time/beta)^3  -84*5*     (1/beta)   *(time/beta)^4  +70*6*       (1/beta)*    (time/beta)^5 -20*7*     (1/beta)*   (time/beta)^6);
    a(count) = -1*h*   (35*4*3*  ((1/beta)^2)*(time/beta)^2  -84*5*4*   (1/beta)^2*(time/beta)^3   +70*6*5*     ((1/beta)^2)*(time/beta)^4 -20*7*6*  ((1/beta)^2)*(time/beta)^5);
    j(count) = -1*h*   (35*4*3*2*((1/beta)^3)*(time/beta)^1  -84*5*4*3* (1/beta)^3*(time/beta)^2   +70*6*5*4*   ((1/beta)^3)*(time/beta)^3 -20*7*6*5*((1/beta)^3)*(time/beta)^4);


   drawnow;
    pause(.001);
end



time = 0:.1:50;
   subplot(4,1,1); plot(time,s);
xlabel('time'); ylabel('position');
   subplot(4,1,2); plot(time,v);
xlabel('time'); ylabel('velocity');
   subplot(4,1,3); plot(time,a);
xlabel('time'); ylabel('acceleration');
   subplot(4,1,4); plot(time,j);
xlabel('time'); ylabel('jerk');
hold off