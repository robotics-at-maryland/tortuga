function [s,v] = SVAJ_generator(deltat, act_depth,des_depth,vel_max)

%deltat = sensor update rate (this will be a constant)
%act_depth = the depth we are currently at 
%des_depth = the depth we want to achieve 

change_in_depth = des_depth - act_depth;


%total_time = initially this will be defaulted to 20 seconds
    %but later on we will get this to calculate itself based off of velocity?

%We want to output an array of solutions for the equations for s,v which has calculated gains



%Based off of the change_in_depth we will want to change the # of data
%points we take to get to the final position.





%--------------------------------------------------------%
%------------------Simulation only-----------------------%
%--------------------------------------------------------%
%using the max reasonable vel = 1 meter/second
%solve using v(total_time/2) = vel_max
p = 4;
q = 5;
r = 6;
s = 7;

[Cp Cq Cr Cs] = svaj_coef_generator(p,q,r,s);

%solve('0 = change_in_depth*(Cp*p*(1/total_time)*(.5)^(p-1)   +   Cq*q*(1/total_time)*(.5)^(q-1)+   Cr*r*(1/total_time)*(.5)^(r-1)   +   Cs*s*(1/total_time)*(.5)^(s-1)) - vel_max','total_time')
total_time = change_in_depth*(Cp*p*.5^(p-1)+Cq*q*.5^(q-1)+Cr*r*.5^(r-1)+Cs*s*.5^(s-1))/vel_max;
%----------------------------------------------------%
%----------------------------------------------------%
%----------------------------------------------------%

%total_time = change_in_depth*(35*4*.5^(4-1)-84*5*.5^(5-1)+70*6*.5^(6-1)-20*7*.5^(7-1))/vel_max;

step = 0;
for current_time=0:deltat:total_time
step=step+1;
s(step) = (change_in_depth)*  (Cp*               (current_time/total_time)^4  +Cq*               (current_time/total_time)^5  +Cr*               (current_time/total_time)^6 +Cs*               (current_time/total_time)^7) + act_depth;
v(step) = (change_in_depth)*  (Cp*4*(1/total_time)*(current_time/total_time)^3  +Cq*5*(1/total_time)*(current_time/total_time)^4  +Cr*6*(1/total_time)*(current_time/total_time)^5 +Cs*7*(1/total_time)*(current_time/total_time)^6);
end

%have it analyze the maximum velocity and keep it within constraints
%otherwise redevelop trajectory




